#include "stdafx.h"
#include <PPPLib/CString.h>
#include <PPPLib/CStringTokenizer.h>

#include "VolcanoInfo.h"
#include "Common/Common.h"
#include "SetupFileReader.h"
#include "Configuration/NovacPPPConfiguration.h"
#include "Configuration/UserConfiguration.h"
#include "Common/EvaluationConfigurationParser.h"
#include "Common/ProcessingFileReader.h"

#include <iostream>

extern Configuration::CNovacPPPConfiguration        g_setup;	   // <-- The settings
extern Configuration::CUserConfiguration			g_userSettings;// <-- The settings of the user
extern CVolcanoInfo									g_volcanoes;   // <-- A list of all known volcanoes


void LoadConfigurations(int argc, char* argv[]);
void OnBnClickedCalculateFluxes();
UINT CalculateAllFluxes(void* pParam);
void ParseCommandLineOptions(int argc, char* argv[]);


// Main entry point for the application!
int main(int argc, char* argv[])
{
	try
	{
		LoadConfigurations(argc, argv);

		OnBnClickedCalculateFluxes();
	}
	catch (std::exception& e)
	{
		std::cout << e.what() << std::endl;
		return 1;
	}

	return 0;
}

void LoadConfigurations(int argc, char* argv[])
{
	// Declaration of variables and objects
	Common common;
	novac::CString setupPath, processingPath, evalConfPath, message, volcanoName;
	FileHandler::CSetupFileReader reader;
	FileHandler::CEvaluationConfigurationParser eval_reader;
	FileHandler::CProcessingFileReader processing_reader;

	//Read configuration from file setup.xml */	
	common.GetExePath();
	setupPath.Format("%s\\configuration\\setup.xml", common.m_exePath);
	if (SUCCESS != reader.ReadSetupFile(setupPath, g_setup))
	{
		throw std::exception("Could not read setup.xml. Setup not complete. Please fix and try again");
	}

	// Read the users options from file processing.xml
	processingPath.Format("%s\\configuration\\processing.xml", common.m_exePath);
	if (SUCCESS != processing_reader.ReadProcessingFile(processingPath, g_userSettings)) {
		throw std::exception("Could not read processing.xml. Setup not complete. Please fix and try again");
	}

	// Check if there is a configuration file for every spectrometer serial number
	bool failed = false;
	for (int k = 0; k < g_setup.m_instrumentNum; ++k) {
		evalConfPath.Format("%s\\configuration\\%s.exml", common.m_exePath, g_setup.m_instrument[k].m_serial);

		if (IsExistingFile(evalConfPath))
			eval_reader.ReadConfigurationFile(evalConfPath, &g_setup.m_instrument[k].m_eval, &g_setup.m_instrument[k].m_darkCurrentCorrection);
		else {
			throw std::exception("Could not find configuration file: " +  evalConfPath);
		}
	}

	// Get the options from the command line
	ParseCommandLineOptions(argc, argv);

	// If we are supposed to start the processing automatically, then let's do so
	if (g_userSettings.m_startNow) {
		OnBnClickedCalculateFluxes();
	}
}


