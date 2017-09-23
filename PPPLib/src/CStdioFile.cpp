#include "CStdioFile.h"

namespace novac
{
	CStdioFile::CStdioFile()
	{

	}

	bool CStdioFile::Open(const char* fileName, unsigned nOpenFlags)
	{
		return Open(fileName, nOpenFlags, nullptr);
	}

	bool CStdioFile::Open(const char* fileName, unsigned nOpenFlags, CFileException* ex)
	{
		m_f.open(fileName, nOpenFlags);

		return m_f.is_open();
	}


	void CStdioFile::Close()
	{
		m_f.close();
	}

	bool CStdioFile::ReadString(char* destination, unsigned int nMax)
	{
		if (!m_f.is_open() || m_f.eof())
		{
			return false;
		}
		else
		{
			m_f.getline(destination, nMax);
			return true;
		}
	}
}