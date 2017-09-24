#include "StdAfx.h"
#include "FTPServerConnection.h"

// Include synchronization classes
// #include <afxmt.h>

#include "../Common/Common.h"
#include "../Common/EvaluationLogFileHandler.h"

// This is the settings for how to do the procesing
#include "../Configuration/UserConfiguration.h"

// This is the global list of volcanoes
#include "../VolcanoInfo.h"

#include <PPPLib/CCriticalSection.h>
#include <PPPLib/CSingleLock.h>

extern Configuration::CUserConfiguration			g_userSettings;// <-- The settings of the user
extern CVolcanoInfo									g_volcanoes;   // <-- A list of all known volcanoes

using namespace Communication;

UINT DownloadDataFromDir(LPVOID pParam);

CFTPServerConnection::CFTPServerConnection(void)
{
}

CFTPServerConnection::~CFTPServerConnection(void)
{
}

novac::CString s_username;
novac::CString s_password;
novac::CString s_server;
novac::CList <novac::CString, novac::CString &> s_pakFileList;
novac::CCriticalSection s_pakFileListCritSect; // synchronization access to the list of pak-files
novac::CCriticalSection s_ThreadNumCritSect; // synchronization access to the number of concurrently running threads
volatile int nFTPThreadsRunning = 0;
volatile double nMbytesDownloaded = 0.0;
double nSecondsPassed = 0.0;

/** Downloads .pak - files from the given FTP-server 

	@return 0 on successful connection and completion of the list
		otherwise non-zero
*/
int CFTPServerConnection::DownloadDataFromFTP(const novac::CString &serverDir, const novac::CString &username,
	const novac::CString &password, novac::CList <novac::CString, novac::CString &> &pakFileList){
	
	novac::CString userMessage;
	unsigned int nRounds = 0;

	s_username.Format(username);
	s_password.Format(password);

	nFTPThreadsRunning = 0;

	// Extract the name of the server and each of the sub-directories specified
	novac::CString subString;
	novac::CString directory;
	int indexOfSlash[128];
	int nSlashesFound = 0; // the number of slashes found in the 'serverDir' - path
	if(serverDir.Find("ftp://") != -1){
		indexOfSlash[0] = 5;
	}else{
		indexOfSlash[0] = 0;
	}
	while(-1 != (indexOfSlash[nSlashesFound + 1] = serverDir.Find('/', indexOfSlash[nSlashesFound] + 1))){
		++nSlashesFound;
	}
	subString.Format(serverDir.Left(indexOfSlash[1]));
	directory.Format(serverDir.Right(serverDir.GetLength() - indexOfSlash[1] - 1));
	s_server.Format(subString.Right(subString.GetLength() - indexOfSlash[0] - 1));
	if(nSlashesFound == 1){
		directory.Format("%s/", g_volcanoes.GetSimpleVolcanoName(g_userSettings.m_volcano));
	}
	
	// Make sure thath the temporary directory exists
	if(CreateDirectoryStructure(g_userSettings.m_tempDirectory)){
		userMessage.Format("Could not create temp directory: %s", g_userSettings.m_tempDirectory);
		ShowMessage(userMessage);
		return 1;
	}	
	
	// download the data in this directory
	CWinThread *downloadThread = AfxBeginThread(DownloadDataFromDir, new novac::CString(directory), THREAD_PRIORITY_BELOW_NORMAL, 0, 0, NULL);
	Common::SetThreadName(downloadThread->m_nThreadID, "DownloadDataFromDir");

	// wait for all threads to terminate
	Sleep(500);
	clock_t cStart = clock();
	while(nFTPThreadsRunning > 0){
		if(++nRounds % 10 == 0){
			nSecondsPassed = (double)(clock() - cStart) / CLOCKS_PER_SEC;

			if(nSecondsPassed < 100.0 * nMbytesDownloaded){
				userMessage.Format("  %.0lf MBytes downloaded (<=> %.2lf MBytes/second)", nMbytesDownloaded, nMbytesDownloaded / nSecondsPassed);
			}else{
				userMessage.Format("  %.0lf MBytes downloaded (<=> %.2lf kBytes/second)", nMbytesDownloaded, 1024 * nMbytesDownloaded / nSecondsPassed);
			}
			ShowMessage(userMessage);
		}

		Sleep(100);
	}

	// copy the data to the output list
	POSITION p = s_pakFileList.GetHeadPosition();
	while(p != NULL){
		pakFileList.AddTail(novac::CString(s_pakFileList.GetNext(p)));
	}

	return 0;
}

