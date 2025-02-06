#include <string>
#include <string_view>
#include <forward_list>
#include <memory>
#include <iostream>

#include "lz77.hpp"

// return true if first symbol were erased from look ahead window
bool push_shift(std::string &la_window, char c, size_t len)
{
    if (la_window.size() < len)
    {
        la_window.push_back(c);
        return false;
    }
    std::move(std::next(la_window.begin()), la_window.end(), la_window.begin());
    la_window.back() = c;
    return true;
}

// encoding original string
void LZ77(
    const std::unique_ptr<std::string> &s,
    std::unique_ptr<std::forward_list<CodeNode>> &code,
    const size_t la_size,
    const size_t sb_size)
{
    auto it = code->before_begin();
    std::string win, buf;
    win.reserve(la_size);
    buf.reserve(sb_size);
    CodeNode next;
    size_t saved_win_len = 0;
    for (char c : *s)
    {
        buf.push_back(c);
        size_t pos;
        next.ch = c;
        // std::cout << win << ' ' << buf << ' ' << saved_win_len << '\n';
        if ((pos = win.rfind(buf)) != std::string::npos)
        {
            next.beg = saved_win_len - pos;
            next.len = buf.size();
            if (push_shift(win, c, la_size))
                saved_win_len--; // shift
        }
        else
        {
            it = code->insert_after(it, next);
            buf.resize(0);
            next.beg = 0;
            next.len = 0;
            push_shift(win, c, la_size);
            saved_win_len = win.size();
        }
    }
    if (next.len != 0)
    {
        next.len--;
        code->insert_after(it, next);
    }
}

// get length of original string
size_t LZ77length(const std::unique_ptr<std::forward_list<CodeNode>> &code)
{
    size_t len = 0;
    for (const CodeNode &cn : *code)
        len += cn.len + 1;
    return len;
}

// get size of coded data
size_t LZ77size(const std::unique_ptr<std::forward_list<CodeNode>> &code)
{
    return sizeof(CodeNode) * std::distance(code->begin(), code->end());
}

// decoding encoded string function
void LZ77decode(
    const std::unique_ptr<std::forward_list<CodeNode>> &code,
    std::unique_ptr<std::string> &res)
{
    res->reserve(LZ77length(code)); // can not be
    for (CodeNode &cn : *code)
    {
        for (size_t i = res->size() - cn.beg, e = i + cn.len; i != e; ++i)
        {
            res->push_back(res->at(i));
        }
        res->push_back(cn.ch);
    }
}

// for debugging
std::ostream &operator<<(std::ostream &os, CodeNode cn)
{
    return os << '<' << int(cn.beg) << ',' << int(cn.len) << ',' << cn.ch << '>';
}