#pragma once

/** The class CStringTokenizer takes care of tokenizing strings the way we want them
	This is primarily used when passing parameters to the program through the command-line
		since the normal CRT tokenizer will not be able to keep together strings containing spaces
	This class will return any string surrounded by ":s as one token (even if it contains spaces)
*/
class CStringTokenizer
{
public:
	CStringTokenizer(void);
	CStringTokenizer(const char* string, const char *separators);
	~CStringTokenizer(void);
	
	/** Returns the next token */
	const char *NextToken();
	
private:
	/** The string that we are to interpret */
	char	*m_stringToInterpret;
	
	/** The length of the string to interpret */
	long	m_strLen;
	
	/** Out current position in the string to interpret */
	long	m_stringPos;

	/** Local buffer holding the last token returned */
	char	*m_lastToken;

	/** The separator characters */
	static const int MAX_N_SEPARATORS = 256;
	char	m_separators[MAX_N_SEPARATORS];
	int		m_separatorNum;
};
