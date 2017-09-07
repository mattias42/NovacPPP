// ViewLocationPage.cpp : implementation file
//

#include "stdafx.h"
#include "../NovacPPP.h"
#include "ViewLocationPage.h"

using namespace Dialogs;

// CViewLocationPage dialog

IMPLEMENT_DYNAMIC(CViewLocationPage, CPropertyPage)
CViewLocationPage::CViewLocationPage()
	: CPropertyPage(CViewLocationPage::IDD)
{
}

CViewLocationPage::~CViewLocationPage()
{
}

void CViewLocationPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	unsigned int y1, y2;
	
	// The name
	DDX_Text(pDX, IDC_EDIT_LOCATIONNAME, m_location.m_locationName);

	// The volcano
	DDX_Text(pDX, IDC_EDIT_VOLCANO, m_location.m_volcano);

	// The valid time
	if(pDX->m_bSaveAndValidate){
		DDX_Text(pDX, IDC_EDIT_VALIDFROMYEAR,	y1);
		DDX_Text(pDX, IDC_EDIT_VALIDTOYEAR,		y2);
		m_location.m_validFrom.year = y1;
		m_location.m_validTo.year	= y2;
	}else{
		y1 = m_location.m_validFrom.year;
		y2 = m_location.m_validTo.year;
		DDX_Text(pDX, IDC_EDIT_VALIDFROMYEAR,	y1);
		DDX_Text(pDX, IDC_EDIT_VALIDTOYEAR,		y2);
	}
	DDX_Text(pDX, IDC_EDIT_VALIDFROMMONTH,	m_location.m_validFrom.month);
	DDX_Text(pDX, IDC_EDIT_VALIDFROMDAY,	m_location.m_validFrom.day);
	DDX_Text(pDX, IDC_EDIT_VALIDTOMONTH,	m_location.m_validTo.month);
	DDX_Text(pDX, IDC_EDIT_VALIDTODAY,		m_location.m_validTo.day);
	
	// The position
	DDX_Text(pDX, IDC_EDIT_LATITUDE,	m_location.m_latitude);
	DDX_Text(pDX, IDC_EDIT_LONGITUDE,	m_location.m_longitude);
	DDX_Text(pDX, IDC_EDIT_ALTITUDE,	m_location.m_altitude);
	
	// The instrument setup
	DDX_Text(pDX, IDC_EDIT_COMPASS,		m_location.m_compass);
	DDX_Text(pDX, IDC_EDIT_CONEANGLE,	m_location.m_coneangle);
	
	// The type of instrument
	DDX_Control(pDX, IDC_COMBO_INSTRUMENTTYPE, m_instrumentCombo);
	if(pDX->m_bSaveAndValidate){
		this->m_location.m_instrumentType = (INSTRUMENT_TYPE)m_instrumentCombo.GetCurSel();
	}else{
		if(m_location.m_instrumentType == INSTR_GOTHENBURG)
			m_instrumentCombo.SetCurSel(0);
		else if(m_location.m_instrumentType == INSTR_HEIDELBERG)
			m_instrumentCombo.SetCurSel(1);
	}
	
	// The type of spectrometer
	DDX_Control(pDX, IDC_COMBO_SPECTROMETER, m_spectrometerCombo);
	if(pDX->m_bSaveAndValidate){
		this->m_location.m_spectrometerModel = (SPECTROMETER_MODEL)m_spectrometerCombo.GetCurSel();
	}else{
		m_spectrometerCombo.SetCurSel((int)m_location.m_spectrometerModel);
	}
	
	
	
}


BEGIN_MESSAGE_MAP(CViewLocationPage, CPropertyPage)
END_MESSAGE_MAP()


BOOL CViewLocationPage::OnInitDialog()
{
	CPropertyPage::OnInitDialog();
	CString modelName;
	
	// the type of instrument
	this->m_instrumentCombo.SetCurSel(0);

	// the type of spectrometer
	for(int k = 0; k < NUM_CONF_SPEC_MODELS; ++k){
		CSpectrometerModel::ToString((SPECTROMETER_MODEL)k, modelName);
		m_spectrometerCombo.AddString(modelName);
	}
	m_spectrometerCombo.SetCurSel((int)m_location.m_spectrometerModel);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CViewLocationPage::OnSetActive()
{
	UpdateData(FALSE);
	return CPropertyPage::OnSetActive();
}