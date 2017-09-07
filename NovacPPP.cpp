// NovacPPP.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "NovacPPP.h"
#include "NovacPPPDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CNovacPPPApp

BEGIN_MESSAGE_MAP(CNovacPPPApp, CWinApp)
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()


// CNovacPPPApp construction

CNovacPPPApp::CNovacPPPApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}


// The one and only CNovacPPPApp object

CNovacPPPApp theApp;


// CNovacPPPApp initialization

BOOL CNovacPPPApp::InitInstance()
{
	// InitCommonControls() is required on Windows XP if an application
	// manifest specifies use of ComCtl32.dll version 6 or later to enable
	// visual styles.  Otherwise, any window creation will fail.
	InitCommonControls();

	CWinApp::InitInstance();

	AfxEnableControlContainer();

	CNovacPPPDlg dlg;
	m_pMainWnd = &dlg;
	
	// Set the command line arguments
	dlg.m_commandLine.Format(this->m_lpCmdLine);
	
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with OK
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with Cancel
	}

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}
