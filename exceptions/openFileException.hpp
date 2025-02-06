#pragma once

#include <exception>
#include <string>

class OpenFileException : public std::exception
{
public:
    OpenFileException(const std::string& message): message{message} {}
    const char* what() const noexcept override
    {
        return message.c_str();
    }
private:
    std::string message;
};