// ViewFitWindowPage.cpp : implementation file
//

#include "stdafx.h"
#include "../NovacPPP.h"
#include "ViewFitWindowPage.h"

using namespace Dialogs;

// CViewFitWindowPage dialog

IMPLEMENT_DYNAMIC(CViewFitWindowPage, CPropertyPage)
CViewFitWindowPage::CViewFitWindowPage()
	: CPropertyPage(CViewFitWindowPage::IDD)
{
}

CViewFitWindowPage::~CViewFitWindowPage()
{
}

void CViewFitWindowPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	unsigned int y1, y2;
	
	// The name
	DDX_Text(pDX, IDC_EDIT_FITWINDOWNAME, m_window.name);
	
	// The valid time
	if(pDX->m_bSaveAndValidate){
		DDX_Text(pDX, IDC_EDIT_VALIDFROMYEAR,	y1);
		DDX_Text(pDX, IDC_EDIT_VALIDTOYEAR,		y2);
		m_validFrom.year = y1;
		m_validTo.year	= y2;
	}else{
		y1 = m_validFrom.year;
		y2 = m_validTo.year;
		DDX_Text(pDX, IDC_EDIT_VALIDFROMYEAR,	y1);
		DDX_Text(pDX, IDC_EDIT_VALIDTOYEAR,		y2);
	}
	DDX_Text(pDX, IDC_EDIT_VALIDFROMMONTH,	m_validFrom.month);
	DDX_Text(pDX, IDC_EDIT_VALIDFROMDAY,	m_validFrom.day);
	DDX_Text(pDX, IDC_EDIT_VALIDTOMONTH,	m_validTo.month);
	DDX_Text(pDX, IDC_EDIT_VALIDTODAY,		m_validTo.day);
	
	// The fit range
	DDX_Text(pDX, IDC_EDIT_FITFROM, m_window.fitLow);
	DDX_Text(pDX, IDC_EDIT_FITTO,	m_window.fitHigh);
	
	// The fit type
	DDX_Radio(pDX, IDC_RADIO_FITTYPE1, (int &)m_window.fitType);
	
	// The spectra
	DDX_Text(pDX, IDC_EDIT_SPECTRUMSIZE,			m_window.specLength);
	DDX_Text(pDX, IDC_EDIT_SPECTRUM_INTERLACESTEPS, m_window.interlaceStep);
	DDX_Text(pDX, IDC_EDIT_SPECTRUM_STARTCHN,		m_window.startChannel);
	
	// Other fit parameters
	DDX_Text(pDX, IDC_EDIT_POLYORDER, m_window.polyOrder);
	
	DDX_Control(pDX, IDC_REFERENCE_FRAME, m_referenceFrame);
	
	if(pDX->m_bSaveAndValidate == 0 && m_referenceFileCtrl.m_hWnd != 0)
		PopulateReferenceFileControl();
}


BEGIN_MESSAGE_MAP(CViewFitWindowPage, CPropertyPage)
END_MESSAGE_MAP()


BOOL CViewFitWindowPage::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	// Initialize the reference grid control
	InitReferenceFileControl();
	PopulateReferenceFileControl();
	

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CViewFitWindowPage::OnSetActive()
{
	PopulateReferenceFileControl();
	UpdateData(FALSE);
	return CPropertyPage::OnSetActive();
}


void CViewFitWindowPage::InitReferenceFileControl(){
	m_referenceFileCtrl.m_window = &this->m_window;

	CRect rect;
	m_referenceFrame.GetWindowRect(&rect);
	int width = rect.right - rect.left;
	int height = rect.bottom - rect.top;

	if(m_referenceFileCtrl.m_hWnd != NULL)
		return;

	rect.top = 20;
	rect.left = 10;
	rect.right = width - 20;
	rect.bottom = height - 10;
	this->m_referenceFileCtrl.Create(rect, &m_referenceFrame, 0);

	m_referenceFileCtrl.InsertColumn("Name");
	m_referenceFileCtrl.SetColumnWidth(0, (int)(rect.right / 9));
	m_referenceFileCtrl.InsertColumn("Reference File");
	m_referenceFileCtrl.SetColumnWidth(1, (int)(rect.right * 5 / 8));
	m_referenceFileCtrl.InsertColumn("Shift");
	m_referenceFileCtrl.SetColumnWidth(2, (int)(rect.right / 8));
	m_referenceFileCtrl.InsertColumn("Squeeze");
	m_referenceFileCtrl.SetColumnWidth(3, (int)(rect.right / 8));

	m_referenceFileCtrl.SetFixedRowCount(1);
	m_referenceFileCtrl.SetEditable(TRUE); /* make sure the user can edit the positions */
	m_referenceFileCtrl.SetRowCount(3);
	m_referenceFileCtrl.EnableTitleTips(FALSE);	// <-- Disable the small title tips
	m_referenceFileCtrl.parent = this;
}

/** Fills in the reference file control */
void CViewFitWindowPage::PopulateReferenceFileControl(){
	long i;

	// Clear the control
	m_referenceFileCtrl.DeleteNonFixedRows();

	m_referenceFileCtrl.m_window = &this->m_window;

	long numberOfReferences = m_window.nRef;
	m_referenceFileCtrl.SetRowCount(2 + numberOfReferences);

	for(i = 0; i < numberOfReferences; ++i){
		Evaluation::CReferenceFile &ref = m_window.ref[i];

		m_referenceFileCtrl.SetItemTextFmt(i+1, 0, ref.m_specieName);
		m_referenceFileCtrl.SetItemTextFmt(i+1, 1, ref.m_path);

		if(ref.m_shiftOption == Evaluation::SHIFT_FREE)
		m_referenceFileCtrl.SetItemTextFmt(1 + i, 2, "free");

		if(ref.m_shiftOption == Evaluation::SHIFT_FIX)
		m_referenceFileCtrl.SetItemTextFmt(1 + i, 2, "fix to %.2lf", ref.m_shiftValue);

		if(ref.m_shiftOption == Evaluation::SHIFT_LINK)
		m_referenceFileCtrl.SetItemTextFmt(1 + i, 2, "link to %.2lf", ref.m_shiftValue);

		if(ref.m_shiftOption == Evaluation::SHIFT_LIMIT)
		m_referenceFileCtrl.SetItemTextFmt(1 + i, 2, "limit to %.2lf", ref.m_shiftValue);

		if(ref.m_squeezeOption == Evaluation::SHIFT_FREE)
		m_referenceFileCtrl.SetItemTextFmt(1 + i, 3, "free");

		if(ref.m_squeezeOption == Evaluation::SHIFT_FIX)
		m_referenceFileCtrl.SetItemTextFmt(1 + i, 3, "fix to %.2lf", ref.m_squeezeValue);

		if(ref.m_squeezeOption == Evaluation::SHIFT_LINK)
		m_referenceFileCtrl.SetItemTextFmt(1 + i, 3, "link to %.2lf", ref.m_squeezeValue);

		if(ref.m_squeezeOption == Evaluation::SHIFT_LIMIT)
		m_referenceFileCtrl.SetItemTextFmt(1 + i, 3, "limit to %.2lf", ref.m_squeezeValue);
	}
	// the last line should be cleared
	m_referenceFileCtrl.SetItemTextFmt(i + 1, 0, "");
	m_referenceFileCtrl.SetItemTextFmt(i + 1, 1, "");
	m_referenceFileCtrl.SetItemTextFmt(i + 1, 2, "");
	m_referenceFileCtrl.SetItemTextFmt(i + 1, 3, "");

	// Update the grid
	m_referenceFileCtrl.UpdateData(FALSE);
}