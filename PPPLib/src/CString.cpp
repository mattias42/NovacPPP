#include "CString.h"
#include <stdarg.h>

namespace novac
{
    CString::CString()
        :m_data{""}
    {
    }

    CString::CString(const CString& other)
        : m_data{other.m_data}
    {

    }

    CString::CString(const char* other)
        : m_data{other}
    {
    }

    CString::CString(const std::string& other)
        : m_data{other}
    {

    }


    // --------------------- Formatting -----------------------

    inline void CString::Format(const char * format, ...)
    {
        va_list args;
        va_start(args, format);
        vprintf(format, args);
        va_end(args);
    }

    CString CString::Left(int nChars) const
    {
        if (nChars >= m_data.size())
        {
            return CString(*this);
        }
        else
        {
            return CString(m_data.substr(0, nChars));
        }
    }

    CString CString::Right(int nChars) const
    {
        if (nChars >= m_data.size())
        {
            return CString(*this);
        }
        else
        {
            return CString(m_data.substr(m_data.size() - nChars, nChars));
        }
    }


}