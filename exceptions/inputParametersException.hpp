#pragma once

#include <exception>
#include <string>

class InputParametersException : public std::exception
{
public:
    InputParametersException(const std::string& message): message{message} {}
    const char* what() const noexcept override
    {
        return message.c_str();
    }
private:
    std::string message;
};