void OnBnClickedCalculateFluxes()
{
	// 1. check input!!!
	int curSel = m_comboAllVolcanoes.GetCurSel();
	if (curSel < 0) {
		throw std::exception("You have to choose a volcano!!!");
	}

	if (m_CheckLocalDirectory.GetCheck() != BST_CHECKED) {
		g_userSettings.m_LocalDirectory.Format("");
	}

	if (m_CheckFtp.GetCheck() != BST_CHECKED) {
		g_userSettings.m_FTPDirectory.Format("");
	}

	if (m_CheckDatabase.GetCheck() == BST_CHECKED) {
		throw std::exception("Can not yet search for files on Database server. Sorry!!");
	}

	// 2. check the data

	// 2a. make sure that the ftp-path ends with a '/'
	if (g_userSettings.m_FTPDirectory.GetLength() > 1) {
		if (!Equals(g_userSettings.m_FTPDirectory.Right(1), "/")) {
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
	g_userSettings.m_volcano = this->m_comboAllVolcanoes.GetCurSel();
	g_userSettings.m_fromDate = CDateTime(this->m_FromYear, this->m_FromMonth, this->m_FromDay, 00, 00, 00);
	g_userSettings.m_toDate = CDateTime(this->m_ToYear, this->m_ToMonth, this->m_ToDay, 23, 59, 59);

	// 5. Run
	CWinThread *postProcessingthread = AfxBeginThread(CalculateAllFluxes, NULL, THREAD_PRIORITY_NORMAL, 0, 0, NULL);
	Common::SetThreadName(postProcessingthread->m_nThreadID, "PostProcessing");
}


UINT CalculateAllFluxes(void* pParam) {
	CPostProcessing post;
	novac::CString userMessage, processingOutputFile, setupOutputFile;
	novac::CString confCopyDir, serial;
	Common common;
	common.GetExePath();
	FileHandler::CProcessingFileReader writer;
	unsigned int k;

	// Set the directory where we're working in...
	post.m_exePath.Format(common.m_exePath);

	// set the directory to which we want to copy the settings
	confCopyDir.Format("%s\\copiedConfiguration\\", g_userSettings.m_outputDirectory);

	// make sure that the output directory exists
	if (CreateDirectoryStructure(g_userSettings.m_outputDirectory)) {
		userMessage.Format("Could not create output directory: %s", g_userSettings.m_outputDirectory);
		ShowMessage(userMessage);
		ShowMessage("-- Exit post processing --");
		return 1;
	}
	if (CreateDirectoryStructure(confCopyDir)) {
		userMessage.Format("Could not create directory for copied configuration: %s", confCopyDir);
		ShowMessage(userMessage);
		ShowMessage("-- Exit post processing --");
		return 1;
	}
	// we want to copy the setup and processing files to the confCopyDir
	processingOutputFile.Format("%s\\processing.xml", confCopyDir);
	setupOutputFile.Format("%s\\setup.xml", confCopyDir);

	// check if the setup-file and the processing file already exists...
	if (!g_userSettings.m_startNow) {
		if (IsExistingFile(processingOutputFile) && IsExistingFile(setupOutputFile)) {
			FileHandler::CProcessingFileReader oldProcessingFileReader;
			Configuration::CUserConfiguration oldSettings;

			oldProcessingFileReader.ReadProcessingFile(processingOutputFile, oldSettings);
			if (oldSettings == g_userSettings) {
				if (Common::AreIdenticalFiles(setupOutputFile, _T(common.m_exePath + "configuration\\setup.xml"))) {
					int userAnswer = MessageBox(NULL, "Found old processing files in output directory. Is this an continuation of the old run?", "Continue?", MB_YESNO);
					if (IDYES == userAnswer) {
						userAnswer = MessageBox(NULL, "Ok. This means that spectrum-files which have already been evaluated will not be evaluated again. Proceed?", "Continue?", MB_YESNO);
						if (IDYES == userAnswer) {
							g_userSettings.m_fIsContinuation = true;
						}
						else {
							ShowMessage("-- Exit post processing --");
							return 1;
						}
					}
				}
			}
		}
	}
	else {
		Common::ArchiveFile(setupOutputFile);
		Common::ArchiveFile(processingOutputFile);
	}

	// Copy the settings that we have read in from the 'configuration' directory
	//	to the output directory to make it easier for the user to remember 
	//	what has been done...
	writer.WriteProcessingFile(processingOutputFile, g_userSettings);
	CopyFile(common.m_exePath + "configuration\\setup.xml", setupOutputFile, FALSE);
	for (k = 0; k < g_setup.m_instrumentNum; ++k) {
		serial.Format(g_setup.m_instrument[k].m_serial);

		CopyFile(common.m_exePath + "configuration\\" + serial + ".exml", confCopyDir + serial + ".exml", FALSE);
	}

	// Do the post-processing
	if (g_userSettings.m_processingMode == PROCESSING_MODE_COMPOSITION) {
		ShowMessage("Warning: Post processing of composition measurements is not yet fully implemented");
		post.DoPostProcessing_Flux(); // this uses the same code as the flux processing
	}
	else if (g_userSettings.m_processingMode == PROCESSING_MODE_STRATOSPHERE) {
		ShowMessage("Warning: Post processing of stratospheric measurements is not yet fully implemented");
		post.DoPostProcessing_Strat();
	}
	else if (g_userSettings.m_processingMode == PROCESSING_MODE_TROPOSPHERE) {
		ShowMessage("Post processing of tropospheric measurements is not yet implemented");
	}
	else if (g_userSettings.m_processingMode == PROCESSING_MODE_GEOMETRY) {
		post.DoPostProcessing_Geometry();
	}
	else if (g_userSettings.m_processingMode == PROCESSING_MODE_DUALBEAM) {

	}
	else {
		post.DoPostProcessing_Flux();
	}

	// Tell the user that we're done
	ShowMessage("-- Exit post processing --");

	// If we were to start the processing automatically then also quit it automatically
	if (g_userSettings.m_startNow) {
		// exit the application
		ASSERT(AfxGetApp()->m_pMainWnd != NULL);
		AfxGetApp()->m_pMainWnd->SendMessage(WM_CLOSE);
	}

	return 0;
}


void ParseCommandLineOptions(int argc, char* argv[])
{
	char seps[] = " \t";
	char *buffer = new char[16384];
	novac::CString parameter;
	novac::CString errorMessage;

	// a local copy of the command line parameters
	char *szLine = new char[16384];
	sprintf(szLine, "%s", this->m_commandLine);

	// Go through the received input parameters and set the approprate option
	novac::CStringTokenizer tokenizer(szLine, seps);
	const char *token = tokenizer.NextToken();

	while (NULL != token)
	{

		// The first date which we should analyze data from
		if (Equals(token, FLAG(str_fromDate), strlen(FLAG(str_fromDate)))) {
			parameter.Format(token + strlen(FLAG(str_fromDate)));
			if (!CDateTime::ParseDate(parameter, g_userSettings.m_fromDate)) {
				errorMessage.Format("Could not parse date: %s", parameter);
				ShowMessage(errorMessage);
			}
			token = tokenizer.NextToken();
			continue;
		}

		// The last date which we should analyze data from
		if (Equals(token, FLAG(str_toDate), strlen(FLAG(str_toDate)))) {
			parameter.Format(token + strlen(FLAG(str_toDate)));
			if (!CDateTime::ParseDate(parameter, g_userSettings.m_toDate)) {
				errorMessage.Format("Could not parse date: %s", parameter);
				ShowMessage(errorMessage);
			}
			token = tokenizer.NextToken();
			continue;
		}

		// the volcano to process data from 
		if (Equals(token, FLAG(str_volcano), strlen(FLAG(str_volcano)))) {
			parameter.Format(token + strlen(FLAG(str_volcano)));
			int volcano = g_volcanoes.GetVolcanoIndex(parameter);
			if (volcano >= 0) {
				g_userSettings.m_volcano = volcano;
			}
			else {
				errorMessage.Format("Could not find volcano: %s", parameter);
				ShowMessage(errorMessage);
			}
			token = tokenizer.NextToken();
			continue;
		}

		// the maximum number of threads
		if (Equals(token, FLAG(str_maxThreadNum), strlen(FLAG(str_maxThreadNum)))) {
			sscanf(token + strlen(FLAG(str_maxThreadNum)), "%d", &g_userSettings.m_maxThreadNum);
			g_userSettings.m_maxThreadNum = max(g_userSettings.m_maxThreadNum, 1);
			token = tokenizer.NextToken();
			continue;
		}

		// if we should start the program immediately
		if (Equals(token, FLAG(str_startNow), strlen(FLAG(str_startNow)))) {
			sscanf(token + strlen(FLAG(str_startNow)), "%d", &g_userSettings.m_startNow);
			token = tokenizer.NextToken();
			continue;
		}
		else if (Equals(token, "--" + novac::CString(str_startNow), strlen("--" + novac::CString(str_startNow)))) {
			g_userSettings.m_startNow = 1;
			token = tokenizer.NextToken();
			continue;
		}

		// The options for the local directory
		if (Equals(token, FLAG(str_includeSubDirectories_Local), strlen(FLAG(str_includeSubDirectories_Local)))) {
			sscanf(token + strlen(FLAG(str_includeSubDirectories_Local)), "%d", &g_userSettings.m_includeSubDirectories_Local);
			token = tokenizer.NextToken();
			continue;
		}
		if (Equals(token, FLAG(str_LocalDirectory), strlen(FLAG(str_LocalDirectory)))) {
			if (sscanf(token + strlen(FLAG(str_LocalDirectory)), "%s", buffer)) {
				g_userSettings.m_LocalDirectory.Format("%s", buffer);
				m_CheckLocalDirectory.SetCheck(1);
			}
			else {
				g_userSettings.m_LocalDirectory.Format("");
				m_CheckLocalDirectory.SetCheck(0);
			}
			token = tokenizer.NextToken();
			continue;
		}

		// The options for the FTP directory
		if (Equals(token, FLAG(str_includeSubDirectories_FTP), strlen(FLAG(str_includeSubDirectories_FTP)))) {
			sscanf(token + strlen(FLAG(str_includeSubDirectories_FTP)), "%d", &g_userSettings.m_includeSubDirectories_FTP);
			token = tokenizer.NextToken();
			continue;
		}
		if (Equals(token, FLAG(str_FTPDirectory), strlen(FLAG(str_FTPDirectory)))) {
			if (sscanf(token + strlen(FLAG(str_FTPDirectory)), "%s", buffer)) {
				g_userSettings.m_FTPDirectory.Format("%s", buffer);
				m_CheckFtp.SetCheck(1);
			}
			else {
				g_userSettings.m_FTPDirectory.Format("");
				m_CheckFtp.SetCheck(0);
			}
			token = tokenizer.NextToken();
			continue;
		}
		if (Equals(token, FLAG(str_FTPUsername), strlen(FLAG(str_FTPUsername)))) {
			if (sscanf(token + strlen(FLAG(str_FTPUsername)), "%s", buffer)) {
				g_userSettings.m_FTPUsername.Format("%s", buffer);
			}
			token = tokenizer.NextToken();
			continue;
		}
		if (Equals(token, FLAG(str_FTPPassword), strlen(FLAG(str_FTPPassword)))) {
			if (sscanf(token + strlen(FLAG(str_FTPPassword)), "%s", buffer)) {
				g_userSettings.m_FTPPassword.Format("%s", buffer);
			}
			token = tokenizer.NextToken();
			continue;
		}

		// If we should upload the results to the NovacFTP server at the end...
		if (Equals(token, FLAG(str_uploadResults), strlen(FLAG(str_uploadResults)))) {
			sscanf(token + strlen(FLAG(str_uploadResults)), "%d", &g_userSettings.m_uploadResults);
			token = tokenizer.NextToken();
			continue;
		}

		// The output directory
		if (Equals(token, FLAG(str_outputDirectory), strlen(FLAG(str_outputDirectory)))) {
			if (sscanf(token + strlen(FLAG(str_outputDirectory)), "%[^/*?<>|]", buffer)) {
				g_userSettings.m_outputDirectory.Format("%s", buffer);
				// make sure that this ends with a trailing '\'
				if (g_userSettings.m_outputDirectory.GetAt(g_userSettings.m_outputDirectory.GetLength() - 1) != '\\') {
					g_userSettings.m_outputDirectory.AppendFormat("\\");
				}
			}
			token = tokenizer.NextToken();
			continue;
		}

		// The temporary directory
		if (Equals(token, FLAG(str_tempDirectory), strlen(FLAG(str_tempDirectory)))) {
			if (sscanf(token + strlen(FLAG(str_tempDirectory)), "%[^/*?<>]", buffer)) {
				g_userSettings.m_tempDirectory.Format("%s", buffer);
				// make sure that this ends with a trailing '\'
				if (g_userSettings.m_tempDirectory.GetAt(g_userSettings.m_tempDirectory.GetLength() - 1) != '\\') {
					g_userSettings.m_tempDirectory.AppendFormat("\\");
				}
			}
			token = tokenizer.NextToken();
			continue;
		}

		// The windField file
		int N = (int)strlen(FLAG(str_windFieldFile));
		if (Equals(token, FLAG(str_windFieldFile), N)) {
			if (sscanf(token + N, "%s", buffer)) {
				g_userSettings.m_windFieldFile.Format("%s", buffer);
			}
			token = tokenizer.NextToken();
			continue;
		}

		// The processing mode
		if (Equals(token, FLAG(str_processingMode), strlen(FLAG(str_processingMode)))) {
			sscanf(token + strlen(FLAG(str_processingMode)), "%d", &g_userSettings.m_processingMode);
			token = tokenizer.NextToken();
			continue;
		}

		// the molecule
		if (Equals(token, FLAG(str_molecule), strlen(FLAG(str_molecule)))) {
			if (sscanf(token + strlen(FLAG(str_molecule)), "%s", buffer)) {
				if (Equals(buffer, "BrO")) {
					g_userSettings.m_molecule = MOLEC_BRO;
				}
				else if (Equals(buffer, "NO2")) {
					g_userSettings.m_molecule = MOLEC_NO2;
				}
				else if (Equals(buffer, "O3")) {
					g_userSettings.m_molecule = MOLEC_O3;
				}
				else {
					g_userSettings.m_molecule = MOLEC_SO2;
				}
			}
			token = tokenizer.NextToken();
			continue;
		}

		// get the next token
		token = tokenizer.NextToken();
	}
}