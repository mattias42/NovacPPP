#include "CString.h"
#include <stdarg.h>
#include <vector>

namespace novac
{
	CString::CString()
		:m_data{ "" }
	{
	}

	CString::CString(const CString& other)
		: m_data{ other.m_data }
	{

	}

	CString::CString(const char* other)
		: m_data{ other }
	{
	}

	CString::CString(const std::string& other)
		: m_data{ other }
	{

	}


	// --------------------- Formatting -----------------------

	void CString::Format(const char * format, ...)
	{
		std::vector<char> localBuffer(65535);

		va_list args;
		va_start(args, format);
		vsprintf_s(localBuffer.data(), localBuffer.size(), format, args);
		va_end(args);

		m_data = std::string{ localBuffer.data() };
	}

	CString CString::Left(int nChars) const
	{
		return Left((size_t)(nChars));
	}

	CString CString::Left(size_t nChars) const
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
		return Right((size_t)nChars);
	}

	CString CString::Right(size_t nChars) const
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