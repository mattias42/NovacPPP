// NovacPPPDlg.cpp : implementation file
//

#include "stdafx.h"
#include "NovacPPP.h"
#include "NovacPPPDlg.h"

#include "VolcanoInfo.h"
#include "StringTokenizer.h"
#include "Common/Common.h"
#include "PostProcessing.h"
#include "Configuration/NovacPPPConfiguration.h"
#include "Configuration/UserConfiguration.h"
#include "SetupFileReader.h"
#include "Common/EvaluationConfigurationParser.h"
#include "Common/ProcessingFileReader.h"
#include "Meteorology/XMLWindFileReader.h"
#include "Dialogs/ViewconfigurationDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CDialog *pView;

extern Configuration::CNovacPPPConfiguration        g_setup;	   // <-- The settings
extern Configuration::CUserConfiguration			g_userSettings;// <-- The settings of the user
extern CVolcanoInfo									g_volcanoes;   // <-- A list of all known volcanoes

UINT CalculateAllFluxes(LPVOID pParam);

// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CNovacPPPDlg dialog



CNovacPPPDlg::CNovacPPPDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CNovacPPPDlg::IDD, pParent)
	, m_FromYear(2005)
	, m_FromMonth(10)
	, m_FromDay(01)
	, m_ToYear(Common::GetYear())
	, m_ToMonth(Common::GetMonth())
	, m_ToDay(Common::GetDay())
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

#ifdef _DEBUG
	g_userSettings.m_LocalDirectory.Format("D:\\NovacPPPTest\\");
#endif
}

void CNovacPPPDlg::DoDataExchange(CDataExchange* pDX)
{
	CString year, month, day;
	
	CDialog::DoDataExchange(pDX);
	
	// the volcano
	DDX_Control(pDX, IDC_COMBO_ALL_VOLCANOES, m_comboAllVolcanoes);
	
	// The range of times...
	if(pDX->m_bSaveAndValidate){
		// we're saving data
		DDX_Text(pDX, IDC_EDIT_FROM_YEAR, m_FromYear);
		DDV_MinMaxInt(pDX, m_FromYear, 2005, 2100);
		DDX_Text(pDX, IDC_EDIT_FROM_MONTH, m_FromMonth);
		DDV_MinMaxInt(pDX, m_FromMonth, 1, 12);
		DDX_Text(pDX, IDC_EDIT_FROM_DAY, m_FromDay);
		DDV_MinMaxInt(pDX, m_FromDay, 1, 31);
		DDX_Text(pDX, IDC_EDIT_TO_YEAR, m_ToYear);
		DDV_MinMaxInt(pDX, m_ToYear, 2005, 2100);
		DDX_Text(pDX, IDC_EDIT_TO_MONTH, m_ToMonth);
		DDV_MinMaxInt(pDX, m_ToMonth, 1, 12);
		DDX_Text(pDX, IDC_EDIT_TO_DAY, m_ToDay);
		DDV_MinMaxInt(pDX, m_ToDay, 1, 31);	
	}else{
		// we're filling in the dialog
		year.Format("%04d", m_FromYear);
		month.Format("%02d", m_FromMonth);
		day.Format("%02d", m_FromDay);
		DDX_Text(pDX, IDC_EDIT_FROM_YEAR, year);
		DDX_Text(pDX, IDC_EDIT_FROM_MONTH, month);
		DDX_Text(pDX, IDC_EDIT_FROM_DAY, day);

		year.Format("%04d", m_ToYear);
		month.Format("%02d", m_ToMonth);
		day.Format("%02d", m_ToDay);
		DDX_Text(pDX, IDC_EDIT_TO_YEAR, year);
		DDX_Text(pDX, IDC_EDIT_TO_MONTH, month);
		DDX_Text(pDX, IDC_EDIT_TO_DAY, day);
	}

	// the settings for how the get the data
	DDX_Text(pDX, IDC_EDIT_LOCAL_DIRECTORY, g_userSettings.m_LocalDirectory);
	DDX_Text(pDX, IDC_EDIT_FTP_DIRECTORY,	g_userSettings.m_FTPDirectory);
	DDX_Text(pDX, IDC_EDIT_FTPUSERNAME,		g_userSettings.m_FTPUsername);
	DDX_Text(pDX, IDC_EDIT_FTPPASSWORD,		g_userSettings.m_FTPPassword);

	DDX_Check(pDX, IDC_CHECK_INCLUDESUBDIRS,		g_userSettings.m_includeSubDirectories_Local);
	DDX_Check(pDX, IDC_CHECK_INCLUDESUBDIRS_FTP,	g_userSettings.m_includeSubDirectories_FTP);

	DDX_Control(pDX, IDC_CHECK_LOCAL,				m_CheckLocalDirectory);
	DDX_Control(pDX, IDC_CHECK_FTP,					m_CheckFtp);
	DDX_Control(pDX, IDC_CHECK_DATABASE,			m_CheckDatabase);
	DDX_Control(pDX, IDC_CHECK_INCLUDESUBDIRS,		m_CheckIncludeSubDirsLocal);
	DDX_Control(pDX, IDC_CHECK_INCLUDESUBDIRS_FTP,	m_CheckIncludeSubDirsFTP);
	
	// The wind field
	DDX_Text(pDX, IDC_EDIT_WINDFIELDFILE,			g_userSettings.m_windFieldFile);
	
	// Controlling the dialog
	DDX_Control(pDX, IDC_EDIT_LOCAL_DIRECTORY,	m_editLocalDir);
	DDX_Control(pDX, IDC_EDIT_FTP_DIRECTORY,	m_editFTPDir);
	DDX_Control(pDX, IDC_EDIT_FTPPASSWORD,		m_editFTPpwd);
	DDX_Control(pDX, IDC_EDIT_FTPUSERNAME,		m_editFTPuser);
}

