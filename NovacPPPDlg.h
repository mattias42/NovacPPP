// NovacPPPDlg.h : header file
//

#pragma once
#include "afxwin.h"

#include "Dialogs/StatusDlg.h"

/** The class <b>CNovacPPPDlg</b> implements the main interface of the NovacPPP
	This takes care of the (few) dialog items and the button to start the program.
*/
class CNovacPPPDlg : public CDialog
{
// Construction
public:
	CNovacPPPDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_NOVACPPP_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	
	/** This parses the string sent to the program through the command-line option and sets
		the appropriate variables in 'g_setup' and 'g_userOptions' accordingly.
		This function should be called <b>after</b> the processing.xml etc have been read
		in for this option to take higher priority over the configuration-files */
	void ParseCommandLineOptions();
	
	DECLARE_MESSAGE_MAP()
public:
	CComboBox m_comboAllVolcanoes;
	int m_FromYear;
	int m_FromMonth;
	int m_FromDay;
	int m_ToYear;
	int m_ToMonth;
	int m_ToDay;
	
	/** This is the arguments passed in the command line to the software... */
	CString m_commandLine;
	
	afx_msg void OnBnClickedCalculateFluxes();
	
	Dialogs::CStatusDlg m_statusDlg;
	
	afx_msg void OnMenuViewConfigurationForSelectedVolcano();
	afx_msg void OnUpdateViewConfigurationForSelectedVolcano(CCmdUI *pCmdUI);
	afx_msg void OnChangeSelectedVolcano();
	afx_msg void OnBnClickedCheckLocal();
	afx_msg void OnBnClickedCheckFtp();
	
	/** Enables or disables the controls depending on the settings */
	void EnableControls();
	
	/** Updates teh values in the interface with the ones found in 
		the configuration or from the command-line */
	void UpdateInterface();
private:
	// Dialog components
	CEdit m_editLocalDir, m_editFTPDir, m_editFTPpwd, m_editFTPuser;
	CButton m_CheckLocalDirectory;
	CButton m_CheckFtp;
	CButton m_CheckDatabase;
	CButton m_CheckIncludeSubDirsLocal;
	CButton m_CheckIncludeSubDirsFTP;
	
public:
	afx_msg void OnClose();
};
