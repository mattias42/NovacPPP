#include "StdAfx.h"
#include "stringtokenizer.h"

CStringTokenizer::CStringTokenizer(void)
{
	m_stringToInterpret = NULL;
	m_strLen = 0;
	this->m_stringPos = 0;
	memset(m_separators, 0, MAX_N_SEPARATORS * sizeof(char));
	m_separatorNum = 0;
	m_lastToken = NULL;
}

CStringTokenizer::CStringTokenizer(const char *string, const char *separators){
	if(string != NULL){
		m_strLen = (long)strlen(string);

		// allocate a new string as a copy of the string to tokenize
		this->m_stringToInterpret = new char[m_strLen + 2];
		sprintf(m_stringToInterpret, "%s", string);
	}else{
		m_stringToInterpret = NULL;
		m_strLen = 0;
	}

	memset(m_separators, 0, MAX_N_SEPARATORS * sizeof(char));
	if(separators != NULL){
		m_separatorNum = min(MAX_N_SEPARATORS, (int)strlen(separators));
		memcpy(m_separators, separators, m_separatorNum * sizeof(char));
	}else{
		m_separators[0] = ' ';
		m_separators[1] = '\t';
		m_separators[2] = '\n';
		m_separatorNum  = 3;
	}
	
	m_lastToken = NULL;
	m_stringPos = 0;
}

CStringTokenizer::~CStringTokenizer(void)
{
	if(m_stringToInterpret != NULL){
		delete m_stringToInterpret;
		m_stringToInterpret = NULL;
	}
	if(m_lastToken != NULL){
		delete m_lastToken;
		m_lastToken = NULL;
	}
	m_stringPos = 0;
	m_strLen = 0;
}

const char *CStringTokenizer::NextToken(){
	// check if we're out of the string
	if(m_stringPos >= m_strLen){
		return NULL;
	}
	if(m_lastToken != NULL){
		delete m_lastToken;
		m_lastToken = NULL;
	}

	int from = m_stringPos;
	int to	 = 0;

	// find the next quotation mark (this is NULL if none found)
	int nextQuote = m_strLen;
	char *pt = strchr(m_stringToInterpret + from, '\"');
	if(pt != NULL){
		nextQuote = (int)(pt - 	m_stringToInterpret);
	}	

	// find the next separator character
	int nextSep = m_strLen;
	for(int k = 0; k < m_separatorNum; ++k){
		char *pt = strchr(m_stringToInterpret + from, m_separators[k]);
		if(pt != NULL){
			nextSep = min(nextSep, pt - m_stringToInterpret);
		}
	}

	// check if the next separator is within a quotation
	if(nextQuote != m_strLen && (nextQuote < nextSep)){
		int previousQuote	= nextQuote;
		char *pt = strchr(m_stringToInterpret + previousQuote + 1, '\"');
		if(pt != NULL){
			nextQuote = (int)(pt - 	m_stringToInterpret);
		}

		nextSep = nextQuote + 1;
		for(int k = 0; k < m_separatorNum; ++k){
			char *pt = strchr(m_stringToInterpret + nextQuote, m_separators[k]);
			if(pt != NULL){
				nextSep = min(nextSep, pt - m_stringToInterpret);
			}
		}
	}

	m_lastToken = new char[nextSep - m_stringPos + 5]; // allocate a few characters extra, to be safe...
	memcpy(m_lastToken, m_stringToInterpret + from, (nextSep - m_stringPos) * sizeof(char));
	m_lastToken[nextSep - m_stringPos] = '\0';
	int N = strlen(m_lastToken);
	ASSERT(N <= (nextSep - m_stringPos + 1));
	
	// if there's any quotation signs inside this token then remove them...
	while(NULL != (pt = strchr(m_lastToken, '"'))){
		char *last = strchr(m_lastToken, '\0');
		while(pt < last){
			pt[0] = pt[1];
			++pt;
		}
	}
	
	// update the counter and return the string
	m_stringPos = nextSep + 1;
	return m_lastToken;
}
