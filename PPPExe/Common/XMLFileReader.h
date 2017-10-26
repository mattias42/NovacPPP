#pragma once

#include "../stdafx.h"

#include <PPPLib/DateTime.h>
#include <PPPLib/CString.h>
#include <PPPLib/CStdioFile.h>

namespace FileHandler
{
	class CXMLFileReader
	{
	public:
		CXMLFileReader(void);
		~CXMLFileReader(void);

		/** Retrieve the next token from the xml file */
		char *NextToken();

		/** Retrieves the value of the given attribute from the current token 
			@return NULL if this attribute does not exist or the current token is not
				a valid element */
		const char * GetAttributeValue(const novac::CString &label);		

		/** General parsing of a single, simple string item */
		int Parse_StringItem(const novac::CString &label, novac::CString &string);

		/** General parsing of a single, simple float item */
		int Parse_FloatItem(const novac::CString &label, double &number);

		/** General parsing of a single, simple integer item */
		int Parse_IntItem(const novac::CString &label, int &number);

		/** General parsing of a single, simple long integer item */
		int Parse_LongItem(const novac::CString &label, long &number);

		/** General parsing of a single, simple long integer item */
		int Parse_IPNumber(const novac::CString &label, BYTE &ip0, BYTE &ip1, BYTE &ip2, BYTE &ip3);

		/** General parsing of a date */
		int Parse_Date(const novac::CString &label, novac::CDateTime &datum);

		/**set the opened file pointer*/
		void SetFile(novac::CStdioFile* file);

		/**variables*/

		/** A handle to the file to read from. */
		novac::CStdioFile *m_File;
		
		/** The name of the currently opened file. For debugging reasons */
		novac::CString m_filename;
		
		/** The number of lines that has been read from the file */
		long nLinesRead;

		/** The tokenizer */
		char *szToken;

		/** The string that was read from the file */
		char szLine[4096];
		
		/** String representing the value of the last retrieved attribute */
		char attributeValue[4096];

	protected:
		/** Pointer to the next token. Should only be modified by 'NextToken()' */
		char *m_tokenPt;

	};
}