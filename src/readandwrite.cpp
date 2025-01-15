#include <fstream>
#include <filesystem>
#include <string>
#include <iostream>
#include <functional>
#include <forward_list>
#include <memory>

// структура для сохранения результатов кодирования LZ77
struct CodeNode
{
    unsigned char beg;
    unsigned char len;
    char ch;
};

bool push_shift(std::string &s, char c, size_t len);

std::shared_ptr<std::forward_list<CodeNode>> LZ77(std::shared_ptr<std::string> s, size_t win_len = 255);

size_t LZ77length(const std::shared_ptr<std::forward_list<CodeNode>> code);

size_t LZ77size(const std::shared_ptr<std::forward_list<CodeNode>> code);

std::shared_ptr<std::string> LZ77decode(const std::shared_ptr<std::forward_list<CodeNode>> code);

std::ostream &operator<<(std::ostream &os, CodeNode cn);

// чтение всех файлов из директории
//void read_all_files(const std::string&& path, std::function<void (const std::string&&)> action /* <- сжатие/разжатие файла*/);

// запись обработанного файла в соответствующую директорию
//template <typename T>
//void write_file_in_directory(const std::string&& filename, T&& data);

void read_raw_file(const std::string& path, std::shared_ptr<std::string> buffer);

void write_bin_file(const std::string& path, const std::shared_ptr<std::forward_list<CodeNode>> code);

std::shared_ptr<std::forward_list<CodeNode>> read_bin_file(const std::string& path);


int main()
{
    std::shared_ptr<std::string> s {std::make_shared<std::string>()};
    read_raw_file("D://Portfolio//archivation//test.txt", s);
    std::shared_ptr<std::forward_list<CodeNode>> code {LZ77(s)};
    write_bin_file("D://Portfolio//archivation//test_encoded.bin", code);
    std::shared_ptr<std::forward_list<CodeNode>> code2 = read_bin_file("D://Portfolio//archivation//test_encoded.bin");

    std::cout << "Encoded string:" << std::endl;
    for (CodeNode& cn : *code2)
        std::cout << cn << ' ';
    std::cout << std::endl;

    if (code2)
    {
        std::cout << "Decoded string:" << std::endl;
        std::cout << *LZ77decode(code2) << '\n'
            << *s << std::endl;
        std::cout << s->size() << ' ' << LZ77size(code2) << std::endl;
    }
    return 0;
}

/*
Дальнейшие планы:
1. выделять память в куче - сделано!
2. сделать фрагментацию текста, т.к. символов может быть очень много. Архивировать поочереди каждый фрагмент по 128 байт
3. записывать сжатые данные в отдельный файл - сделано!
4. перевести в бинарный формат чтения записи - сделано!
5. архивирование других, не txt форматов
6. добавить cmake и git
*/




// чтение всех файлов из директории

//template <typename T>
//void read_all_files(const std::string&& path, std::function<void (const std::string&&)> action /* <- сжатие/разжатие файла*/)
/*{
    for (const auto& file : std::filesystem::directory_iterator(path))
    {
        if (std::filesystem::is_directory(file))
        {
            create_directory(); // <- создаем директорию для дальнейшей записи результата сжатия/разархивирования
            read_all_files(file.path().string(), action);
        }
        else
        {
            T data = action(file.path().string());
            write_file_in_directory(file.path().string(), data);
        }
    }
}

// запись обработанного файла в соответствующую директорию
template <typename T>
void write_file_in_directory(const std::string&& filename, T&& data)
{
    std::ofstream ofile(filename, std::ios::binary);
    if (!ofile.is_open())
    {
        std::cout << "Error!" << std::endl;
        return;
    }

    if (sizeof(data)) // на случай, если файл пуст
    {
        int len = sizeof(data) / sizeof(data[0]);
        ofile.write((char*)&T, len);
    }

    ofile.close();
}
*/

// reading original data
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

// reading binary file with encoded data
std::shared_ptr<std::forward_list<CodeNode>> read_bin_file(const std::string& path)
{
    std::ifstream file(path, std::ios::binary);
    if (file.is_open())
    {
        std::shared_ptr<std::forward_list<CodeNode>> code {std::make_shared<std::forward_list<CodeNode>>()};
        auto it = code->before_begin();
        CodeNode cn;
        while (!file.eof())
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

// return true if first symbol were erased
bool push_shift(std::string &s, char c, size_t len) 
{
    if (s.size() < len)
    {
        s.push_back(c);
        return false;
    }
    std::move(std::next(s.begin()), s.end(), s.begin());
    s.back() = c;
    return true;
}

// encoding original string
std::shared_ptr<std::forward_list<CodeNode>> LZ77(std::shared_ptr<std::string> s, size_t win_len)
{
    std::shared_ptr<std::forward_list<CodeNode>> res {std::make_shared<std::forward_list<CodeNode>>()};
    auto it = res->before_begin();
    std::string win, buf;
    win.reserve(win_len);
    buf.reserve(win_len);
    CodeNode next;
    size_t saved_win_len = 0;
    for (char c : *s)
    {
        buf.push_back(c);
        size_t pos;
        next.ch = c;
        // cout<<win<<' '<<buf<<' '<<saved_win_len<<'\n';
        if ((pos = win.rfind(buf)) != std::string::npos)
        {
            next.beg = saved_win_len - pos;
            next.len = buf.size();
            if (push_shift(win, c, win_len))
                saved_win_len--; // shift
        }
        else
        {
            it = res->insert_after(it, next);
            buf.resize(0);
            next.beg = 0;
            next.len = 0;
            push_shift(win, c, win_len);
            saved_win_len = win.size();
        }
    }
    if (next.len != 0)
    {
        next.len--;
        res->insert_after(it, next);
    }
    return res;
}

// get length of original string
size_t LZ77length(const std::shared_ptr<std::forward_list<CodeNode>> code)
{ 
    size_t len = 0;
    for (const CodeNode &cn : *code)
        len += cn.len + 1;
    return len;
}

// get size of coded data
size_t LZ77size(const std::shared_ptr<std::forward_list<CodeNode>> code)
{ 
    return sizeof(CodeNode) * std::distance(code->begin(), code->end());
}

// decoding encoded string function
std::shared_ptr<std::string> LZ77decode(const std::shared_ptr<std::forward_list<CodeNode>> code)
{
    std::shared_ptr<std::string> res {std::make_shared<std::string>()};
    res->reserve(LZ77length(code)); // can not be
    for (CodeNode& cn : *code)
    {
        for (size_t i = res->size() - cn.beg, e = i + cn.len; i != e; ++i)
            res->push_back(res->at(i));
        res->push_back(cn.ch);
    }
    return res;
}

std::ostream &operator<<(std::ostream &os, CodeNode cn)
{
    return os << '<' << int(cn.beg) << ',' << int(cn.len) << ',' << cn.ch << '>';
}