// this function takes care of adding filenames to the list
//	in a synchronized way so that no two threads access
//	the list at the same time...
void AddFileToList(const novac::CString &fileName){
	novac::CSingleLock singleLock(&s_pakFileListCritSect);
	singleLock.Lock();
	if(singleLock.IsLocked()){
		s_pakFileList.AddTail(novac::CString(fileName));
	}
	singleLock.Unlock();
}

/** Downloads all the data-files from the directory that the 
	connection 'm_ftp' is in at the moment 
	@return 0 on success, otherwise non-zero */
UINT DownloadDataFromDir(LPVOID pParam){
	// the directory to search in
	novac::CString *directory = (novac::CString *)pParam;

	novac::CList <CFileInfo, CFileInfo &> filesFound;
	novac::CString localFileName, serial;
	novac::CString userMessage;
	CDateTime start;
	int channel;
	MEASUREMENT_MODE mode;

	// if there are too many threads running, then wait for a while
	while(nFTPThreadsRunning >= (int)g_userSettings.m_maxThreadNum){
		Sleep(500);
	}
	novac::CSingleLock singleLockThread(&s_ThreadNumCritSect);
	singleLockThread.Lock();
	if(singleLockThread.IsLocked()){
		++nFTPThreadsRunning;
	}
	singleLockThread.Unlock();

	userMessage.Format("Started thread #%d", nFTPThreadsRunning);
	ShowMessage(userMessage);

	// create a new connection
	CFTPCom *ftp = new CFTPCom();

	// connect to the server
	if(ftp->Connect(s_server, s_username, s_password, TRUE) != 1){

		novac::CSingleLock singleLockThread(&s_ThreadNumCritSect);
		singleLockThread.Lock();
		if(singleLockThread.IsLocked()){
			--nFTPThreadsRunning;
		}
		singleLockThread.Unlock();

		delete ftp;
		delete directory;
		return 1; // failed to connect!
	}

	// Search for files in the specified directory
	if(ftp->GetFileList(*directory, filesFound)){
		ftp->Disconnect();

		novac::CSingleLock singleLockThread(&s_ThreadNumCritSect);
		singleLockThread.Lock();
		if(singleLockThread.IsLocked()){
			--nFTPThreadsRunning;
		}
		singleLockThread.Unlock();

		delete directory;
		return 1;
	}
	
	
	// download each of the files .pak-files found
	//	or enter the all the sub-directories
	POSITION p = filesFound.GetHeadPosition();
	while(p != NULL){
		CFileInfo &fileInfo = filesFound.GetNext(p);

		if(Equals(fileInfo.m_fileName.Right(4), ".pak")){
			// if this is a .pak-file then check the date when it was created
			if(FileHandler::CEvaluationLogFileHandler::GetInfoFromFileName(fileInfo.m_fileName, start, serial, channel, mode)){
				if(start <= g_userSettings.m_toDate && g_userSettings.m_fromDate <= start){
					// the creation date is between the start and the stop dates. Download the file
					localFileName.Format("%s\\%s", g_userSettings.m_tempDirectory, fileInfo.m_fileName);
					if(IsExistingFile(localFileName)){
						userMessage.Format("File %s is already downloaded", localFileName);
						ShowMessage(userMessage);
						
						AddFileToList(localFileName);
					}else{
						
						// download the file
						if(ftp->DownloadAFile(fileInfo.m_fullFileName, localFileName)){
							nMbytesDownloaded	+= fileInfo.m_fileSize / 1048576.0;

							AddFileToList(localFileName);
						}
					}
				}
			}
		}else if(g_userSettings.m_includeSubDirectories_FTP && fileInfo.m_isDirectory){
			int year, month, day;
			CDateTime date;
			int nNumbers = sscanf(fileInfo.m_fileName, "%d.%d.%d", &year, &month, &day);
			if(nNumbers == 3){
				date = CDateTime(year, month, day, 0, 0, 0);
				if(date < g_userSettings.m_fromDate)
					continue;
			}
			if(nFTPThreadsRunning >= (int)g_userSettings.m_maxThreadNum){
				// start downloading using the same thread as we're running in
				DownloadDataFromDir(new novac::CString(fileInfo.m_fullFileName + "/"));
			}else{
				// start downloading using a new thread
				CWinThread *downloadThread = AfxBeginThread(DownloadDataFromDir, new novac::CString(fileInfo.m_fullFileName + "/"), THREAD_PRIORITY_BELOW_NORMAL, 0, 0, NULL);
				Common::SetThreadName(downloadThread->m_nThreadID, "DownloadDataFromDir");
			}
		}
	}

	// remember to close the connection before returning
	ftp->Disconnect();
	
	// clean up	
	delete directory;

	novac::CSingleLock singleLockThread2(&s_ThreadNumCritSect);
	singleLockThread2.Lock();
	if(singleLockThread2.IsLocked()){
		--nFTPThreadsRunning;
	}
	singleLockThread2.Unlock();
	
	return 0;
}