BEGIN_MESSAGE_MAP(CNovacPPPDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	
	// The very important button!!!
	ON_BN_CLICKED(IDC_BUTTON_CALCULATE_FLUXES, OnBnClickedCalculateFluxes)
	
	// The menu
	ON_COMMAND(ID_VIEW_CONFIGURATIONFORSELECTEDVOLCANO,				OnMenuViewConfigurationForSelectedVolcano)
	ON_UPDATE_COMMAND_UI(ID_VIEW_CONFIGURATIONFORSELECTEDVOLCANO,	OnUpdateViewConfigurationForSelectedVolcano)
	ON_CBN_SELCHANGE(IDC_COMBO_ALL_VOLCANOES,						OnChangeSelectedVolcano)
	ON_BN_CLICKED(IDC_CHECK_LOCAL,									OnBnClickedCheckLocal)
	ON_BN_CLICKED(IDC_CHECK_FTP,									OnBnClickedCheckFtp)
	ON_WM_CLOSE()
END_MESSAGE_MAP()


// CNovacPPPDlg message handlers

BOOL CNovacPPPDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Declaration of variables and objects
	Common common;
	CString setupPath, processingPath, evalConfPath, message, volcanoName;
	FileHandler::CSetupFileReader reader;
	FileHandler::CEvaluationConfigurationParser eval_reader;
	FileHandler::CProcessingFileReader processing_reader;

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// Setup the list of volcanoes in the combo-box in the interface
	for(unsigned int k = 0; k < g_volcanoes.m_volcanoNum; ++k){
		volcanoName.Format("%s     [%s]", g_volcanoes.GetVolcanoName(k), g_volcanoes.GetVolcanoCode(k));
		m_comboAllVolcanoes.AddString(volcanoName);
	}
#ifdef _DEBUG
	m_comboAllVolcanoes.SetCurSel(5);
#endif
	//Read configuration from file setup.xml */	
	common.GetExePath();
	setupPath.Format("%s\\configuration\\setup.xml", common.m_exePath);
	if(SUCCESS != reader.ReadSetupFile(setupPath, g_setup)){
		MessageBox("Could not read setup.xml. Setup not complete. Please fix and try again");
	}
	
	// Read the users options from file processing.xml
	processingPath.Format("%s\\configuration\\processing.xml", common.m_exePath);
	if(SUCCESS != processing_reader.ReadProcessingFile(processingPath, g_userSettings)){
		MessageBox("Could not read processing.xml. Setup not complete. Please fix and try again");
	}
	
	// Check if there is a configuration file for every spectrometer serial number
	bool failed = false;
	for(k = 0; k < g_setup.m_instrumentNum; ++k){
		evalConfPath.Format("%s\\configuration\\%s.exml", common.m_exePath, g_setup.m_instrument[k].m_serial);

		if(IsExistingFile(evalConfPath))
			eval_reader.ReadConfigurationFile(evalConfPath, &g_setup.m_instrument[k].m_eval, &g_setup.m_instrument[k].m_darkCurrentCorrection);
		else{
			message.Format("Could not find configuration file: %s", evalConfPath);
			MessageBox(message);
			failed = true;
		}
	}
	if(failed){
        // exit the application
        ASSERT(AfxGetApp()->m_pMainWnd != NULL);
        AfxGetApp()->m_pMainWnd->SendMessage(WM_CLOSE);
	}
	
	// Get the options from the command line
	ParseCommandLineOptions();
	
	// Update the interface according to the options found in the 'processing.xml' file	
	UpdateInterface();
	UpdateData(FALSE);
	
	// Set the currently selected volcano
	m_comboAllVolcanoes.SetCurSel(g_userSettings.m_volcano);
	
	// enable or disable the controls
	EnableControls();

	// If we are supposed to start the processing automatically, then let's do so
	if(g_userSettings.m_startNow){
		OnBnClickedCalculateFluxes();
	}
		
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CNovacPPPDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CNovacPPPDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CNovacPPPDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CNovacPPPDlg::OnMenuViewConfigurationForSelectedVolcano(){
	Dialogs::CViewConfigurationDlg dlg;

	UpdateData(TRUE);

	// Get the selected volcano
	dlg.m_selectedVolcano = m_comboAllVolcanoes.GetCurSel();
	
	// Open the dialog
	dlg.DoModal();
}

void CNovacPPPDlg::OnBnClickedCalculateFluxes()
{
	CRect rect;

	UpdateData(TRUE);

	// 1. check input!!!
	int curSel = m_comboAllVolcanoes.GetCurSel();
	if(curSel < 0){
		MessageBox("You have to choose a volcano!!!");
		return;
	}

	if(m_CheckLocalDirectory.GetCheck() != BST_CHECKED){
		g_userSettings.m_LocalDirectory.Format("");
	}

	if(m_CheckFtp.GetCheck() != BST_CHECKED){
		g_userSettings.m_FTPDirectory.Format("");
	}
	
	if(m_CheckDatabase.GetCheck() == BST_CHECKED){
		MessageBox("Can not yet search for files on Database server. Sorry!!");
		return;
	}

	// 2. check the data
	
	// 2a. make sure that the ftp-path ends with a '/'
	if(g_userSettings.m_FTPDirectory.GetLength() > 1){
		if(!Equals(g_userSettings.m_FTPDirectory.Right(1), "/")){
			g_userSettings.m_FTPDirectory.AppendFormat("/");
		}
	}
	

	// 3. Open the message window
	pView = &m_statusDlg;
	m_statusDlg.Create(IDD_MESSAGEDLG);
	m_statusDlg.ShowWindow(SW_SHOW);
	m_statusDlg.GetWindowRect(rect);
	int w = rect.Width();
	int h = rect.Height();
	rect.left = 10; rect.right = rect.left + w;
	rect.top = 10; rect.bottom = rect.top + h;
	m_statusDlg.MoveWindow(rect); // move the window to the corner of the screen

	// 4. Set the parameters for the post-processing..
	g_userSettings.m_volcano  = this->m_comboAllVolcanoes.GetCurSel();
	g_userSettings.m_fromDate = CDateTime(this->m_FromYear, this->m_FromMonth,	this->m_FromDay, 00, 00, 00);
	g_userSettings.m_toDate	  = CDateTime(this->m_ToYear,	this->m_ToMonth,	this->m_ToDay, 23, 59, 59);

	// 5. Run
	CWinThread *postProcessingthread = AfxBeginThread(CalculateAllFluxes, NULL, THREAD_PRIORITY_NORMAL, 0, 0, NULL);
	Common::SetThreadName(postProcessingthread->m_nThreadID, "PostProcessing");

}

UINT CalculateAllFluxes(LPVOID pParam){
	CPostProcessing post;
	CString userMessage, processingOutputFile, setupOutputFile;
	CString confCopyDir, serial;
	Common common;
	common.GetExePath();
	FileHandler::CProcessingFileReader writer;
	unsigned int k;

	// Set the directory where we're working in...
	post.m_exePath.Format(common.m_exePath);

	// set the directory to which we want to copy the settings
	confCopyDir.Format("%s\\copiedConfiguration\\", g_userSettings.m_outputDirectory);

	// make sure that the output directory exists
	if(CreateDirectoryStructure(g_userSettings.m_outputDirectory)){
		userMessage.Format("Could not create output directory: %s", g_userSettings.m_outputDirectory);
		ShowMessage(userMessage);
		ShowMessage("-- Exit post processing --");
		return 1;
	}
	if(CreateDirectoryStructure(confCopyDir)){
		userMessage.Format("Could not create directory for copied configuration: %s", confCopyDir);
		ShowMessage(userMessage);
		ShowMessage("-- Exit post processing --");
		return 1;
	}
	// we want to copy the setup and processing files to the confCopyDir
	processingOutputFile.Format("%s\\processing.xml", confCopyDir);
	setupOutputFile.Format("%s\\setup.xml",	confCopyDir);

	// check if the setup-file and the processing file already exists...
	if(!g_userSettings.m_startNow){
		if(IsExistingFile(processingOutputFile) && IsExistingFile(setupOutputFile)){
			FileHandler::CProcessingFileReader oldProcessingFileReader;
			Configuration::CUserConfiguration oldSettings;

			oldProcessingFileReader.ReadProcessingFile(processingOutputFile, oldSettings);
			if(oldSettings == g_userSettings){
				if(Common::AreIdenticalFiles(setupOutputFile, _T(common.m_exePath + "configuration\\setup.xml"))){
					int userAnswer = MessageBox(NULL, "Found old processing files in output directory. Is this an continuation of the old run?", "Continue?", MB_YESNO);
					if(IDYES == userAnswer){
						userAnswer = MessageBox(NULL, "Ok. This means that spectrum-files which have already been evaluated will not be evaluated again. Proceed?", "Continue?", MB_YESNO);
						if(IDYES == userAnswer){
							g_userSettings.m_fIsContinuation = true;
						}else{
							ShowMessage("-- Exit post processing --");
							return 1;
						}
					}
				}
			}
		}
	}else{
		Common::ArchiveFile(setupOutputFile);
		Common::ArchiveFile(processingOutputFile);
	}

	// Copy the settings that we have read in from the 'configuration' directory
	//	to the output directory to make it easier for the user to remember 
	//	what has been done...
	writer.WriteProcessingFile(processingOutputFile, g_userSettings);
	CopyFile(common.m_exePath + "configuration\\setup.xml", setupOutputFile, FALSE);
	for(k = 0; k < g_setup.m_instrumentNum; ++k){
		serial.Format(g_setup.m_instrument[k].m_serial);

		CopyFile(common.m_exePath + "configuration\\" + serial + ".exml", confCopyDir + serial + ".exml", FALSE);
	}

	// Do the post-processing
	if(g_userSettings.m_processingMode == PROCESSING_MODE_COMPOSITION){
		ShowMessage("Warning: Post processing of composition measurements is not yet fully implemented");
		post.DoPostProcessing_Flux(); // this uses the same code as the flux processing
	}else if(g_userSettings.m_processingMode == PROCESSING_MODE_STRATOSPHERE){
		ShowMessage("Warning: Post processing of stratospheric measurements is not yet fully implemented");
		post.DoPostProcessing_Strat();
	}else if(g_userSettings.m_processingMode == PROCESSING_MODE_TROPOSPHERE){
		ShowMessage("Post processing of tropospheric measurements is not yet implemented");
	}else if(g_userSettings.m_processingMode == PROCESSING_MODE_GEOMETRY){
		post.DoPostProcessing_Geometry();
	}else if(g_userSettings.m_processingMode == PROCESSING_MODE_DUALBEAM){
			
	}else{
		post.DoPostProcessing_Flux();
	}

	// Tell the user that we're done
	ShowMessage("-- Exit post processing --");

	// If we were to start the processing automatically then also quit it automatically
	if(g_userSettings.m_startNow){
        // exit the application
        ASSERT(AfxGetApp()->m_pMainWnd != NULL);
        AfxGetApp()->m_pMainWnd->SendMessage(WM_CLOSE);
	}
	
	return 0;
}


// Updating the interface...
void CNovacPPPDlg::OnUpdateViewConfigurationForSelectedVolcano(CCmdUI *pCmdUI)
{
	this->UpdateData(TRUE);
	
	if(m_comboAllVolcanoes.GetCurSel() == -1)
		pCmdUI->Enable(FALSE);
	else
		pCmdUI->Enable(TRUE);
}

void CNovacPPPDlg::OnChangeSelectedVolcano()
{
}

void CNovacPPPDlg::OnBnClickedCheckLocal()
{
	UpdateData(TRUE);
	EnableControls();
}

void CNovacPPPDlg::OnBnClickedCheckFtp()
{
	UpdateData(TRUE);
	EnableControls();
}

void CNovacPPPDlg::EnableControls(){
	if(m_CheckFtp.GetCheck()){
		m_editFTPDir.EnableWindow(TRUE);
		m_editFTPpwd.EnableWindow(TRUE);
		m_editFTPuser.EnableWindow(TRUE);
		m_CheckIncludeSubDirsFTP.EnableWindow(TRUE);
	}else{
		m_editFTPDir.EnableWindow(FALSE);
		m_editFTPpwd.EnableWindow(FALSE);
		m_editFTPuser.EnableWindow(FALSE);
		m_CheckIncludeSubDirsFTP.EnableWindow(FALSE);
	}

	if(m_CheckLocalDirectory.GetCheck()){
		m_editLocalDir.EnableWindow(TRUE);
		m_CheckIncludeSubDirsLocal.EnableWindow(TRUE);
	}else{
		m_editLocalDir.EnableWindow(FALSE);
		m_CheckIncludeSubDirsLocal.EnableWindow(FALSE);
	}
}


void CNovacPPPDlg::ParseCommandLineOptions(){
	char seps[]   = " \t";
	char *buffer = new char[16384];
	CString parameter;
	CString errorMessage;

	// a local copy of the command line parameters
	char *szLine = new char[16384];
	sprintf(szLine, "%s", this->m_commandLine);

	// Go through the received input parameters and set the approprate option
	CStringTokenizer *tokenizer = new CStringTokenizer(szLine, seps);
	const char *token = tokenizer->NextToken();

	while(NULL != token)
	{
	
		// The first date which we should analyze data from
		if(Equals(token, FLAG(str_fromDate), strlen(FLAG(str_fromDate)))){
			parameter.Format(token + strlen(FLAG(str_fromDate)));
			if(!CDateTime::ParseDate(parameter, g_userSettings.m_fromDate)){
				errorMessage.Format("Could not parse date: %s", parameter);
				ShowMessage(errorMessage);
			}
			token = tokenizer->NextToken();
			continue;
		}

		// The last date which we should analyze data from
		if(Equals(token, FLAG(str_toDate), strlen(FLAG(str_toDate)))){
			parameter.Format(token + strlen(FLAG(str_toDate)));
			if(!CDateTime::ParseDate(parameter, g_userSettings.m_toDate)){
				errorMessage.Format("Could not parse date: %s", parameter);
				ShowMessage(errorMessage);
			}
			token = tokenizer->NextToken();
			continue;
		}

		// the volcano to process data from 
		if(Equals(token, FLAG(str_volcano), strlen(FLAG(str_volcano)))){
			parameter.Format(token + strlen(FLAG(str_volcano)));
			int volcano = g_volcanoes.GetVolcanoIndex(parameter);
			if(volcano >= 0){
				g_userSettings.m_volcano = volcano;
			}else{
				errorMessage.Format("Could not find volcano: %s", parameter);
				ShowMessage(errorMessage);
			}
			token = tokenizer->NextToken();
			continue;
		}

		// the maximum number of threads
		if(Equals(token, FLAG(str_maxThreadNum), strlen(FLAG(str_maxThreadNum)))){
			sscanf(token + strlen(FLAG(str_maxThreadNum)), "%d", &g_userSettings.m_maxThreadNum);
			g_userSettings.m_maxThreadNum = max(g_userSettings.m_maxThreadNum, 1);
			token = tokenizer->NextToken();
			continue;
		}

		// if we should start the program immediately
		if(Equals(token, FLAG(str_startNow), strlen(FLAG(str_startNow)))){
			sscanf(token + strlen(FLAG(str_startNow)), "%d", &g_userSettings.m_startNow);
			token = tokenizer->NextToken();
			continue;
		}else if(Equals(token, "--" + CString(str_startNow), strlen("--" + CString(str_startNow)))){
			g_userSettings.m_startNow = 1;
			token = tokenizer->NextToken();
			continue;
		}

		// The options for the local directory
		if(Equals(token, FLAG(str_includeSubDirectories_Local), strlen(FLAG(str_includeSubDirectories_Local)))){
			sscanf(token + strlen(FLAG(str_includeSubDirectories_Local)), "%d", &g_userSettings.m_includeSubDirectories_Local);
			token = tokenizer->NextToken();
			continue;
		}
		if(Equals(token, FLAG(str_LocalDirectory), strlen(FLAG(str_LocalDirectory)))){
			if(sscanf(token + strlen(FLAG(str_LocalDirectory)), "%s", buffer)){
				g_userSettings.m_LocalDirectory.Format("%s", buffer);
				m_CheckLocalDirectory.SetCheck(1);
			}else{
				g_userSettings.m_LocalDirectory.Format("");
				m_CheckLocalDirectory.SetCheck(0);
			}
			token = tokenizer->NextToken();
			continue;
		}

		// The options for the FTP directory
		if(Equals(token, FLAG(str_includeSubDirectories_FTP), strlen(FLAG(str_includeSubDirectories_FTP)))){
			sscanf(token + strlen(FLAG(str_includeSubDirectories_FTP)), "%d", &g_userSettings.m_includeSubDirectories_FTP);
			token = tokenizer->NextToken();
			continue;
		}
		if(Equals(token, FLAG(str_FTPDirectory), strlen(FLAG(str_FTPDirectory)))){
			if(sscanf(token + strlen(FLAG(str_FTPDirectory)), "%s", buffer)){
				g_userSettings.m_FTPDirectory.Format("%s", buffer);
				m_CheckFtp.SetCheck(1);
			}else{
				g_userSettings.m_FTPDirectory.Format("");
				m_CheckFtp.SetCheck(0);
			}
			token = tokenizer->NextToken();
			continue;
		}	
		if(Equals(token, FLAG(str_FTPUsername), strlen(FLAG(str_FTPUsername)))){
			if(sscanf(token + strlen(FLAG(str_FTPUsername)), "%s", buffer)){
				g_userSettings.m_FTPUsername.Format("%s", buffer);
			}
			token = tokenizer->NextToken();
			continue;
		}
		if(Equals(token, FLAG(str_FTPPassword), strlen(FLAG(str_FTPPassword)))){
			if(sscanf(token + strlen(FLAG(str_FTPPassword)), "%s", buffer)){
				g_userSettings.m_FTPPassword.Format("%s", buffer);
			}
			token = tokenizer->NextToken();
			continue;
		}

		// If we should upload the results to the NovacFTP server at the end...
		if(Equals(token, FLAG(str_uploadResults), strlen(FLAG(str_uploadResults)))){
			sscanf(token + strlen(FLAG(str_uploadResults)), "%d", &g_userSettings.m_uploadResults);
			token = tokenizer->NextToken();
			continue;
		}

		// The output directory
		if(Equals(token, FLAG(str_outputDirectory), strlen(FLAG(str_outputDirectory)))){
			if(sscanf(token + strlen(FLAG(str_outputDirectory)), "%[^/*?<>|]", buffer)){
				g_userSettings.m_outputDirectory.Format("%s", buffer);
				// make sure that this ends with a trailing '\'
				if(g_userSettings.m_outputDirectory.GetAt(g_userSettings.m_outputDirectory.GetLength() - 1) != '\\'){
					g_userSettings.m_outputDirectory.AppendFormat("\\");
				}
			}
			token = tokenizer->NextToken();
			continue;
		}
		
		// The temporary directory
		if(Equals(token, FLAG(str_tempDirectory), strlen(FLAG(str_tempDirectory)))){
			if(sscanf(token + strlen(FLAG(str_tempDirectory)), "%[^/*?<>]", buffer)){
				g_userSettings.m_tempDirectory.Format("%s", buffer);
				// make sure that this ends with a trailing '\'
				if(g_userSettings.m_tempDirectory.GetAt(g_userSettings.m_tempDirectory.GetLength() - 1) != '\\'){
					g_userSettings.m_tempDirectory.AppendFormat("\\");
				}
			}
			token = tokenizer->NextToken();
			continue;
		}
		
		// The windField file
		int N = (int)strlen(FLAG(str_windFieldFile));
		if(Equals(token, FLAG(str_windFieldFile), N)){
			if(sscanf(token + N, "%s", buffer)){
				g_userSettings.m_windFieldFile.Format("%s", buffer);
			}
			token = tokenizer->NextToken();
			continue;
		}

		// The processing mode
		if(Equals(token, FLAG(str_processingMode), strlen(FLAG(str_processingMode)))){
			sscanf(token + strlen(FLAG(str_processingMode)), "%d", &g_userSettings.m_processingMode);
			token = tokenizer->NextToken();
			continue;
		}
		
		// the molecule
		if(Equals(token, FLAG(str_molecule), strlen(FLAG(str_molecule)))){
			if(sscanf(token + strlen(FLAG(str_molecule)), "%s", buffer)){
				if(Equals(buffer, "BrO")){
					g_userSettings.m_molecule = MOLEC_BRO;
				}else if(Equals(buffer, "NO2")){
					g_userSettings.m_molecule = MOLEC_NO2;
				}else if(Equals(buffer, "O3")){
					g_userSettings.m_molecule = MOLEC_O3;
				}else{
					g_userSettings.m_molecule = MOLEC_SO2;
				}
			}
			token = tokenizer->NextToken();
			continue;
		}
		
		// get the next token
		token = tokenizer->NextToken();
	}

	delete tokenizer;
}
void CNovacPPPDlg::OnClose()
{
	CDialog::OnClose();
}

/** Updates teh values in the interface with the ones found in 
	the configuration or from the command-line */
void CNovacPPPDlg::UpdateInterface(){

	// Should we search for files on the local computer(?)
	if(g_userSettings.m_LocalDirectory.GetLength() > 3)
		m_CheckLocalDirectory.SetCheck(1);
	else
		m_CheckLocalDirectory.SetCheck(0);

	if(g_userSettings.m_includeSubDirectories_Local)
		m_CheckIncludeSubDirsLocal.SetCheck(1);
	else
		m_CheckIncludeSubDirsLocal.SetCheck(0);
		
	if(g_userSettings.m_FTPDirectory.GetLength() > 3)
		m_CheckFtp.SetCheck(1);
	else
		m_CheckFtp.SetCheck(0);

	if(g_userSettings.m_includeSubDirectories_FTP)
		m_CheckIncludeSubDirsFTP.SetCheck(1);
	else
		m_CheckIncludeSubDirsFTP.SetCheck(0);

	// update the local copies of the date (these are either from the 
	//	processing.xml-file or from the command line, here it doesn't matter what)
	this->m_FromDay		= g_userSettings.m_fromDate.day;
	this->m_FromMonth	= g_userSettings.m_fromDate.month;
	this->m_FromYear	= g_userSettings.m_fromDate.year;
	this->m_ToDay		= g_userSettings.m_toDate.day;
	this->m_ToMonth		= g_userSettings.m_toDate.month;
	this->m_ToYear		= g_userSettings.m_toDate.year;
}
