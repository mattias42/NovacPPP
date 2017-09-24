#pragma once


// CViewInstrumentPage dialog
namespace Dialogs{
	class CViewInstrumentPage : public CPropertyPage
	{
		DECLARE_DYNAMIC(CViewInstrumentPage)

	public:
		CViewInstrumentPage();
		virtual ~CViewInstrumentPage();

	// Dialog Data
		enum { IDD = IDD_VIEWINSTRUMENTPAGE };

	protected:
		virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

		DECLARE_MESSAGE_MAP()
	};
}