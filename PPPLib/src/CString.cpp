#include "PPPLib/CString.h"
#include <stdarg.h>
#include <vector>
#include <algorithm> 
#include <functional> 
#include <cctype>
#include <locale>
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

	std::string Right(const std::string& input, size_t nChars) {
		return input.substr(input.size() - nChars, nChars);
	}

	std::string Left(const std::string& input, size_t nChars) {
		return input.substr(0, nChars);
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
		if (iStart >= m_data.size() || iStart < 0)
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
				iStart = -1;
				return CString(m_data.substr(strStart, foundPos));
			}
		}

		// 2. update the position
		iStart = (int)foundPos;

		// 3. extract the string
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

	void CleanString(const CString &in, CString &out) {
		std::vector<char> buffer(strlen(in) + 2);
		
		for (int it = 0; it < in.GetLength(); ++it)
		{
			buffer[it] = in.GetAt(it);
		}
		CleanString(buffer.data(), out);
	}

	void CleanString(const char *in, CString &out) {
		out.Format("");
		for (unsigned int it = 0; it < strlen(in); ++it) {
			if ((unsigned char)in[it] >= 32) {
				out.AppendFormat("%c", in[it]);
			}
		}
	}

	CString SimplifyString(const CString& in) {
		static CString str;

		// Clean the string for non-printable characters
		CleanString(in, str);

		// Make a local copy of the string
		unsigned long L = (unsigned long)strlen(str);
		char *buffer = new char[L + 2];
		for (unsigned int it = 0; it < L; ++it) {
			buffer[it] = str.GetAt(it);
		}
		buffer[L] = 0;

		// Check all characters in the string
		for (unsigned long i = 0; i < L; ++i) {
			// 1. Replace spaces, commas and dots with underscores
			if (buffer[i] == ' ' || buffer[i] == ',' || buffer[i] == '.') {
				buffer[i] = '_';
				continue;
			}

			// 2. Convert the character to lower-case
			buffer[i] = char(tolower(buffer[i]));

			// 3. Remove paranthesis...
			if (buffer[i] == '(' || buffer[i] == '[' || buffer[i] == '{' || buffer[i] == ')' || buffer[i] == ']' || buffer[i] == '}') {
				for (unsigned long j = i; j < L - 1; ++j) {
					buffer[j] = buffer[j + 1];
				}
				buffer[L - 1] = '\0';
				--L;
				i = i - 1;
				continue;
			}

			// 4. Check if there's any accent on the character
			if ((unsigned char)buffer[i] <= 127) {
				continue;
			}

			char c = buffer[i];

			if (c == 'á' || c == 'à' || c == 'â' || c == 'ä' || c == 'å')
				buffer[i] = 'a';
			else if (c == 'ç' || c == 'c')
				buffer[i] = 'c';
			else if (c == 'é' || c == 'è' || c == 'ê' || c == 'ë')
				buffer[i] = 'e';
			else if (c == 'í' || c == 'ì' || c == 'î' || c == 'ï')
				buffer[i] = 'i';
			else if (c == 'ó' || c == 'ò' || c == 'ô' || c == 'ö')
				buffer[i] = 'o';
			else if (c == 'ú' || c == 'ù' || c == 'ü' || c == 'û')
				buffer[i] = 'u';
			else if (c == 'ñ')
				buffer[i] = 'n';
		}

		// copy the buffer to a CString
		str.Format("%s", buffer);

		delete[] buffer;
		return str;
	}

	int Equals(const CString &str1, const CString &str2) {
		return (0 == _strnicmp(str1, str2, std::max(strlen(str1), strlen(str2))));
	}

	int Equals(const CString &str1, const CString &str2, size_t nCharacters) {
		return (0 == _strnicmp(str1, str2, std::min(nCharacters, std::max(strlen(str1), strlen(str2)))));
	}

}