/** Retrieves the list of files in a given directory on the FTP-server
	@return 0 on successful connection and completion of the download
*/
int CFTPServerConnection::DownloadFileListFromFTP(const novac::CString &serverDir, novac::CList <novac::CString, novac::CString&> &fileList, const novac::CString &username, const novac::CString &password){

	novac::CList <CFileInfo, CFileInfo &> filesFound;

	// Extract the name of the server and each of the sub-directories specified
	novac::CString subString, directory, server;
	int indexOfSlash[128];
	int nSlashesFound = 0; // the number of slashes found in the 'serverDir' - path
	if(serverDir.Find("ftp://") != -1){
		indexOfSlash[0] = 5;
	}else{
		indexOfSlash[0] = 0;
	}
	while(-1 != (indexOfSlash[nSlashesFound + 1] = serverDir.Find('/', indexOfSlash[nSlashesFound] + 1))){
		++nSlashesFound;
	}
	subString.Format(serverDir.Left(indexOfSlash[1]));
	directory.Format(serverDir.Right(serverDir.GetLength() - indexOfSlash[1] - 1));
	server.Format(subString.Right(subString.GetLength() - indexOfSlash[0] - 1));
	if(nSlashesFound == 1){
		directory.Format("%s/", g_volcanoes.GetSimpleVolcanoName(g_userSettings.m_volcano));
	}
	
	// create a new connection
	CFTPCom *ftp = new CFTPCom();

	// connect to the server
	if(ftp->Connect(server, username, password, TRUE) != 1){
		delete ftp;
		return 1; // failed to connect!
	}

	// Search for files in the specified directory
	if(ftp->GetFileList(directory, filesFound)){
		ftp->Disconnect();
		delete ftp;
		return 1;
	}
	ftp->Disconnect();
	
	// Extract the file-names...
	POSITION p = filesFound.GetHeadPosition();
	while(p != NULL){
		CFileInfo &fileInfo = filesFound.GetNext(p);
		
		if(!fileInfo.m_isDirectory){
			fileList.AddTail(fileInfo.m_fileName);
		}
	}	
	
	delete ftp;
	return 0;
}

