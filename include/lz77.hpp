#pragma once

#include <string>
#include <memory>
#include <forward_list>

struct CodeNode
{
    unsigned char beg;
    unsigned char len;
    char ch;
};

bool push_shift(std::string &s, char c, size_t len);

std::shared_ptr<std::forward_list<CodeNode>> LZ77(std::shared_ptr<std::string> s, size_t win_len = 256);

size_t LZ77length(const std::shared_ptr<std::forward_list<CodeNode>> code);

size_t LZ77size(const std::shared_ptr<std::forward_list<CodeNode>> code);

std::shared_ptr<std::string> LZ77decode(const std::shared_ptr<std::forward_list<CodeNode>> code);

std::ostream &operator<<(std::ostream &os, CodeNode cn);
