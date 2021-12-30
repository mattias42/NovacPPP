#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do this in one cpp file
#include "catch.hpp"
#include <PPPLib/Logging.h>
#include <PPPLib/VolcanoInfo.h>


novac::CVolcanoInfo g_volcanoes;   // <-- A list of all known volcanoes


void ShowMessage(const novac::CString& message)
{
    std::cout << message.std_str() << std::endl;
}
void ShowMessage(const char message[])
{
    std::cout << message << std::endl;
}
void ShowMessage(const novac::CString& message, novac::CString /* connectionID */)
{
    std::cout << message << std::endl;
}
void ShowMessage(const std::string& message)
{
    std::cout << message << std::endl;
}

/** Appends an error message to the logs */
void ShowError(const novac::CString& message)
{
    std::cout << message.std_str() << std::endl;
}
void ShowError(const char message[])
{
    std::cout << message << std::endl;
}