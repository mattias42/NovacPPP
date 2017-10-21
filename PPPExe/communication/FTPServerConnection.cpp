#include "FTPServerConnection.h"

#include "../Common/Common.h"
#include "../Common/EvaluationLogFileHandler.h"

// This is the settings for how to do the procesing
#include "../Configuration/UserConfiguration.h"

// This is the global list of volcanoes
#include <PPPLib/VolcanoInfo.h>

#include <PPPLib/CCriticalSection.h>
#include <PPPLib/CSingleLock.h>
#include <PPPLib/CFtpUtils.h>
#include <Poco/Net/FTPClientSession.h>
#include <Poco/Net/NetException.h>

#include <fstream>
#include <thread>
#include <chrono>
#include <mutex>

extern Configuration::CUserConfiguration			g_userSettings;// <-- The settings of the user
extern novac::CVolcanoInfo g_volcanoes;   // <-- A list of all known volcanoes

using namespace Communication;

void DownloadDataFromDir(std::string directory);

CFTPServerConnection::CFTPServerConnection(void)
{
}

CFTPServerConnection::~CFTPServerConnection(void)
{
}

novac::CString s_username;
novac::CString s_password;
std::string s_server;
novac::CList <novac::CString, novac::CString &> s_pakFileList;
novac::CCriticalSection s_pakFileListCritSect; // synchronization access to the list of pak-files
volatile double nMbytesDownloaded = 0.0;
double nSecondsPassed = 0.0;

struct GuardedValue
{
public:
	GuardedValue()
		: value(0) { }

	int GetValue() const { return value; }

	void Zero()
	{
		std::lock_guard<std::mutex> lock(guard);
		value = 0;
	}

	void IncrementValue()
	{
		std::lock_guard<std::mutex> lock(guard);
		++value;
	}

	void DecrementValue()
	{
		std::lock_guard<std::mutex> lock(guard);
		--value;
	}

private:
	int value;
	std::mutex guard;
};

static GuardedValue nFTPThreadsRunning;


/** Downloads .pak - files from the given FTP-server

	@return 0 on successful connection and completion of the list
		otherwise non-zero
*/
int CFTPServerConnection::DownloadDataFromFTP(const novac::CString &serverDir, const novac::CString &username,
	const novac::CString &password, novac::CList <novac::CString, novac::CString &> &pakFileList) {

	novac::CString userMessage;
	unsigned int nRounds = 0;

	s_username.Format(username);
	s_password.Format(password);

	nFTPThreadsRunning.Zero();

	// Extract the name of the server and each of the sub-directories specified
	std::string directory;
	novac::CFtpUtils ftpUtil{ g_volcanoes, g_userSettings.m_volcano };
	ftpUtil.SplitPathIntoServerAndDirectory(serverDir, s_server, directory);


	// Make sure thath the temporary directory exists
	if (CreateDirectoryStructure(g_userSettings.m_tempDirectory)) {
		userMessage.Format("Could not create temp directory: %s", (const char*)g_userSettings.m_tempDirectory);
		ShowMessage(userMessage);
		return 1;
	}

	// download the data in this directory
	// TODO: ImplementMe
	//CWinThread *downloadThread = AfxBeginThread(DownloadDataFromDir, new novac::CString(directory), THREAD_PRIORITY_BELOW_NORMAL, 0, 0, nullptr);
	nFTPThreadsRunning.IncrementValue();
	std::thread downloadThread{ DownloadDataFromDir, directory };

	// wait for all threads to terminate
	std::this_thread::sleep_for(std::chrono::milliseconds{ 500 });

	clock_t cStart = clock();
	while (nFTPThreadsRunning.GetValue() > 0) {
		if (++nRounds % 10 == 0) {
			nSecondsPassed = (double)(clock() - cStart) / CLOCKS_PER_SEC;

			if (nSecondsPassed < 100.0 * nMbytesDownloaded) {
				userMessage.Format("  %.0lf MBytes downloaded (<=> %.2lf MBytes/second)", nMbytesDownloaded, nMbytesDownloaded / nSecondsPassed);
			}
			else {
				userMessage.Format("  %.0lf MBytes downloaded (<=> %.2lf kBytes/second)", nMbytesDownloaded, 1024 * nMbytesDownloaded / nSecondsPassed);
			}
			ShowMessage(userMessage);
		}

		// Sleep(100);
	}

	// copy the data to the output list
	auto p = s_pakFileList.GetHeadPosition();
	while (p != nullptr) {
		pakFileList.AddTail(novac::CString(s_pakFileList.GetNext(p)));
	}

	return 0;
}

