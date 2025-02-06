#include <memory>
#include <list>
#include <filesystem>
#include <memory>
#include <fstream>
#include <iostream>
#include <iterator>

#include "readandwrite.hpp"
#include "openFileException.hpp"

void read_original_file(const std::string& path, std::unique_ptr<std::string>& buffer)
{
    std::ifstream file(path, std::ios::binary);
    if (file.is_open())
    {
        std::uintmax_t filesize = fs::file_size(path);
        buffer->reserve(filesize);
        
        char c;
        while (filesize)
        {
            file.read(reinterpret_cast<char*>(&c), sizeof(char));
            buffer->push_back(c);
            --filesize;
        }
        file.close();
    }
    else
    {
        throw OpenFileException("Error while opening reading file");
    }
}

// writing encoded data to binary file
void write_compressed_file(const std::string& path, const std::unique_ptr<std::forward_list<CodeNode>>& code)
{
    std::ofstream file(path, std::ios::binary);
    if (file.is_open())
    {
        for (CodeNode& cn : *code)
        {
            file.write((char*)&cn, sizeof(cn));
        }
        file.close();
    }
    else
    {
        throw OpenFileException("Error while opening writing file");
    }
}

// writing decoded data to the original format file
void write_decompressed_file(const std::string& path, const std::unique_ptr<std::string>& s)
{
    std::ofstream file(path, std::ios::binary);    
    if (file.is_open())
    {
        for (char c : *s)
        {
            file.write(reinterpret_cast<char*>(&c), sizeof(char));
        }
        file.close();
    }
    else
    {
        throw OpenFileException("Error while opening writing file");
    }
}

// reading binary file with encoded data
void read_compressed_file(
    const std::string& path, 
    std::unique_ptr<std::forward_list<CodeNode>>& code
)
{
    std::ifstream file(path, std::ios::binary);
    if (file.is_open())
    {
        auto it = code->before_begin();
        CodeNode cn;

        std::uintmax_t filesize = fs::file_size(path);

        while (filesize)
        {
            file.read((char*)&cn, sizeof(cn));
            it = code->insert_after(it, cn);
            filesize -= 3;
        }

        file.close();
    }
    else
    {
        throw OpenFileException("Error while opening reading file");
    }
}

std::string get_path_to_write(
    const std::string& original_path, 
    const std::string& new_path, 
    const fs::directory_entry &dir_entry
)
{
    return dir_entry.path().string()
    .replace(0, original_path.length(), new_path, 0, new_path.length());
}

void process_all_files(
    const std::string& input_filepath, 
    const std::string& output_filepath, 
    const MODES mode, 
    const unsigned char la_size, 
    const unsigned char sb_size
)
{
    // checking if input directory excists
    if (!fs::exists(input_filepath))
    {
        throw OpenFileException("No such file or directory: " + input_filepath);
    }

    fs::create_directory(output_filepath);

    if (mode == MODES::ENCODE)
    {
        for (auto const& dir_entry : fs::recursive_directory_iterator(input_filepath))
        {
            std::string writing_path = get_path_to_write(input_filepath, output_filepath, dir_entry);

            if (fs::is_directory(dir_entry))
            {
                fs::create_directory(writing_path);
            }
            else
            {
                std::unique_ptr<std::string> s {std::make_unique<std::string>()};
                read_original_file(dir_entry.path().string(), s);
                std::unique_ptr<std::forward_list<CodeNode>> code {
                    std::make_unique<std::forward_list<CodeNode>>() 
                };
                LZ77(s, code, la_size, sb_size);
                write_compressed_file(writing_path, code);
            }
        }
    }

    else if (mode == MODES::DECODE)
    {        
        for (auto const& dir_entry : fs::recursive_directory_iterator(input_filepath))
        {
            std::string writing_path = get_path_to_write(
                input_filepath, 
                output_filepath, 
                dir_entry
            );

            if (fs::is_directory(dir_entry))
            {
                fs::create_directory(writing_path);
            }
            else
            {
                std::unique_ptr<std::forward_list<CodeNode>> code { 
                    std::make_unique<std::forward_list<CodeNode>>() 
                };
                read_compressed_file(dir_entry.path().string(), code);
                std::unique_ptr<std::string> s {std::make_unique<std::string>()};
                LZ77decode(code, s);
                write_decompressed_file(writing_path, s);
            }
        }
    }
}
