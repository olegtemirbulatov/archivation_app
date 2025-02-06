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

enum class MODES
{
    NOT_CHOSEN,
    ENCODE,
    DECODE
};

bool push_shift(std::string &s, char c, size_t len);

void LZ77(const std::unique_ptr<std::string> &s, std::unique_ptr<std::forward_list<CodeNode>> &code, const size_t la_size, const size_t sb_size);

size_t LZ77length(const std::unique_ptr<std::forward_list<CodeNode>> &code);

size_t LZ77size(const std::unique_ptr<std::forward_list<CodeNode>> &code);

void LZ77decode(const std::unique_ptr<std::forward_list<CodeNode>> &code, std::unique_ptr<std::string> &res);

std::ostream &operator<<(std::ostream &os, CodeNode cn);