// this function takes care of adding filenames to the list
//	in a synchronized way so that no two threads access
//	the list at the same time...
void AddFileToList(const novac::CString &fileName) {
	novac::CSingleLock singleLock(&s_pakFileListCritSect);
	singleLock.Lock();
	if (singleLock.IsLocked()) {
		s_pakFileList.AddTail(novac::CString(fileName));
	}
	singleLock.Unlock();
}

/** Downloads all the data-files from the directory that the
	connection 'm_ftp' is in at the moment */
void DownloadDataFromDir(std::string directory) {

	std::vector<novac::CFileInfo> filesFound;
	novac::CString localFileName, serial;
	novac::CString userMessage;
	CDateTime start;
	int channel;
	MEASUREMENT_MODE mode;

	novac::CFtpUtils ftpHelper;

	userMessage.Format("Started thread #%d", nFTPThreadsRunning.GetValue());
	ShowMessage(userMessage);

	// create a new connection
	Poco::Net::FTPClientSession ftp;

	// connect to the server
	try
	{
		ftp.open(s_server, 21, s_username.std_str(), s_password.std_str());
	}
	catch (Poco::Net::FTPException& ex)
	{
		ShowMessage("Failed to connect: " + ex.message());
		return; // failed to connect!
	}

	// Search for files in the specified directory
	std::istream& fileListStream = ftp.beginList(directory, true);
	// std::string fileName;
	// while (fileListStream >> fileName)
	for (std::string line; std::getline(fileListStream, line); )
	{
		novac::CFileInfo result;
		if (ftpHelper.ReadFtpDirectoryListing(line, result))
		{
			filesFound.push_back(result);
		}
	}
	ftp.endList();

	//if (ftp->GetFileList(directory, filesFound)) {
	//	ftp->Disconnect();

	//	nFTPThreadsRunning.DecrementValue();

	//	return;
	//}

	//// download each of the files .pak-files found
	////	or enter the all the sub-directories
	//auto p = filesFound.GetHeadPosition();
	//while (p != nullptr) {
	//	CFileInfo &fileInfo = filesFound.GetNext(p);

	//	if (Equals(fileInfo.m_fileName.Right(4), ".pak")) {
	//		// if this is a .pak-file then check the date when it was created
	//		if (FileHandler::CEvaluationLogFileHandler::GetInfoFromFileName(fileInfo.m_fileName, start, serial, channel, mode)) {
	//			if (start <= g_userSettings.m_toDate && g_userSettings.m_fromDate <= start) {
	//				// the creation date is between the start and the stop dates. Download the file
	//				localFileName.Format("%s\\%s", (const char*)g_userSettings.m_tempDirectory, (const char*)fileInfo.m_fileName);
	//				if (IsExistingFile(localFileName)) {
	//					userMessage.Format("File %s is already downloaded", (const char*)localFileName);
	//					ShowMessage(userMessage);

	//					AddFileToList(localFileName);
	//				}
	//				else {

	//					// download the file
	//					if (ftp->DownloadAFile(fileInfo.m_fullFileName, localFileName)) {
	//						nMbytesDownloaded += fileInfo.m_fileSize / 1048576.0;

	//						AddFileToList(localFileName);
	//					}
	//				}
	//			}
	//		}
	//	}
	//	else if (g_userSettings.m_includeSubDirectories_FTP && fileInfo.m_isDirectory) {
	//		int year, month, day;
	//		CDateTime date;
	//		int nNumbers = sscanf(fileInfo.m_fileName, "%d.%d.%d", &year, &month, &day);
	//		if (nNumbers == 3) {
	//			date = CDateTime(year, month, day, 0, 0, 0);
	//			if (date < g_userSettings.m_fromDate)
	//				continue;
	//		}
	//		if (nFTPThreadsRunning.GetValue() >= (int)g_userSettings.m_maxThreadNum) {
	//			// start downloading using the same thread as we're running in
	//			novac::CString subDir = fileInfo.m_fullFileName + "/";
	//			DownloadDataFromDir(subDir.std_str());
	//		}
	//		else {
	//			// start downloading using a new thread
	//			// TODO: ImplementMe
	//			// CWinThread *downloadThread = AfxBeginThread(DownloadDataFromDir, new novac::CString(fileInfo.m_fullFileName + "/"), THREAD_PRIORITY_BELOW_NORMAL, 0, 0, nullptr);
	//			//Common::SetThreadName(downloadThread->m_nThreadID, "DownloadDataFromDir");
	//		}
	//	}
	//}

	//// remember to close the connection before returning
	//ftp->Disconnect();


	nFTPThreadsRunning.DecrementValue();

	return;
}

