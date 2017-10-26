#pragma once


// CStatusDlg dialog
namespace Dialogs{
	/** The class <b>CStatusDlg</b> is used to show the output messages
		from the post processing
	*/

	class CStatusDlg : public CDialog
	{
		DECLARE_DYNAMIC(CStatusDlg)

	public:
		CStatusDlg(CWnd* pParent = NULL);   // standard constructor
		virtual ~CStatusDlg();

	// Dialog Data
		enum { IDD = IDD_MESSAGEDLG };

	protected:
		virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

		DECLARE_MESSAGE_MAP()
		
	public:
		// ------------------- DIALOG COMPONENTS ----------------------	
		
		/** the status messages */
		CListBox m_statusListBox;
		
		
		// -------------------- MESSAGE HANDLING ------------------------
		
		/** Called to update the status box */
		afx_msg LRESULT OnShowMessage(WPARAM wParam, LPARAM lParam);
	
		/** Called when the size of the window is changed */
		afx_msg void OnSize(UINT nType, int cx, int cy);
		afx_msg void OnClose();
	};
}