/** Downloads a single file from the given FTP-server 
	@return 0 on successful connection and completion of the download
*/
int CFTPServerConnection::DownloadFileFromFTP(const novac::CString &remoteFileName, const novac::CString &localFileName,
	const novac::CString &username, const novac::CString &password){
	CDateTime now;
	novac::CString errorMessage;

	// Extract the name of the server and the login
	s_username.Format(username);
	s_password.Format(password);

	// Extract the name of the server and each of the sub-directories specified
	novac::CString subString;
	novac::CString directory;
	int indexOfSlash[128];
	int nSlashesFound = 0; // the number of slashes found in the 'serverDir' - path
	if(remoteFileName.Find("ftp://") != -1){
		indexOfSlash[0] = 5;
	}else{
		indexOfSlash[0] = 0;
	}
	while(-1 != (indexOfSlash[nSlashesFound + 1] = remoteFileName.Find('/', indexOfSlash[nSlashesFound] + 1))){
		++nSlashesFound;
	}
	subString.Format(remoteFileName.Left(indexOfSlash[1]));
	directory.Format(remoteFileName.Right(remoteFileName.GetLength() - indexOfSlash[1] - 1));
	s_server.Format(subString.Right(subString.GetLength() - indexOfSlash[0] - 1));

	// create a new connection
	CFTPCom *ftp = new CFTPCom();

	// connect to the server
	if(ftp->Connect(s_server, s_username, s_password, TRUE) != 1){
		ShowMessage("Failed to connect to FTP server");
		delete ftp;
		return 1; // failed to connect!
	}

	// Download the file
	if(0 == ftp->DownloadAFile(directory, localFileName)){
		errorMessage.Format("Failed to download remote file %s from FTP server", remoteFileName);
		ShowMessage(errorMessage);
	}
	
	// disconnect
	ftp->Disconnect();
	delete ftp;
	return 0;
}

/** Uploads result-files to the given FTP-server 
	@return 0 on success otherwise non-zero
*/
int CFTPServerConnection::UploadResults(const novac::CString &server, const novac::CString &username, 
						const novac::CString &password, novac::CList <novac::CString, novac::CString &> &fileList){
	CDateTime now;

	// Extract the name of the server and the login
	s_username.Format(username);
	s_password.Format(password);
	s_server.Format(server);

	novac::CString volcanoName, directoryName, remoteFile, errorMessage;

	// create a new connection
	CFTPCom *ftp = new CFTPCom();

	// connect to the server
	if(ftp->Connect(s_server, s_username, s_password, TRUE) != 1){
		ShowMessage("Failed to connect to FTP server");
		delete ftp;
		return 1; // failed to connect!
	}

	// Enter the volcano's directory
	volcanoName.Format("%s", g_volcanoes.GetSimpleVolcanoName(g_userSettings.m_volcano));
	ftp->EnterFolder(volcanoName);

	// Enter the upload-directory
	now.SetToNow();
	directoryName.Format("PostProcessed_BETA_%04d%02d%02d", now.year, now.month, now.day);
	if(0 == ftp->CreateDirectory(directoryName)){
		ShowMessage("Failed to create directory on FTP server");
		ftp->Disconnect();
		delete ftp;
		return 2;
	}
	ftp->EnterFolder(directoryName);

	// Upload the files
	POSITION p = fileList.GetHeadPosition();
	while(p != NULL){
		// Get the local name and path of the file to upload
		novac::CString &localFile = fileList.GetNext(p);
		
		// Get the file-name to upload the file to...
		remoteFile.Format(localFile);
		Common::GetFileName(remoteFile);
	
		if(0 == ftp->UploadFile(localFile, remoteFile)){
			errorMessage.Format("Failed to upload local file %s to FTP server", localFile);
			ShowMessage(errorMessage);
		}
	}
	
	// disconnect
	ftp->Disconnect();
	delete ftp;
	return 0;
}
