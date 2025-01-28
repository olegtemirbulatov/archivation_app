#include <string>
#include <string_view>
#include <forward_list>
#include <memory>
#include <iostream>

#include "lz77.hpp"

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