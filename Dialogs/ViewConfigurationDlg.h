#pragma once
#include "afxcmn.h"

#include "ViewLocationPage.h"
#include "ViewFitWindowPage.h"
#include "ViewInstrumentPage.h"

// CViewConfigurationDlg dialog
namespace Dialogs{
	class CViewConfigurationDlg : public CDialog
	{
		DECLARE_DYNAMIC(CViewConfigurationDlg)

	public:
		CViewConfigurationDlg(CWnd* pParent = NULL);   // standard constructor
		virtual ~CViewConfigurationDlg();

	// Dialog Data
		enum { IDD = IDD_VIEW_CONFIGURATION_DLG };

	protected:
		virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
		virtual BOOL OnInitDialog();

		DECLARE_MESSAGE_MAP()
	public:


		// ----------------------------------------------------------------------
		// ---------------------- PUBLIC DATA -----------------------------------
		// ----------------------------------------------------------------------

		/** the selected volcano, the one we should show the configuration for. 
			If this is equal to -1 then the configuration for all volcanoes will
				be showed */
		int	m_selectedVolcano;

		// ----------------------------------------------------------------------
		// --------------------- PUBLIC METHODS ---------------------------------
		// ----------------------------------------------------------------------

		/** Fills the current configuration into the tree */
		void PopulateTree();

	private:
		// ----------------------------------------------------------------------
		// ---------------------- PRIVATE DATA ----------------------------------
		// ----------------------------------------------------------------------

		/** The tree... */
		CTreeCtrl m_treeCtrl;

		/** The page that shows the location information */
		CViewLocationPage m_locationPage;

		/** The page that shows the fit-window information */
		CViewFitWindowPage m_fitWindowPage;
		
		/** The page that shows the instrument information */
		CViewInstrumentPage m_instrumentPage;
		
		/** The sheet, this acts as a container for the 'm_locationPage' and 
			'm_fitWindowPage'. */
		CPropertySheet	m_sheet;
		CStatic			m_sheetFrame;


		// ----------------------------------------------------------------------
		// --------------------- PRIVATE METHODS --------------------------------
		// ----------------------------------------------------------------------

		/** Called when the selection item in the tree has changed */
		void OnChangedSelectedTreeItem(NMHDR* pNMHDR, LRESULT* pResult);
		

	};
}