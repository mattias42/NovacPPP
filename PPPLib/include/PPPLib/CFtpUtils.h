#ifndef NOVAC_PPPLIB_CFTP_UTILS_H
#define NOVAC_PPPLIB_CFTP_UTILS_H

#include <PPPLib/CString.h>
#include <PPPLib/VolcanoInfo.h>

namespace novac
{
	class CFtpUtils
	{
	public:
		CFtpUtils(const CVolcanoInfo& volcanoes, int currentVolcano)
			: m_volcanoes(volcanoes), m_currentVolcano(currentVolcano)
		{
		}

		/** Splits a full directory path on the server (e.g. ftp://127.0.0.1/some/directory/path/ into the server
				component (ftp://127.0.0.1/) and the directory component (some/directory/path/) */
		void SplitPathIntoServerAndDirectory(const novac::CString& fullServerPath, std::string& server, std::string& directory);

	private:
		const CVolcanoInfo& m_volcanoes;
		const int m_currentVolcano;
	};
}  // namespace novac

#endif  // NOVAC_PPPLIB_CFTP_UTILS_H