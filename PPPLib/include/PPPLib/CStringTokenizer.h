#ifndef NOVAC_PPPLIB_CSTRING_TOKENIZER_H
#define NOVAC_PPPLIB_CSTRING_TOKENIZER_H

#include <string>
#include <vector>

namespace novac
{
	class CStringTokenizer
	{
	public:
		CStringTokenizer(const char *text, const char* separators);

		const char* NextToken();

	private:
		std::string m_data;
		std::vector<char> m_separators;
	};
}  // namespace novac

#endif  // NOVAC_PPPLIB_CSTRING_TOKENIZER_H