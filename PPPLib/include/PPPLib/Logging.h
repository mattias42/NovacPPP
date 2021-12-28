#pragma once

#include <PPPLib/MFC/CString.h>

/** Appends an information / warning message to the logs.
    Implementations are in Common.cpp (using Poco) */
void ShowMessage(const novac::CString& message);
void ShowMessage(const char message[]);
void ShowMessage(const novac::CString& message, novac::CString connectionID);
void ShowMessage(const std::string& message);
