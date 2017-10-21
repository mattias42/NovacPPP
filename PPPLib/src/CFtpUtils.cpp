#include "PPPLib/CFtpUtils.h"

namespace novac
{
	void CFtpUtils::SplitPathIntoServerAndDirectory(const novac::CString& fullServerPath, std::string& server, std::string& directory)
	{
		novac::CString subString;

		int indexOfSlash[128];
		int nSlashesFound = 0; // the number of slashes found in the 'serverDir' - path

		if (fullServerPath.Find("ftp://") != -1) {
			indexOfSlash[0] = 5;
		}
		else {
			indexOfSlash[0] = 0;
		}

		while (-1 != (indexOfSlash[nSlashesFound + 1] = fullServerPath.Find('/', indexOfSlash[nSlashesFound] + 1))) {
			++nSlashesFound;
		}
		subString.Format(fullServerPath.Left(indexOfSlash[1]));

		server = (subString.Right(subString.GetLength() - indexOfSlash[0] - 1)).std_str();

		if (nSlashesFound == 1)
		{
			directory = m_volcanoes.GetSimpleVolcanoName(m_currentVolcano).std_str() + "/";
		}
		else
		{
			directory = (fullServerPath.Right(fullServerPath.GetLength() - indexOfSlash[1] - 1)).std_str();
		}
	}
}