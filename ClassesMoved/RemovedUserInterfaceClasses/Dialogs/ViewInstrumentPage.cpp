// ViewInstrumentPage.cpp : implementation file
//

#include "stdafx.h"
#include "../NovacPPP.h"
#include "ViewInstrumentPage.h"


// CViewInstrumentPage dialog

using namespace Dialogs;

IMPLEMENT_DYNAMIC(CViewInstrumentPage, CPropertyPage)
CViewInstrumentPage::CViewInstrumentPage()
	: CPropertyPage(CViewInstrumentPage::IDD)
{
}

CViewInstrumentPage::~CViewInstrumentPage()
{
}

void CViewInstrumentPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CViewInstrumentPage, CPropertyPage)
END_MESSAGE_MAP()


// CViewInstrumentPage message handlers
