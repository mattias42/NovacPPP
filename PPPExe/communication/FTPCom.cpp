#include "ftpcom.h"
#include "../Common/common.h"
// #include "Winsock2.h"

using namespace Communication;

CFTPCom::CFTPCom(void)
{
	m_FtpConnection = NULL;
	m_InternetSession = NULL;
}

CFTPCom::~CFTPCom(void)
{
}
//Connect to FTP server
//return 0 - fail
//return 1 - success
//return 2 - ftp address parsing problem
//return 3 - can not connect to internet
//return 4 - ftp exception
int CFTPCom::Connect(novac::CString siteName, novac::CString userName, novac::CString password, BOOL mode)
{
	INTERNET_PORT  port = 21;
	DWORD dwServiceType = AFX_INET_SERVICE_FTP;
	novac::CString strServer;
	novac::CString strObject;
	novac::CString urlAddress = novac::CString("ftp://") + siteName;
	m_FTPSite.Format("%s", (const char*)siteName);

	// If already connected, then re-connect
	if (m_FtpConnection != NULL)
		m_FtpConnection->Close();
	delete m_FtpConnection;
	m_FtpConnection = NULL;

	CString c_siteName((const char*)siteName);
	CString c_strObject((const char*)strObject);
	if (!AfxParseURL(siteName, dwServiceType, c_siteName, c_strObject, port))
	{
		// try adding the "ftp://" protocol		

		if (!AfxParseURL(urlAddress, dwServiceType, c_siteName, c_strObject, port))
		{
			m_ErrorMsg = "Can not parse  ftp address";
			ShowMessage(m_ErrorMsg);
			return 2;
		}
	}
	if(m_InternetSession == NULL)
		m_InternetSession = new CInternetSession(NULL, 1, INTERNET_OPEN_TYPE_DIRECT,NULL,
			NULL,0);//INTERNET_FLAG_NO_CACHE_WRITE );

	// Alert the user if the internet session could
	// not be started and close app
	if (!m_InternetSession)
	{
		m_ErrorMsg = "FAIL TO CONNECT INTERNET";
		ShowMessage(m_ErrorMsg);
		return 3;
	}
	
	//	int nTimeout = AfxGetApp()->GetProfileInt("Settings", "ConnectionTimeout", 30);
	int nTimeout = 30 * 60; // timeout = 30 minutes
	if (dwServiceType == INTERNET_SERVICE_FTP)// && !siteName.IsEmpty())
	{
		try
			{
				m_InternetSession->SetOption(INTERNET_OPTION_CONNECT_TIMEOUT, nTimeout * 1000);
				m_InternetSession->SetOption(INTERNET_OPTION_RECEIVE_TIMEOUT, nTimeout * 1000);
				m_InternetSession->SetOption(INTERNET_OPTION_SEND_TIMEOUT, nTimeout * 1000);
			
				m_FtpConnection = m_InternetSession->GetFtpConnection(siteName,
											userName,password,21,mode);
				m_ErrorMsg.Format("CONNECTED to FTP server: %s", (const char*)siteName);
				ShowMessage(m_ErrorMsg);
				return 1;
			}
		catch (CInternetException* pEx)
		{
			// catch errors from WinINet
			TCHAR szErr[255];
			if (pEx->GetErrorMessage(szErr, 255))
			{
				m_ErrorMsg.Format("FTP error from connecting %s: %s", (const char*)m_FTPSite, (const char*)szErr);
				ShowMessage(m_ErrorMsg);
			}
			else
			{
				m_ErrorMsg.Format("FTP exception");
				ShowMessage(m_ErrorMsg);
			}
			pEx->Delete();

			m_FtpConnection = NULL;
			return 4;
		}
	}
	return 1;
}
int CFTPCom::Disconnect()
{
	if (m_FtpConnection != NULL)
		m_FtpConnection->Close();
	delete m_FtpConnection;
	m_FtpConnection = NULL;
	if(m_InternetSession !=NULL)
		m_InternetSession->Close();
	delete m_InternetSession;
	m_InternetSession = NULL;
	return 1;
}


