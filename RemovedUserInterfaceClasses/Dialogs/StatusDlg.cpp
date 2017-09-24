// StatusDlg.cpp : implementation file
//

#include "stdafx.h"
#include "../NovacPPP.h"
#include "StatusDlg.h"

// This is the settings for how to do the procesing
#include "../Configuration/UserConfiguration.h"

#include "../StatusLogFileWriter.h"

extern Configuration::CUserConfiguration			g_userSettings;// <-- The settings of the user
CWinThread*											g_statusLogFileWriter;

using namespace Dialogs;

IMPLEMENT_DYNAMIC(CStatusDlg, CDialog)
CStatusDlg::CStatusDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CStatusDlg::IDD, pParent)
{
	g_statusLogFileWriter = AfxBeginThread(RUNTIME_CLASS(CStatusLogFileWriter), THREAD_PRIORITY_HIGHEST, 0, 0, NULL);
	Common::SetThreadName(g_statusLogFileWriter->m_nThreadID, "StatusLogFileWriter");
}

CStatusDlg::~CStatusDlg()
{
}

void CStatusDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	
	DDX_Control(pDX, IDC_STATUS_MESSAGE_LIST, m_statusListBox);
}


BEGIN_MESSAGE_MAP(CStatusDlg, CDialog)
	ON_MESSAGE(WM_SHOW_MESSAGE,					OnShowMessage)
	ON_WM_SIZE()
	ON_WM_CLOSE()
END_MESSAGE_MAP()


// CStatusDlg message handlers

LRESULT CStatusDlg::OnShowMessage(WPARAM wParam, LPARAM lParam){
	CString *msg = (CString *)wParam;
	long maxItemNum = 100;
	CString windowTitle;
	bool exiting = false;

	if(msg == NULL)
		return 0;

	// search for the special message saying how far we've come with the evaluations...
	if((-1 != msg->Find("Evaluated ")) && (-1 != msg->Find(" scans out of "))){
		int pt1 = msg->Find("(");
		int pt2 = msg->Find(")");
		if(pt2 != -1 && pt1 != -1){
			CString a, b;
			a.Format("%s", msg->Right(msg->GetLength() - pt1));
			b.Format("%s", a.Left(pt2 + 1 - pt1));
		
			windowTitle.Format("Evaluating: %s ", b);
			SetWindowText(windowTitle);
		}
	}else if(-1 != msg->Find("MBytes downloaded")){
		int pt1 = msg->Find(" -- ");
		if(pt1 != -1){
			windowTitle.Format("Downloading: %s", msg->Left(pt1));
			SetWindowText(windowTitle);
		}
	}else if(-1 != msg->Find("Begin to calculate plume heights from scans")){
		windowTitle.Format("Calculating Geometries");
		SetWindowText(windowTitle);
	}else if(-1 != msg->Find("Calculating flux")){
		windowTitle.Format("Calculating Flux");
		SetWindowText(windowTitle);
	}else if(-1 != msg->Find("Exit post processing")){
		windowTitle.Format("Done !");
		SetWindowText(windowTitle);
		exiting = true;
	}
	

	// update the status message listbox
	int nItems = m_statusListBox.GetCount();
	if(nItems > maxItemNum){
		m_statusListBox.DeleteString(maxItemNum);
	}
	m_statusListBox.InsertString(0, *msg);

	if(strlen(*msg) > 15){
		// Find the longest string in the list box.
		CString str;
		CSize sz;
		int dx = 0;
		TEXTMETRIC tm;
		CDC* pDC = m_statusListBox.GetDC();
		CFont* pFont = m_statusListBox.GetFont();

		// Select the listbox font, save the old font
		CFont* pOldFont = pDC->SelectObject(pFont);
		// Get the text metrics for avg char width
		pDC->GetTextMetrics(&tm); 

		for (int i = 0; i < m_statusListBox.GetCount(); i++)
		{
			m_statusListBox.GetText(i, str);
			sz = pDC->GetTextExtent(str);

			// Add the avg width to prevent clipping
			sz.cx += tm.tmAveCharWidth;

			if (sz.cx > dx)
				dx = sz.cx;
		}
		// Select the old font back into the DC
		pDC->SelectObject(pOldFont);
		m_statusListBox.ReleaseDC(pDC);

		// Set the horizontal extent so every character of all strings can be scrolled to.
		m_statusListBox.SetHorizontalExtent(dx);
	}


	// add the message to the log file
	g_statusLogFileWriter->PostThreadMessage(WM_SHOW_MESSAGE, (WPARAM)msg, NULL);

	// if we are done with the processing, close down the writer...
	if(exiting && (g_statusLogFileWriter != NULL)){
		g_statusLogFileWriter->PostThreadMessage(WM_QUIT, NULL, NULL);
		::WaitForSingleObject(g_statusLogFileWriter, INFINITE);
	}
	

	return 0;
}
void Dialogs::CStatusDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);

	if(m_statusListBox.m_hWnd != NULL){
		// resize the message list box
		m_statusListBox.MoveWindow(10, 10, cx - 20, cy - 20);
	}
}

void Dialogs::CStatusDlg::OnClose()
{
	// Close the status-log file writer
	if(g_statusLogFileWriter != NULL){
		g_statusLogFileWriter->PostThreadMessage(WM_QUIT, NULL, NULL);
		::WaitForSingleObject(g_statusLogFileWriter, INFINITE);
	}

	CDialog::OnClose();
}
