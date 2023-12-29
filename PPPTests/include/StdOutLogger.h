#pragma once
#include <PPPLib/Logging.h>
#include <iostream>

class StdOutLogger : public ILogger
{
public:
    virtual void Debug(const std::string& message) override
    {
        std::cout << message << std::endl;
    }

    virtual void Information(const std::string& message) override
    {
        std::cout << message << std::endl;
    }

    virtual void Error(const std::string& message) override
    {
        std::cout << "Error: " << message << std::endl;
    }
};