int CFTPCom::UpdateFile(novac::CString localFile, novac::CString remoteFile)
{
	int result = 0;
	if(m_FtpConnection == NULL){
		ShowMessage("ERROR: Attempted to update file using FTP while not connected!");
		return 0;
	}

	// If the file exists, remove it first...
	if(FindFile((novac::CString&)remoteFile) == TRUE)
		m_FtpConnection->Remove(remoteFile);

	// Upload the file
	result = m_FtpConnection->PutFile(localFile, remoteFile);
	return result;
}
BOOL CFTPCom::DownloadAFile(novac::CString remoteFile, novac::CString fileFullName)
{
	BOOL result = FALSE;
	novac::CString msg;

	// Check that we're connected...
	if(m_FtpConnection == NULL){
		ShowMessage("ERROR: Attempted to download file using FTP while not connected!");
		return FALSE;
	}

	msg.Format("Trying to download %s", (const char*)fileFullName);
	ShowMessage(msg);

	try
	{
		// Try to download the file
		result = m_FtpConnection->GetFile(remoteFile, fileFullName, FALSE);

		if(0 == result){ // this means something went wrong
			int ftpError = GetLastError();
			if(ftpError != 0)
			{
				msg.Format("FTP error happened when downloading %s from %s: %d", (const char*)fileFullName, (const char*)m_FTPSite, ftpError);
				ShowMessage(msg);
				DWORD code;
				DWORD size_needed = 0;
				InternetGetLastResponseInfo(&code,NULL,&size_needed);
				char *message = (char*)malloc(size_needed + 1);
				InternetGetLastResponseInfo(&code,message,&size_needed);
				msg.Format("Error message :%s",message);
				ShowMessage(msg);
			}
		}else{
			// SUCCESS!!
			msg.Format("Finish downloading %s", (const char*)fileFullName);
			ShowMessage(msg);
			return result;
		}
	}
	catch (CInternetException* pEx)
	{
		// catch errors from WinINet
		TCHAR szErr[255];
		if (pEx->GetErrorMessage(szErr, 255))
		{
			m_ErrorMsg.Format("FTP error happened when downloading %s from %s: %s", (const char*)fileFullName, (const char*)m_FTPSite, szErr);
			ShowMessage(m_ErrorMsg);
		}
		else
		{
			m_ErrorMsg.Format("FTP exception");
			ShowMessage(m_ErrorMsg);
		}
		pEx->Delete();

		m_FtpConnection = NULL;
	}
	return result;
}

int CFTPCom::UploadFile(novac::CString localFile, novac::CString remoteFile)
{
	int result;

	// Check that we are connected
	if(m_FtpConnection == NULL){
		ShowMessage("ERROR: Attempted to upload file using FTP while not connected!");
		return 0;
	}

	// See if we can find the file on the remote computer, if so
	//	then we can't upload it...
	if(FindFile((novac::CString&)remoteFile) == TRUE)
		return 1;

	try{
		// Try to upload the file
		result = m_FtpConnection->PutFile(localFile,remoteFile);

	}catch(CInternetException *pEx){
		// catch errors from WinINet
		TCHAR szErr[255];
		if (pEx->GetErrorMessage(szErr, 255))
		{
			m_ErrorMsg.Format("FTP error happened when uploading %s to %s: %s", (const char*)localFile, (const char*)m_FTPSite, (const char*)szErr);
			ShowMessage(m_ErrorMsg);
		}
		else
		{	
			m_ErrorMsg.Format("FTP exception");
			ShowMessage(m_ErrorMsg);
		}
		pEx->Delete();
	}
	return result;
}

int CFTPCom::CreateDirectory(novac::CString remoteDirectory)
{
	// Check that we are connected
	if(m_FtpConnection == NULL){
		ShowMessage("ERROR: Attempted to create directory using FTP while not connected!");
		return 0;
	}

	int result = m_FtpConnection->CreateDirectory(remoteDirectory);
	return result;	
}

BOOL CFTPCom::SetCurDirectory(novac::CString curDirName)
{
	if(m_FtpConnection == NULL){
		ShowMessage("ERROR: Attempted to set directory using FTP while not connected!");
		return 0; // cannot connect...
	}

	BOOL result = m_FtpConnection->SetCurrentDirectory(curDirName);
	return result;
}

