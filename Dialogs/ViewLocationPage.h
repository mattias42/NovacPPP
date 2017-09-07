#pragma once

#include "../Configuration/InstrumentLocation.h"

// CViewLocationPage dialog
namespace Dialogs{
	class CViewLocationPage : public CPropertyPage
	{
		DECLARE_DYNAMIC(CViewLocationPage)

	public:
		CViewLocationPage();
		virtual ~CViewLocationPage();

	// Dialog Data
		enum { IDD = IDD_VIEWLOCATIONPAGE };

	protected:
		virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

		DECLARE_MESSAGE_MAP()
		
		virtual BOOL OnInitDialog();
		virtual BOOL OnSetActive();
public:
		// ----------------------------------------------------------------------
		// ---------------------- PUBLIC DATA -----------------------------------
		// ----------------------------------------------------------------------

		/** This is the location that we draw to the screen */
		Configuration::CInstrumentLocation m_location;

		// ----------------------------------------------------------------------
		// --------------------- PUBLIC METHODS ---------------------------------
		// ----------------------------------------------------------------------

private:
		// ----------------------------------------------------------------------
		// ---------------------- PRIVATE DATA ----------------------------------
		// ----------------------------------------------------------------------

		CComboBox m_instrumentCombo;
		CComboBox m_spectrometerCombo;

		// ----------------------------------------------------------------------
		// --------------------- PRIVATE METHODS --------------------------------
		// ----------------------------------------------------------------------
				
	};
}