/** Retrieves the list of files in a given directory on the FTP-server
	@return 0 on successful connection and completion of the download
*/
int CFTPServerConnection::DownloadFileListFromFTP(const novac::CString &serverDir, novac::CList <novac::CString, novac::CString&> &fileList, const novac::CString &username, const novac::CString &password) {
	// Extract the name of the server and each of the sub-directories specified
	std::string server, directory;
	novac::CFtpUtils ftpUtil{ g_volcanoes, g_userSettings.m_volcano };
	ftpUtil.SplitPathIntoServerAndDirectory(serverDir, server, directory);

	// create a new connection
	Poco::Net::FTPClientSession ftp;

	// connect to the server
	try
	{
		ftp.open(server, 21, username.std_str(), password.std_str());
	}
	catch (Poco::Net::FTPException& ex)
	{
		ShowMessage("Failed to connect: " + ex.message());
		return 1; // failed to connect!
	}

	// Search for files in the specified directory
	std::istream& fileListStream = ftp.beginList(serverDir.std_str());
	std::string fileName;
	while (fileListStream >> fileName)
	{
		fileList.AddTail(fileName);
	}
	ftp.endList();

	// Extract the file-names...
	//auto p = filesFound.GetHeadPosition();
	//while (p != nullptr) {
	//	CFileInfo &fileInfo = filesFound.GetNext(p);

	//	if (!fileInfo.m_isDirectory) {
	//		fileList.AddTail(fileInfo.m_fileName);
	//	}
	//}

	return 0;
}

/** Downloads a single file from the given FTP-server
	@return 0 on successful connection and completion of the download
*/
int CFTPServerConnection::DownloadFileFromFTP(const novac::CString &remoteFileName, const novac::CString &localFileName,
	const novac::CString &username, const novac::CString &password) {
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
	if (remoteFileName.Find("ftp://") != -1) {
		indexOfSlash[0] = 5;
	}
	else {
		indexOfSlash[0] = 0;
	}
	while (-1 != (indexOfSlash[nSlashesFound + 1] = remoteFileName.Find('/', indexOfSlash[nSlashesFound] + 1))) {
		++nSlashesFound;
	}
	subString.Format(remoteFileName.Left(indexOfSlash[1]));
	directory.Format(remoteFileName.Right(remoteFileName.GetLength() - indexOfSlash[1] - 1));
	s_server = subString.Right(subString.GetLength() - indexOfSlash[0] - 1).std_str();

	// create a new connection
	CFTPCom *ftp = new CFTPCom();

	// connect to the server
	if (ftp->Connect(s_server, s_username, s_password, TRUE) != 1) {
		ShowMessage("Failed to connect to FTP server");
		delete ftp;
		return 1; // failed to connect!
	}

	// Download the file
	if (0 == ftp->DownloadAFile(directory, localFileName)) {
		errorMessage.Format("Failed to download remote file %s from FTP server", (const char*)remoteFileName);
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
	const novac::CString &password, novac::CList <novac::CString, novac::CString &> &fileList) {
	CDateTime now;

	// Extract the name of the server and the login
	s_username.Format(username);
	s_password.Format(password);
	s_server = server.std_str();

	novac::CString volcanoName, directoryName, remoteFile, errorMessage;

	// create a new connection
	CFTPCom *ftp = new CFTPCom();

	// connect to the server
	if (ftp->Connect(s_server, s_username, s_password, TRUE) != 1) {
		ShowMessage("Failed to connect to FTP server");
		delete ftp;
		return 1; // failed to connect!
	}

	// Enter the volcano's directory
	volcanoName.Format("%s", (const char*)g_volcanoes.GetSimpleVolcanoName(g_userSettings.m_volcano));
	ftp->EnterFolder(volcanoName);

	// Enter the upload-directory
	now.SetToNow();
	directoryName.Format("PostProcessed_BETA_%04d%02d%02d", now.year, now.month, now.day);
	if (0 == ftp->CreateDirectory(directoryName)) {
		ShowMessage("Failed to create directory on FTP server");
		ftp->Disconnect();
		delete ftp;
		return 2;
	}
	ftp->EnterFolder(directoryName);

	// Upload the files
	auto p = fileList.GetHeadPosition();
	while (p != nullptr) {
		// Get the local name and path of the file to upload
		novac::CString &localFile = fileList.GetNext(p);

		// Get the file-name to upload the file to...
		remoteFile.Format(localFile);
		Common::GetFileName(remoteFile);

		if (0 == ftp->UploadFile(localFile, remoteFile)) {
			errorMessage.Format("Failed to upload local file %s to FTP server", (const char*)localFile);
			ShowMessage(errorMessage);
		}
	}

	// disconnect
	ftp->Disconnect();
	delete ftp;
	return 0;
}
