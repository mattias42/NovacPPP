#include "CString.h"
#include <stdarg.h>
#include <vector>
#include <algorithm> 
#include <functional> 
#include <cctype>
#include <locale>


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

	void CString::SetData(const char* data)
	{
		m_data = std::string(data);
	}
	void CString::SetData(const CString& data)
	{
		m_data = std::string(data.m_data);
	}
	void CString::SetData(const std::string& data)
	{
		m_data = std::string(data);
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

	CString& CString::AppendFormat(const char* format, ...)
	{
		std::vector<char> localBuffer(65535);

		va_list args;
		va_start(args, format);
		vsprintf_s(localBuffer.data(), localBuffer.size(), format, args);
		va_end(args);

		m_data = this->m_data + std::string{ localBuffer.data() };

		return *this;
	}

	CString& CString::Append(const CString& other)
	{
		this->m_data = this->m_data + other.m_data;
		return *this;
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

	CString CString::Tokenize(const char* tokenDelimiters, int& iStart) const
	{
		if (iStart >= m_data.size())
		{
			return CString("");
		}

		size_t strStart = (size_t)iStart;
		size_t foundPos = m_data.find_first_of(tokenDelimiters, strStart);

		if (std::string::npos == foundPos)
		{
			return CString("");
		}

		// 1. skip initial delimiters
		while (foundPos == strStart)
		{
			strStart += 1;
			foundPos = m_data.find_first_of(tokenDelimiters, strStart);

			if (strStart >= m_data.size() || foundPos >= m_data.size())
			{
				return CString("");
			}
		}

		// 2. extract the string
		return CString(m_data.substr(strStart, foundPos));
	}

	// trim from start
	static inline std::string &ltrim(std::string &s) {
		s.erase(s.begin(), std::find_if(s.begin(), s.end(),
			std::not1(std::ptr_fun<int, int>(std::isspace))));
		return s;
	}

	// trim from end
	static inline std::string &rtrim(std::string &s) {
		s.erase(std::find_if(s.rbegin(), s.rend(),
			std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
		return s;
	}

	// trim from both ends
	static inline std::string &trim(std::string &s) {
		return ltrim(rtrim(s));
	}

	void CString::Trim(const char* characters)
	{
		// trim from right
		size_t endpos = this->m_data.find_last_not_of(characters);
		size_t startpos = this->m_data.find_first_not_of(characters);
		if (std::string::npos != endpos)
		{
			this->m_data = this->m_data.substr(0, endpos + 1);
			this->m_data = this->m_data.substr(startpos);
		}

		// trim from left
		startpos = this->m_data.find_first_not_of(characters);
		if (std::string::npos != startpos)
		{
			this->m_data = this->m_data.substr(startpos);
		}
	}

	void CString::Trim()
	{
		this->m_data = trim(this->m_data);
	}

	CString& CString::MakeLower()
	{
		std::transform(this->m_data.begin(), this->m_data.end(), this->m_data.begin(), [](char c) { return char(::tolower(c)); });

		return (*this);
	}

	CString& CString::MakeUpper()
	{
		std::transform(this->m_data.begin(), this->m_data.end(), this->m_data.begin(), [](char c) { return char(::toupper(c)); });

		return (*this);
	}

	void CString::Remove(char character)
	{
		m_data.erase(std::remove(m_data.begin(), m_data.end(), character), m_data.end());
	}

	int CString::Find(char ch) const
	{
		const char* pt = strchr(m_data.c_str(), int(ch));
		if (nullptr == pt)
		{
			return -1;
		}
		else
		{
			return int(pt - m_data.c_str());
		}
	}

	int CString::Find(char ch, int pos) const
	{
		if (pos >= m_data.length())
		{
			return -1;
		}

		const char* c_str = m_data.c_str() + pos;

		const char* pt = strchr(c_str, int(ch));
		if (nullptr == pt)
		{
			return -1;
		}
		else
		{
			return int(pt - m_data.c_str());
		}
	}

	int CString::Find(const char* str) const
	{
		const char* pt = strstr(m_data.c_str(), str);

		if (nullptr == pt)
		{
			return -1;
		}
		else
		{
			return int(pt - m_data.c_str());
		}
	}

	int CString::ReverseFind(char ch) const
	{
		const char* pt = strrchr(m_data.c_str(), int(ch));
		if (nullptr == pt)
		{
			return -1;
		}
		else
		{
			return int(pt - m_data.c_str());
		}
	}


	CString operator+(const CString& str1, CString& other)
	{
		return CString(str1.ToStdString() + other.ToStdString());
	}
	CString operator+(const CString& str1, const char* other)
	{
		return CString(str1.ToStdString() + std::string(other));
	}
	CString operator+(const CString& str1, std::string other)
	{
		return CString(str1.ToStdString() + other);
	}

	char CString::GetAt(int index) const
	{
		if (index < 0 || index >= m_data.length())
		{
			throw std::invalid_argument("Invalid character retrieved in CString.");
		}
		else
		{
			return m_data.at(index);
		}
	}

	int CString::Compare(const CString& other) const
	{
		return m_data.compare(other.m_data);
	}
}