int CFTPCom::FindFile(novac::CString& fileName)
{
	if(m_FtpConnection == NULL){
		ShowMessage("ERROR: Attempted to find file using FTP while not connected!");
		return 0; // cannot connect...
	}

	// use a file find object to enumerate files
	CFtpFileFind finder(m_FtpConnection);
	BOOL result = finder.FindFile(fileName);
	if(0 == result){
		DWORD retcode = GetLastError();
		//ShowMessage("Could not find remote file");
	}
	return result;
}

// @return 0 if fail...
BOOL CFTPCom::DeleteFolder(const novac::CString& folder)
{
	// Check
	if(m_FtpConnection == NULL){
		ShowMessage("ERROR: Attempted to delete folder using FTP while not connected");
		return 0; 
	}

	// Remove the directory
	BOOL result = m_FtpConnection->RemoveDirectory(folder);
	return result;
}

BOOL CFTPCom::EnterFolder(const novac::CString& folder)
{
	novac::CString strDir, strFolder, msg;

	// Check...
	if(m_FtpConnection == NULL){
		ShowMessage("ERROR: Attempted to enter folder using FTP while not connected");
		return FALSE;
	}

	// Set the current directory, return 0 if fail...
	if(0 == m_FtpConnection->SetCurrentDirectory(folder))
		return FALSE;

	// Get the current directory, return 0 if fail...
	CString c_strDir;
	if(0 == m_FtpConnection->GetCurrentDirectory(c_strDir))
		return FALSE;

	strDir = novac::CString(c_strDir);

	// The response we want to have...
	strFolder.Format("/%s", (const char*)folder);

	// Compare if the returned string is the same as what we want...
	if(Equals(strDir, strFolder))
	{
		msg.Format("Get into folder %s", (const char*)folder);
		ShowMessage(msg);
		return TRUE;
	}
	else
	{
		msg.Format("Can not get into folder %s", (const char*)folder);
		ShowMessage(msg);
		return FALSE;
	}
}

BOOL CFTPCom::GotoTopDirectory()
{
	novac::CString folder("//");	
	return EnterFolder(folder);
}

void CFTPCom::ReadResponse(CInternetFile* file)
{
	char readBuf[256];
	novac::CString str,restStr;
	unsigned int rd = 0;

	// Check input-parameter...
	if(file == NULL){
		return;
	}

	ULONGLONG a = file->GetLength();
	do
	{
		rd = file->Read(readBuf, 256);
		if (rd > 0)
		{
			str.Format("%s",readBuf);
			restStr.Append(str);
		}
	} while (rd > 0);
}

/** Retrieves the list of files in the current directory */
int CFTPCom::GetFileList(novac::CList <novac::CString, novac::CString &> &fileNames){
	novac::CString name;

	// start by clearing the list
	fileNames.RemoveAll();

	if(this->m_FtpConnection == NULL)
		return 1; // fail

	// use CFtpFileFind to find all files in the current directory
	CFtpFileFind finder(m_FtpConnection);
	BOOL bWorking = finder.FindFile(_T("*"));

	// start looping
	while (bWorking)
	{
		// get the next file-name
		bWorking = finder.FindNextFile();
		name.Format("%s", finder.GetFileName());
		
		// insert the file-name into the list
		fileNames.AddTail(name);
	}

	return 0;
}

/** Retrieves the list of files in the current directory */
int CFTPCom::GetFileList(const novac::CString &directory, novac::CList <CFileInfo, CFileInfo &> &fileInfos){
	CFileInfo info;

	// start by clearing the list
	fileInfos.RemoveAll();

	if(this->m_FtpConnection == NULL)
		return 1; // fail

	// use CFtpFileFind to find all files in the current directory
	CFtpFileFind finder(m_FtpConnection);
	BOOL bWorking = finder.FindFile(_T(directory + "*"));

	// start looping
	while (bWorking)
	{
		// get the next file-name
		bWorking = finder.FindNextFile();
		
		if(!finder.IsDots()){
			info.m_fileName.Format(finder.GetFileName());
			info.m_isDirectory	= (finder.IsDirectory() == TRUE) ? true : false;
			info.m_fileSize		= finder.GetLength();
			info.m_fullFileName.Format(finder.GetFilePath());
			
			// insert the file-name into the list
			fileInfos.AddTail(info);
		}
	}

	return 0;
}