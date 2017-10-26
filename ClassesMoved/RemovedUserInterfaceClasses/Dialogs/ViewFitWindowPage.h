#pragma once

#include "../Evaluation/FitWindow.h"
#include "../DlgControls/ReferenceFileControl.h"

// CViewFitWindowPage dialog
namespace Dialogs{
	class CViewFitWindowPage : public CPropertyPage
	{
		DECLARE_DYNAMIC(CViewFitWindowPage)

	public:
		CViewFitWindowPage();
		virtual ~CViewFitWindowPage();

	// Dialog Data
		enum { IDD = IDD_VIEWFITWINDOWPAGE };

	protected:
		virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

		DECLARE_MESSAGE_MAP()
		
		virtual BOOL OnInitDialog();
		virtual BOOL OnSetActive();
public:
		// ----------------------------------------------------------------------
		// ---------------------- PUBLIC DATA -----------------------------------
		// ----------------------------------------------------------------------

		/** This is the fit-window that we draw to the screen */
		Evaluation::CFitWindow m_window;
		
		CDateTime	m_validFrom;
		CDateTime	m_validTo;

		// ----------------------------------------------------------------------
		// --------------------- PUBLIC METHODS ---------------------------------
		// ----------------------------------------------------------------------

private:
		// ----------------------------------------------------------------------
		// ---------------------- PRIVATE DATA ----------------------------------
		// ----------------------------------------------------------------------

		CStatic m_referenceFrame;
		
		DlgControls::CReferenceFileControl m_referenceFileCtrl;

		// ----------------------------------------------------------------------
		// --------------------- PRIVATE METHODS --------------------------------
		// ----------------------------------------------------------------------
		
		void InitReferenceFileControl();
		void PopulateReferenceFileControl();
	};
}