#include <memory>
#include <list>
#include <filesystem>
#include <memory>
#include <fstream>
#include <iostream>

#include "readandwrite.hpp"

void read_raw_file(const std::string& path, std::shared_ptr<std::string> buffer)
{
    std::ifstream file(path, std::ios::binary);
    if (file.is_open())
    {
        std::string line;
        while (std::getline(file, line)) 
        {
            buffer->append(line);
        }
        file.close();
    }
    else
    {
        std::cout << "Error while opening reading file!" << std::endl;
    }
}

// writing encoded data to binary file
void write_bin_file(const std::string& path, const std::shared_ptr<std::forward_list<CodeNode>> code)
{
    std::ofstream file(path, std::ios::binary);
    if (file.is_open())
    {
        size_t size = std::distance(code->begin(), code->end());
        file.write((char*)&size, sizeof(size_t));
        for (CodeNode& cn : *code)
        {
            file.write((char*)&cn.beg, sizeof(cn.beg));
            file.write((char*)&cn.len, sizeof(cn.len));
            file.write((char*)&cn.ch, sizeof(cn.ch));
        }
        file.close();
    }
    else
    {
        std::cout << "Error while opening writing file!" << std::endl;
    }
}

// writing decoded data to the original format file
void write_orig_file(const std::string& path, std::shared_ptr<std::string> s)
{
    std::ofstream file(path);    
    if (file.is_open())
    {
        file << *s << std::endl;
        file.close();
    }
    else
    {
        std::cout << "Error while opening writing file!" << std::endl;
    }
}

// reading binary file with encoded data
std::shared_ptr<std::forward_list<CodeNode>> read_bin_file(const std::string& path)
{
    std::ifstream file(path, std::ios::binary);
    if (file.is_open())
    {
        std::shared_ptr<std::forward_list<CodeNode>> code {std::make_shared<std::forward_list<CodeNode>>()};
        auto it = code->before_begin();
        CodeNode cn;
        size_t size;
        file.read((char*)&size, sizeof(size_t));
        for (size_t i = 0; i < size; ++i)
        {
            file.read((char*)&cn.beg, sizeof(cn.beg));
            file.read((char*)&cn.len, sizeof(cn.len));
            file.read((char*)&cn.ch, sizeof(cn.ch));
            it = code->insert_after(it, cn);
        }
        file.close();
        return code;
    }
    else
    {
        std::cout << "Error while opening reading file!" << std::endl; // exception
        return nullptr;
    }
}