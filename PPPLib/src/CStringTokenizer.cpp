#include "CStringTokenizer.h"

namespace novac
{
	CStringTokenizer::CStringTokenizer(const char *text, const char* separators)
		: m_data(text)
	{
		const char* pt = separators;
		while (pt != 0)
		{
			m_separators.push_back(*pt);
			++pt;
		}
	}

	const char* CStringTokenizer::NextToken()
	{

		return "bad";
	}
}
