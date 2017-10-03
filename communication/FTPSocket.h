#pragma once

#ifndef FTPSOCKET_H
#define FTPSOCKET_H

// #include <afxsock.h>
#include <vector>
// #include <afxtempl.h>
#define RESPONSE_LEN 12288

#include <PPPLib/CString.h>
#include <PPPLib/CList.h>

namespace Communication
{

	typedef std::vector<char> TByteVector;

	class CFTPSocket
	{
	public:

		CFTPSocket(void);

		~CFTPSocket(void);
		// ----------------------------------------------------------------------
		// ---------------------- PUBLIC METHODS --------------------------------
		// ----------------------------------------------------------------------

		/**Send command to FTP server
		  @param command - the standard commands that are listed in RFC 959
		  @param commandText - the parameters to be set in this command.
		   When there is no parameter to be used, this value is "".
		  @return the error code if this thread's last Windows Sockets operation failed.
		  @return 0 if successful.
		*/
		int SendCommand(novac::CString command, novac::CString commandText);

		/**Connect FTP server
		   @param usedSocket - the socket to be used to make connection
		   @param serverIP - server ip address
		   @param serverPort - the port on the server for connection
		   @return true if connected
		*/
		bool Connect(SOCKET& usedSocket, char* serverIP, int serverPort);

		/** Read the control response from the server, write message to m_serverMsg
			@return true if there is response
		*/
		int ReadResponse();

		/** Read data from the FTP server*/
		bool ReadData();

		/** Set the ftp log file name*/
		void SetLogFileName(const novac::CString fileName);

		/**Upload a file from the local path
		   @param fileLocalPath file full path including file name
		*/
		bool UploadFile(novac::CString fileLocalPath);

		/** delete a file in remote ftp server*/
		bool DeleteFTPFile(novac::CString fileName);

		/**Get file name from full file path
			@param filePath - file's full path
		*/
		void GetFileName(novac::CString& filePath);

		/**Get port number from FTP server's response
			@return port number if successful
			@return 0 if it fails
		*/
		int GetPortNumber();

		bool SendFileToServer(novac::CString& fileLocalPath);

		/**Login FTP server
		   @param ftpServerIP - ip address of ftp server
		   @param ftpPort - ftp server's port , for control channel, standard is 21.
		   @param userName - login name
		   @param pwd - login password
		   @return true if successful
		*/
		bool Login(const novac::CString ftpServerIP, novac::CString userName, novac::CString pwd, int ftpPort = 21);

		/** Get system type*/
		void GetSysType(novac::CString& type);

		/** Enter one folder*/
		bool EnterFolder(novac::CString& folder);

		/** Go to upper folder*/
		bool GoToUpperFolder();

		/** Enter passive mode*/
		bool EnterPassiveMode();

		/** Send list command to the FTP server*/
		bool List();

		/** Send nlst command to the FTP server*/
		bool NameList();

		/** List the file names from the FTP server*/
		bool GetFileNameList();

		/** List the files and other file information*/
		bool GetFileList();

		/** Get the FTP server's features*/
		bool GetServerFeature();

		/**Get the remote file size in FTP server,NOT FOR NOVAC SCANNER FTP SERVER
		  @param fileName - the remote file's name
		  @return file size if successful,
		  @return -1 if fails
		*/
		long Size(novac::CString& fileName);

		/**Check whehter the remote file exists
		*@param remoteFileName - the remote file name
		*@param remoteDirectory - remote directory
		*@return true if exists
		*@return false if the file does not exist
		*/
		bool CheckFileExistence(novac::CString& remoteFileName, novac::CString& remoteDirectory);

		/**Get FTP error code which is at the beginning of the reply
		*@ftpMsg- the mesage that the FTP server responses.
		*@return 0 if fail to find a number at the beginning of the reply
		*@return a positive integer if successful
		*/
		int GetMsgCode(novac::CString& ftpMsg);

		/**judge the ftp error code meaning
		*@code - the ftp error code
		*return TRUE if successful
		*return FALSE if there is error in last communication
		*/
		int JudgeFTPCode(int code);

		/**Judge successful or not from the ftp reply message
		*return true if successful
		*/
		bool IsFTPCommandDone();

		int DownloadFile(novac::CString remoteFileName, novac::CString localFileName);

		bool OpenFileHandle(novac::CString& fileName);

		/**Makes the given directory to be the current directory in the FTP server
		*@directory the directory to be change to in the FTP server
		*/
		bool SetCurrentFTPDirectory(novac::CString& directory);

		/**Get the current directory*/
		bool GetCurrentFTPDirectory(novac::CString& curDirectory);

		/**Get a string which is in one pair of seperator
		*@param line a string which includes the wanted part
		*@param seperator one pair of seperators which keep the wanted part in the middle
		*for example, 220 xx "\" is the current. "\" is what we want
		*/
		void GetCitedString(novac::CString& line, novac::CString& leftSeperator, novac::CString& rightSeperator);

		/**Create a directory in remote FTP server
		*@param parenetDirectory parent directory name
		*@param newDirectory new directory name
		*/
		bool CreateFTPDirectory(novac::CString& parentDirectory, novac::CString& newDirectory);

		/**close socket*/
		int Disconnect();

		int CloseASocket(SOCKET sock);

		/** find a file in ftp server
			@return true if it finds the file
		*/
		bool FindFile(novac::CString fileName);

		/**store receive data from data socket in m_vDataBuffer*/
		virtual void StoreReceivedBytes(const TByteVector& vBuffer, long receivedBytes);

		/**write the data from the vector into a file*/
		void WriteVectorFile(novac::CString fileName, const TByteVector& vBuffer, long receivedBytes);

		/**check whether there  is data to be read*/
		bool IsDataReady(const SOCKET& socket, long timeout);

		// ----------------------------------------------------------------------
		// ---------------------- PUBLIC DATA -----------------------------------
		// ----------------------------------------------------------------------
	public:
		/**special socket for control connection */
		SOCKET m_controlSocket;

		/**the server's reply */
		novac::CString m_serverMsg;

		novac::CString m_msg;

		/**data structure to store ftp server parameters*/
		typedef struct
		{
			char m_serverIP[64];
			int m_serverPort;
			int m_serverDataPort;
			novac::CString userName;
			novac::CString password;
		} ftpParameter;

		ftpParameter m_serverParam;

		/**file handle to the file to be downloaded*/
		HANDLE m_hDownloadedFile;

		/** file to store the file list of the FTP server */
		novac::CString m_listFileName;

		// ----------------------------------------------------------------------
		// ---------------------- PRIVATE DATA -----------------------------------
		// ----------------------------------------------------------------------
	private:
		/**socket for data connection - to send or download data*/
		SOCKET m_dataSocket;

		/**longest duration for one connection*/
		long m_timeout;

		/**buffer to store received control info from the FTP server*/
		char m_receiveBuf[RESPONSE_LEN];

		/**vector to store received data from the FTP server*/
		TByteVector m_vDataBuffer;

		/**list to store the ftp codes which indicate the status of last communication */
		novac::CList<int, int> m_ftpCode;
	};
}

#endif