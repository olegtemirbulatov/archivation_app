#include <iostream>
#include <string>
#include <string_view>
#include <forward_list>
#include <algorithm>

struct CodeNode
{
    unsigned char beg;
    unsigned char len;
    char ch;
};

bool push_shift(std::string &s, char c, size_t len) // return true if first symbol were erased
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

std::forward_list<CodeNode> LZ77(std::string_view s, size_t win_len = 255)
{
    std::forward_list<CodeNode> res;
    auto it = res.before_begin();
    std::string win, buf;
    win.reserve(win_len);
    buf.reserve(win_len);
    CodeNode next;
    size_t saved_win_len = 0;
    for (char c : s)
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
            it = res.insert_after(it, next);
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
        res.insert_after(it, next);
    }
    return res;
}

size_t LZ77length(const std::forward_list<CodeNode> &code)
{ // get length of original string
    size_t len = 0;
    for (const CodeNode &cn : code)
        len += cn.len + 1;
    return len;
}

size_t LZ77size(const std::forward_list<CodeNode> &code)
{ // get size of coded data
    return sizeof(CodeNode) * std::distance(code.begin(), code.end());
}

std::string LZ77decode(const std::forward_list<CodeNode> &code)
{
    std::string res;
    res.reserve(LZ77length(code)); // can not be
    for (CodeNode cn : code)
    {
        for (size_t i = res.size() - cn.beg, e = i + cn.len; i != e; ++i)
            res += res[i];
        res += cn.ch;
    }
    return res;
}

std::ostream &operator<<(std::ostream &os, CodeNode cn)
{
    return os << '<' << int(cn.beg) << ',' << int(cn.len) << ',' << cn.ch << '>';
}

int main()
{
    std::string s;
    // std::getline(std::cin, s);
    s = "helolohelololololololololololololo"; // для тестирования
    auto code = LZ77(s, 10);
    for (CodeNode cn : code)
        std::cout << cn << ' ';
    std::cout << std::endl;
    std::cout << LZ77decode(code) << '\n'
         << s << std::endl;
    std::cout << s.size() << ' ' << LZ77size(code) << std::endl;
    return 0;
}