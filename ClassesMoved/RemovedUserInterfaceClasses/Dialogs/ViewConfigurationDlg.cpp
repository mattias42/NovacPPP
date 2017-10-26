// ViewConfigurationDlg.cpp : implementation file
//

#include "stdafx.h"
#include "../NovacPPP.h"
#include "../VolcanoInfo.h"
#include "../Common/Common.h"
#include "../Configuration/NovacPPPConfiguration.h"
#include "ViewConfigurationDlg.h"

extern Configuration::CNovacPPPConfiguration        g_setup;	   // <-- The settings
extern CVolcanoInfo									g_volcanoes;   // <-- A list of all known volcanoes

// CViewConfigurationDlg dialog

using namespace Dialogs;

IMPLEMENT_DYNAMIC(CViewConfigurationDlg, CDialog)
CViewConfigurationDlg::CViewConfigurationDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CViewConfigurationDlg::IDD, pParent)
{
	m_selectedVolcano = -1;
}

CViewConfigurationDlg::~CViewConfigurationDlg()
{
}

void CViewConfigurationDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CONFIGURATION_TREE,	m_treeCtrl);
	DDX_Control(pDX, IDC_CONFIGURATION_FRAME,	m_sheetFrame);
}


BEGIN_MESSAGE_MAP(CViewConfigurationDlg, CDialog)
	ON_NOTIFY(TVN_SELCHANGED, IDC_CONFIGURATION_TREE, OnChangedSelectedTreeItem)
END_MESSAGE_MAP()

BOOL CViewConfigurationDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	
	CRect rect, rect2;

	// Populate the tree control
	PopulateTree();

	// Construct the sheet
	m_sheet.Construct("", this);

	// Create the pages
	m_locationPage.Construct(IDD_VIEWLOCATIONPAGE);
	m_fitWindowPage.Construct(IDD_VIEWFITWINDOWPAGE);
	m_instrumentPage.Construct(IDD_VIEWINSTRUMENTPAGE);

	// Add the pages to the sheet
	m_sheet.AddPage(&m_locationPage);
	m_sheet.AddPage(&m_fitWindowPage);
	m_sheet.AddPage(&m_instrumentPage);

	// Create the sheet
	m_sheet.Create(this, WS_CHILD | WS_VISIBLE | WS_TABSTOP);
	m_sheet.ModifyStyleEx(0, WS_EX_CONTROLPARENT);
	
	// remove the tab-buttons at the top
	CTabCtrl *tab = m_sheet.GetTabControl();
	tab->ShowWindow(SW_HIDE);

	// move the sheet to a more 'nicely looking' position
	m_sheetFrame.GetWindowRect(rect);
	this->ScreenToClient(rect);
	m_sheet.MoveWindow(rect.left, rect.top, rect.Width() - 20, rect.Height() - 20);

	m_sheet.SetActivePage(&m_instrumentPage);

	return FALSE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

/** Fills the current configuration into the tree */
void CViewConfigurationDlg::PopulateTree(){
	unsigned int j, k; // iterator
	CString label;
	CDateTime from, to;

	// Clear
	m_treeCtrl.DeleteAllItems();

	// ---------------- Insert the instruments ---------------
	HTREEITEM allInstrumentNode = m_treeCtrl.InsertItem("Instruments");
	m_treeCtrl.Expand(allInstrumentNode, TVE_EXPAND);
	for(k = 0; k < g_setup.m_instrumentNum; ++k){
		bool insert = false;
		
		// If we should show all instruments then insert this one...
		if(this->m_selectedVolcano == -1)
			insert = true;
		
		// Loop through the locations for this instrument and insert it if
		//	 any of the locations belongs to the currently selected volcano
		Configuration::CInstrumentLocation loc;
		for(j = 0; j < g_setup.m_instrument[k].m_location.GetLocationNum(); ++j){
			g_setup.m_instrument[k].m_location.GetLocation(j, loc);
			if(Equals(loc.m_volcano, g_volcanoes.GetVolcanoName(m_selectedVolcano)) || Equals(loc.m_volcano, g_volcanoes.GetSimpleVolcanoName(m_selectedVolcano))){
				insert = true;
				break;
			}
		}
		
		// this instrument does not belong to this volcano, don't insert it
		if(!insert)
			continue;
			
		// Insert this instrument
		HTREEITEM curInstrumentNode = m_treeCtrl.InsertItem(g_setup.m_instrument[k].m_serial, allInstrumentNode, TVI_LAST);

		if(curInstrumentNode == NULL)
			continue; // failed to insert
		
		// insert each of the fit-windows and each of the locations...
		for(j = 0; j < g_setup.m_instrument[k].m_location.GetLocationNum(); ++j){
			g_setup.m_instrument[k].m_location.GetLocation(j, loc);
			
			label.Format("Location: %s", loc.m_locationName);
			m_treeCtrl.InsertItem(label, curInstrumentNode, TVI_LAST);
		}

		Evaluation::CFitWindow window;
		for(j = 0; j < g_setup.m_instrument[k].m_eval.GetFitWindowNum(); ++j){
			g_setup.m_instrument[k].m_eval.GetFitWindow(j, window, from, to);
			
			if(window.channel == 0)
				label.Format("FitWindow: %s", window.name);
			else
				label.Format("FitWindow: %s (Slave)", window.name);

			m_treeCtrl.InsertItem(label, curInstrumentNode, TVI_LAST);
		}
		
		m_treeCtrl.Expand(curInstrumentNode, TVE_EXPAND);
	}

	m_treeCtrl.Expand(allInstrumentNode, TVE_EXPAND);
	m_treeCtrl.ModifyStyle(0, TVS_LINESATROOT);

	// ---------------- Insert the wind field ---------------
	HTREEITEM windFieldNode = m_treeCtrl.InsertItem("Wind field");
	
	UpdateData(FALSE);
}

