#include <fstream>
#include <filesystem>
#include <string>
#include <iostream>
#include <functional>
#include <forward_list>
#include <list>
#include <memory>

#include "lz77.hpp"
#include "readandwrite.hpp"

namespace fs = std::filesystem;

std::string get_writing_path(const std::string& original_path, const std::string& new_path, const fs::directory_entry &dir_entry)
{
    // протестировано
    return dir_entry.path().string().replace(0, original_path.length(), new_path, 0, new_path.length());
}

void process_all_files(const std::string& path, const std::string& command)
{
    if (command == "encode")
    {
        std::string new_path = path + "_encoded";
        // check if directory excists
        fs::create_directory(new_path);

        for (auto const& dir_entry : fs::recursive_directory_iterator(path))
        {
            std::string writing_path = get_writing_path(path, new_path, dir_entry);

            if (fs::is_directory(dir_entry))
            {
                fs::create_directory(writing_path);
            }
            else
            {
                std::shared_ptr<std::string> s {std::make_shared<std::string>()};
                read_raw_file(dir_entry.path().string(), s);
                std::shared_ptr<std::forward_list<CodeNode>> code {LZ77(s)};
                write_bin_file(writing_path, code);
            }
        }
    }

    else if (command == "decode")
    {
        std::string new_path = path + "_decoded";
        // check if directory excists
        fs::create_directory(new_path);

        for (auto const& dir_entry : fs::recursive_directory_iterator(path))
        {
            std::string writing_path = get_writing_path(path, new_path, dir_entry);

            if (fs::is_directory(dir_entry))
            {
                fs::create_directory(writing_path);
            }
            else
            {
                std::shared_ptr<std::forward_list<CodeNode>> code { read_bin_file(dir_entry.path().string()) };
                std::shared_ptr<std::string> s {std::make_shared<std::string>()};
                s = LZ77decode(code);
                write_orig_file(writing_path, s);
            }
        }
    }
}

int main()
{
    std::string path_to_encode = "D:\\Portfolio\\archivation\\examples";
    process_all_files(path_to_encode, "encode");

    std::string path_to_decode = "D:\\Portfolio\\archivation\\examples_encoded";
    process_all_files(path_to_decode, "decode");

    return 0;
}


/*
Дальнейшие планы:
1. выделять память в куче - сделано!
2. сделать фрагментацию текста, т.к. символов может быть очень много. Архивировать поочереди каждый фрагмент по 128 байт (или больше)
3. записывать сжатые данные в отдельный файл - сделано!
4. перевести в бинарный формат чтения записи - сделано!
5. архивирование других, не txt форматов
6. добавить cmake и git - сделано!
7. добавить архивирование всей директории - сделано
8. добавить многопоточность - ?
9. добавить qt
*/









