#pragma once

#ifndef FTPSERVER_DATADOWNLOAD_H
#define FTPSERVER_DATADOWNLOAD_H

#include <afxtempl.h>

#include "FTPCom.h"

namespace Communication{
	class CFTPServerConnection
	{
	public:
		CFTPServerConnection(void);
		~CFTPServerConnection(void);
		
		// -----------------------------------------------------------
		// ---------------------- PUBLIC DATA ------------------------
		// -----------------------------------------------------------


		// -----------------------------------------------------------
		// --------------------- PUBLIC METHODS ----------------------
		// -----------------------------------------------------------

		/** Downloads .pak - files from the given FTP-server 
			@return 0 on successful connection and completion of the list
		*/
		int DownloadDataFromFTP(const CString &server, const CString &username,
			const CString &password, CList <CString, CString &> &pakFileList);

		/** Downloads a single file from the given FTP-server 
			@return 0 on successful connection and completion of the download
		*/
		int DownloadFileFromFTP(const CString &remoteFileName, const CString &localFileName,
			const CString &username, const CString &password);

		/** Retrieves the list of files in a given directory on the FTP-server
			@return 0 on successful connection and completion of the download
		*/
		int DownloadFileListFromFTP(const CString &serverDir, CList <CString, CString&> &fileList,
			const CString &username, const CString &password);

		/** Uploads result-files to the given FTP-server 
			@return 0 on success otherwise non-zero
		*/
		int UploadResults(const CString &server, const CString &username, 
			const CString &password, CList <CString, CString &> &fileList);

private:
		// -----------------------------------------------------------
		// ---------------------- PRIVATE DATA -----------------------
		// -----------------------------------------------------------
		
	
		// -----------------------------------------------------------
		// --------------------- PRIVATE METHODS ---------------------
		// -----------------------------------------------------------

	};
}

#endif