/** Called when the selection item in the tree has changed */
void CViewConfigurationDlg::OnChangedSelectedTreeItem(NMHDR* pNMHDR, LRESULT* pResult){
	CDateTime from, to; 

	if(m_sheet.m_hWnd == NULL)
		return;

	// Get the selected item
	HTREEITEM selectedItem = m_treeCtrl.GetSelectedItem();
	if(selectedItem == NULL){
		// nothing selected
		return;
	}

	// Get the legend of the selected item
	CString selectedString = m_treeCtrl.GetItemText(selectedItem);

	// If this is a location or a fit-window
	if(Equals(selectedString.Left(9), "Location:")){
		// Get the parent item
		HTREEITEM parentItem = m_treeCtrl.GetParentItem(selectedItem);
		if(parentItem == NULL){
			// error
			return;
		}

		// get the legend of the parent item, this is the serial-number of the instrument
		CString serial = m_treeCtrl.GetItemText(parentItem);
	
		// find the correct location in the configuration...
		Configuration::CInstrumentLocation location;
		for(unsigned int k = 0; k < g_setup.m_instrumentNum; ++k){
			if(Equals(g_setup.m_instrument[k].m_serial, serial)){
				for(unsigned int j = 0; j < g_setup.m_instrument[k].m_location.GetLocationNum(); ++j){
					g_setup.m_instrument[k].m_location.GetLocation(j, location);
					
					if(Equals(location.m_locationName, selectedString.Right(selectedString.GetLength() - 10))){
						m_sheet.SetActivePage(&m_locationPage);
						m_locationPage.m_location = location;
						m_locationPage.UpdateData(FALSE);
						return;
					}
				}
			}
		}
		
	}else if(Equals(selectedString.Left(10), "FitWindow:")){
		// Get the parent item
		HTREEITEM parentItem = m_treeCtrl.GetParentItem(selectedItem);
		if(parentItem == NULL){
			// error
			return;
		}

		// get the legend of the parent item, this is the serial-number of the instrument
		CString serial = m_treeCtrl.GetItemText(parentItem);
	
		// get the channel of the spectrometer
		int channel = 0;
		if(-1 != selectedString.Find("(Slave)"))
			channel = 1;
	
		// find the correct fit-window in the configuration...
		Evaluation::CFitWindow window;
		for(unsigned int k = 0; k < g_setup.m_instrumentNum; ++k){
			if(Equals(g_setup.m_instrument[k].m_serial, serial)){
				for(unsigned int j = 0; j < g_setup.m_instrument[k].m_eval.GetFitWindowNum(); ++j){
					g_setup.m_instrument[k].m_eval.GetFitWindow(j, window, from, to);
					
					bool correctWindow = false;
					
					if(channel == 0){
						if(Equals(window.name, selectedString.Right(selectedString.GetLength() - 11)) && (window.channel == channel)){
							correctWindow = true;
						}
					}else{
						CString tmpStr;
						tmpStr.Format("%s (Slave)", window.name);
						if(Equals(tmpStr, selectedString.Right(selectedString.GetLength() - 11)) && (window.channel == channel)){
							correctWindow = true;
						}
					}
					
					if(correctWindow){
						m_sheet.SetActivePage(&m_fitWindowPage);

						m_fitWindowPage.m_window = window;
						m_fitWindowPage.m_validFrom = from;
						m_fitWindowPage.m_validTo = to;

						m_fitWindowPage.UpdateData(FALSE);
						return;
					}
				}
			}
		}
	}else{
		// this is not a fit-window or a location
		m_sheet.SetActivePage(&m_instrumentPage);
		return;
	}
	
}
