#pragma once

/** The class <b>CStatusLogFileWriter</b> takes care
	of writing the status log file */
class CStatusLogFileWriter : public CWinThread
{
	DECLARE_DYNCREATE(CStatusLogFileWriter)

protected:
	CStatusLogFileWriter();           // protected constructor used by dynamic creation
	virtual ~CStatusLogFileWriter();

public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

	/** Called to write a string to the status log file */
	afx_msg void OnWriteMessage(WPARAM wParam, LPARAM lParam);

	/** */
	afx_msg void OnTimer(WPARAM nIDEvent, LPARAM lp);

	/** Called when the thread is to be stopped */
	afx_msg void OnQuit(WPARAM wp, LPARAM lp);

protected:
	DECLARE_MESSAGE_MAP()
	
private:
	/** This is the buffer of messages to be written to file. 
		The messages are written only occasionally to reduce the number of accesses to the disk */
	char *m_dataBuffer;
	
	/** The number of characters in the databuffer */
	unsigned long m_bufferSize;
	
	/** The maximum size of the data buffer */
	static const int BUFFERSIZE = 65536;

	/** the number of calls made to OnTimer */
	unsigned long m_nTimerCalls;

	/** Timer. We want to flush the buffer if nothing has happened for a long time... */
	UINT_PTR m_nTimerID;

	/** Flushes the buffer to disk.
		Writes the contents of the buffer, and the contents of the supplied
			string (if it is not NULL) */
	void FlushBuffer(const CString *string = NULL);

};


