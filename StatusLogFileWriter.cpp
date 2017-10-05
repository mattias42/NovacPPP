// StatusLogFileWriter.cpp : implementation file
//

#include "stdafx.h"
#include "StatusLogFileWriter.h"

// This is the settings for how to do the procesing
#include "Configuration/UserConfiguration.h"

extern Configuration::CUserConfiguration			g_userSettings;// <-- The settings of the user


// CStatusLogFileWriter

IMPLEMENT_DYNCREATE(CStatusLogFileWriter, CWinThread)

CStatusLogFileWriter::CStatusLogFileWriter()
{
	m_nTimerID = 0;
}

CStatusLogFileWriter::~CStatusLogFileWriter()
{
}

BOOL CStatusLogFileWriter::InitInstance()
{
	m_dataBuffer = new char[BUFFERSIZE];
	m_bufferSize = 0;

	// Set a timer to wake up every five seconds to write data to disk
	m_nTimerID = ::SetTimer(NULL, 0, 5 * 1000, NULL);

	m_nTimerCalls = 0;
	
	// call the timer once to set things up...
	OnTimer(NULL, NULL);

	return TRUE;
}

int CStatusLogFileWriter::ExitInstance()
{
	// kill the timer
	if(0 != m_nTimerID){
		KillTimer(NULL, m_nTimerID);
	}

	if(m_dataBuffer != NULL){
		// flush the buffer to file
		this->FlushBuffer();
		
		// clean up
		delete [] m_dataBuffer;
		m_dataBuffer = NULL;
		m_bufferSize = 0;
	}

	return CWinThread::ExitInstance();
}

BEGIN_MESSAGE_MAP(CStatusLogFileWriter, CWinThread)
	ON_THREAD_MESSAGE(WM_SHOW_MESSAGE,		OnWriteMessage)
	ON_THREAD_MESSAGE(WM_TIMER,				OnTimer)
	ON_THREAD_MESSAGE(WM_QUIT,				OnQuit)
END_MESSAGE_MAP()


// CStatusLogFileWriter message handlers

/** Called to write a string to the status log file */
void CStatusLogFileWriter::OnWriteMessage(WPARAM wParam, LPARAM lParam){
	CString *msg = (CString *)wParam;

	// the length of the newly received string
	long newStringLength = msg->GetLength();

	// If we still have space in the databuffer then just print the 
	//	contents of the string to the buffer and return
	if(m_bufferSize + newStringLength + 2 < BUFFERSIZE){
		sprintf(m_dataBuffer + m_bufferSize, "%s\n", *msg);
		m_bufferSize += newStringLength + 1;
		delete msg;
		return;
	}else{
		// If we got this far then we don't have more space in our buffer
		//	write all the information we have to disk.
		this->FlushBuffer(msg);
		delete msg;
		return;
	}
}


void CStatusLogFileWriter::OnTimer(WPARAM nIDEvent, LPARAM lp){
	CString logFile;

	if(m_nTimerCalls == 0){
		// If the log-file already exists, then move it..
		logFile.Format("%s\\StatusLog.txt",g_userSettings.m_outputDirectory);
		if(IsExistingFile(logFile)){
			Common::ArchiveFile(logFile);
		}
	}
	++m_nTimerCalls;

	if(this->m_bufferSize > 0){
		// Flush the buffer to file
		FlushBuffer();
	}
}

/** Flushes the buffer to disk */
void CStatusLogFileWriter::FlushBuffer(const CString *msg){
	CString logFile,dateStr,logPath;

	// the directory where to put the log-file
	Common::GetDateText(dateStr);
	logPath.Format("%s\\", g_userSettings.m_outputDirectory);

	// make sure that the directory that should contain the log-file exists
	CreateDirectory(logPath,NULL);
	logFile.Format("%s\\StatusLog.txt",logPath);
	
	// Open the file and write the data we have
	FILE *f = fopen(logFile, "a+");
	if(f != NULL){
		// first write the contents of the buffer
		fprintf(f, "%s", m_dataBuffer);
		
		// the the contents of the new string
		if(msg != NULL){
			fprintf(f, "%s\n", *msg);
		}
	
		// always remember to close the file
		fclose(f);
		
		// clear the buffer
		m_bufferSize = 0;
	}
}


/** Quits the thread */
void CStatusLogFileWriter::OnQuit(WPARAM wp, LPARAM lp)
{
	// kill the timer
	if(0 != m_nTimerID){
		KillTimer(NULL, m_nTimerID);
	}

	if(m_dataBuffer != NULL){
		// flush the buffer to file
		this->FlushBuffer();
		
		// clean up
		delete [] m_dataBuffer;
		m_dataBuffer = NULL;
		m_bufferSize = 0;
	}
}