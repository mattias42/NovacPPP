#include "stdafx.h"
#include <PPPLib/CString.h>
#include <PPPLib/CStringTokenizer.h>
#include <PPPLib/VolcanoInfo.h>

#include "Common/Common.h"
#include "SetupFileReader.h"
#include "Configuration/NovacPPPConfiguration.h"
#include "Configuration/UserConfiguration.h"
#include "Common/EvaluationConfigurationParser.h"
#include "Common/ProcessingFileReader.h"
#include "PostProcessing.h"

#include <iostream>
#include <algorithm>
#include <thread>
#include <Poco/Path.h>
#include <Poco/Logger.h>
#include <Poco/FileChannel.h>
#include <Poco/SplitterChannel.h>
#include <Poco/ConsoleChannel.h>
#include <Poco/Util/Application.h>

extern Configuration::CNovacPPPConfiguration        g_setup;	   // <-- The settings
extern Configuration::CUserConfiguration			g_userSettings;// <-- The settings of the user

novac::CVolcanoInfo g_volcanoes;   // <-- A list of all known volcanoes

std::string s_exePath;
std::string s_exeFileName;

#undef min
#undef max

// void LoadConfigurations(int argc, char* argv[]);
void LoadConfigurations();
void OnBnClickedCalculateFluxes(int selectedVolcano = 0);
void CalculateAllFluxes();
void ParseCommandLineOptions(const std::vector<std::string>& arguments);


class NovacPPPApplication : public Poco::Util::Application
{
protected:
    void initialize(Poco::Util::Application& application){
        //this->config().setString("optionval", "defaultoption");
        this->loadConfiguration();
        Poco::Util::Application::initialize(application);
    }

    void uninitialize(){
        Poco::Util::Application::uninitialize();
    }

    void defineOptions(Poco::Util::OptionSet& optionSet) {
        Poco::Util::Application::defineOptions(optionSet);

        /* optionSet.addOption(
                Poco::Util::Option("optionval", "", "Some value")
                        .required(false)
                        .repeatable(true)
                        .argument("<the value>", true)
                        .callback(Poco::Util::OptionCallback<OptionExample>(this, &OptionExample::handleMyOpt))
        ); */
    }

    void handleMyOpt(const std::string &name, const std::string &value) {
        std::cout << "Setting option " << name << " to " << value << std::endl;
        this->config().setString(name, value);
        std::cout << "The option is now " << this->config().getString(name) << std::endl;
    }

    int main(const std::vector<std::string> &arguments) {
		std::cout << "Novac Post Processing Program" << std::endl;

		try
		{
			auto& application = Poco::Util::Application::instance();
			Poco::Path executable(application.commandPath());

			s_exePath = executable.parent().toString();
			s_exeFileName = executable.getFileName();

			// Setup the logging
			Poco::AutoPtr<Poco::SplitterChannel> splitterChannel(new Poco::SplitterChannel());
			splitterChannel->addChannel(new Poco::ConsoleChannel());
			Poco::Logger::root().setChannel(new Poco::ConsoleChannel());
			Poco::Logger& log = Poco::Logger::get("NovacPPP");
			ShowMessage(novac::CString::FormatString(" Executing %s in '%s'", s_exeFileName.c_str(), s_exePath.c_str()));

			// Read the configuration files
			std::cout << " Loading configuration" << std::endl;
			LoadConfigurations();

			splitterChannel->addChannel(new Poco::FileChannel(g_userSettings.m_outputDirectory.std_str() + "StatusLog.txt"));
			log.setChannel(splitterChannel);

			// Get the options from the command line
			std::cout << " Getting command line arguments" << std::endl;
			ParseCommandLineOptions(arguments);

			// Start calculating the fluxes, this is the old button handler
			std::cout << " Setup done: starting calculations" << std::endl;
			OnBnClickedCalculateFluxes();
		}
		catch(Poco::FileNotFoundException& e)
		{
			std::cout << e.displayText() << std::endl;
			return 1;
		}
		catch (std::exception& e)
		{
			std::cout << e.what() << std::endl;
			return 1;
		}

		return 0;
	}
};

POCO_APP_MAIN(NovacPPPApplication)


void LoadConfigurations()
{
	// Declaration of variables and objects
	Common common;
	novac::CString setupPath, processingPath, evalConfPath, volcanoName;
	FileHandler::CSetupFileReader reader;
	FileHandler::CEvaluationConfigurationParser eval_reader;
	FileHandler::CProcessingFileReader processing_reader;

	//Read configuration from file setup.xml */	
	setupPath.Format("%sconfiguration%csetup.xml", (const char*)common.m_exePath, Poco::Path::separator());
	if (SUCCESS != reader.ReadSetupFile(setupPath, g_setup))
	{
		throw std::logic_error("Could not read setup.xml. Setup not complete. Please fix and try again");
	}
	ShowMessage(novac::CString::FormatString(" Parsed %s, %d instruments found. (", setupPath.c_str(), g_setup.m_instrumentNum));


	// Read the users options from file processing.xml
	processingPath.Format("%sconfiguration%cprocessing.xml", (const char*)common.m_exePath, Poco::Path::separator());
	if (SUCCESS != processing_reader.ReadProcessingFile(processingPath, g_userSettings)) {
		throw std::logic_error("Could not read processing.xml. Setup not complete. Please fix and try again");
	}

	// Check if there is a configuration file for every spectrometer serial number
	for (unsigned int k = 0; k < g_setup.m_instrumentNum; ++k) {
		evalConfPath.Format("%sconfiguration%c%s.exml", (const char*)common.m_exePath, Poco::Path::separator(), (const char*)g_setup.m_instrument[k].m_serial);

		if (IsExistingFile(evalConfPath))
			eval_reader.ReadConfigurationFile(evalConfPath, &g_setup.m_instrument[k].m_eval, &g_setup.m_instrument[k].m_darkCurrentCorrection);
		else {
			throw std::logic_error("Could not find configuration file: " + evalConfPath);
		}
	}
}


void OnBnClickedCalculateFluxes(int selectedVolcano)
{
	// 1. check input!!!

// 	if (m_CheckLocalDirectory.GetCheck() != BST_CHECKED) {
// 		g_userSettings.m_LocalDirectory.Format("");
// 	}

// 	if (m_CheckFtp.GetCheck() != BST_CHECKED) {
// 		g_userSettings.m_FTPDirectory.Format("");
// 	}

	// 2. check the data

	// 2a. make sure that the ftp-path ends with a '/'
	if (g_userSettings.m_FTPDirectory.GetLength() > 1) {
		if (!Equals(g_userSettings.m_FTPDirectory.Right(1), "/")) {
			g_userSettings.m_FTPDirectory.AppendFormat("/");
		}
	}


	// 3. Open the message window
	// pView = &m_statusDlg;
	// m_statusDlg.Create(IDD_MESSAGEDLG);
	// m_statusDlg.ShowWindow(SW_SHOW);
	// m_statusDlg.GetWindowRect(rect);
	// int w = rect.Width();
	// int h = rect.Height();
	// rect.left = 10; rect.right = rect.left + w;
	// rect.top = 10; rect.bottom = rect.top + h;
	// m_statusDlg.MoveWindow(rect); // move the window to the corner of the screen

	// 4. Set the parameters for the post-processing..
	g_userSettings.m_volcano = selectedVolcano; // this->m_comboAllVolcanoes.GetCurSel();

	// 5. Run
#ifdef _MFC_VER 
	CWinThread *postProcessingthread = AfxBeginThread(CalculateAllFluxes, NULL, THREAD_PRIORITY_NORMAL, 0, 0, NULL);
	Common::SetThreadName(postProcessingthread->m_nThreadID, "PostProcessing");
#else
	std::thread postProcessingThread(CalculateAllFluxes);
	postProcessingThread.join();
#endif  // _MFC_VER 
}


void CalculateAllFluxes() {
	try
	{
		CPostProcessing post;
		novac::CString userMessage, processingOutputFile, setupOutputFile;
		novac::CString confCopyDir, serial;
		Common common;
		FileHandler::CProcessingFileReader writer;
		unsigned int k;

		// Set the directory where we're working in...
		post.m_exePath.Format(common.m_exePath);

		// set the directory to which we want to copy the settings
		confCopyDir.Format("%s/copiedConfiguration/", (const char*)g_userSettings.m_outputDirectory);

		// make sure that the output directory exists
		if (CreateDirectoryStructure(g_userSettings.m_outputDirectory)) {
			userMessage.Format("Could not create output directory: %s", (const char*)g_userSettings.m_outputDirectory);
			ShowMessage(userMessage);
			ShowMessage("-- Exit post processing --");
			return;
		}
		if (CreateDirectoryStructure(confCopyDir)) {
			userMessage.Format("Could not create directory for copied configuration: %s", (const char*)confCopyDir);
			ShowMessage(userMessage);
			ShowMessage("-- Exit post processing --");
			return;
		}
		// we want to copy the setup and processing files to the confCopyDir
		processingOutputFile.Format("%s/processing.xml", (const char*)confCopyDir);
		setupOutputFile.Format("%s/setup.xml", (const char*)confCopyDir);

		Common::ArchiveFile(setupOutputFile);
		Common::ArchiveFile(processingOutputFile);

		// Copy the settings that we have read in from the 'configuration' directory
		//	to the output directory to make it easier for the user to remember 
		//	what has been done...
		writer.WriteProcessingFile(processingOutputFile, g_userSettings);

		Common::CopyFile(common.m_exePath + "configuration/setup.xml", setupOutputFile);
		for (k = 0; k < g_setup.m_instrumentNum; ++k) {
			serial.Format(g_setup.m_instrument[k].m_serial);

			Common::CopyFile(common.m_exePath + "configuration/" + serial + ".exml", confCopyDir + serial + ".exml");
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

		return;
	}
	catch(Poco::FileNotFoundException& e)
	{
		std::cout << e.displayText() << std::endl;
		return;
	}
	catch (std::exception& e)
	{
		std::cout << e.what() << std::endl;
		return;
	}
}


void ParseCommandLineOptions(const std::vector<std::string> &arguments)
{
	char seps[] = " \t";
	std::vector<char> buffer(16384, 0);
	novac::CString parameter;
	novac::CString errorMessage;

	// a local copy of the command line parameters
	novac::CString commandLine;
	for (size_t arg = 1; arg < arguments.size(); ++arg)
	{
		commandLine = commandLine + " " + novac::CString(arguments[arg]);
	}

	// Go through the received input parameters and set the approprate option
	novac::CStringTokenizer tokenizer(commandLine.c_str(), seps);
	const char *token = tokenizer.NextToken();

	while (nullptr != token)
	{

		// The first date which we should analyze data from
		if (Equals(token, FLAG(str_fromDate), strlen(FLAG(str_fromDate)))) {
			parameter.Format(token + strlen(FLAG(str_fromDate)));
			if (!novac::CDateTime::ParseDate(parameter, g_userSettings.m_fromDate)) {
				errorMessage.Format("Could not parse date: %s", (const char*)parameter);
				ShowMessage(errorMessage);
			}
			token = tokenizer.NextToken();
			continue;
		}

		// The last date which we should analyze data from
		if (Equals(token, FLAG(str_toDate), strlen(FLAG(str_toDate)))) {
			parameter.Format(token + strlen(FLAG(str_toDate)));
			if (!novac::CDateTime::ParseDate(parameter, g_userSettings.m_toDate)) {
				errorMessage.Format("Could not parse date: %s", (const char*)parameter);
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
				errorMessage.Format("Could not find volcano: %s", (const char*)parameter);
				ShowMessage(errorMessage);
			}
			token = tokenizer.NextToken();
			continue;
		}

		// the maximum number of threads
		if (Equals(token, FLAG(str_maxThreadNum), strlen(FLAG(str_maxThreadNum)))) {
			sscanf(token + strlen(FLAG(str_maxThreadNum)), "%ld", &g_userSettings.m_maxThreadNum);
			g_userSettings.m_maxThreadNum = std::max(g_userSettings.m_maxThreadNum, (unsigned long)1);
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
			if (sscanf(token + strlen(FLAG(str_LocalDirectory)), "%s", buffer.data())) {
				g_userSettings.m_LocalDirectory.Format("%s", buffer.data());
				// m_CheckLocalDirectory.SetCheck(1);
			}
			else {
				g_userSettings.m_LocalDirectory.Format("");
				// m_CheckLocalDirectory.SetCheck(0);
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
			if (sscanf(token + strlen(FLAG(str_FTPDirectory)), "%s", buffer.data())) {
				g_userSettings.m_FTPDirectory.Format("%s", buffer.data());
				// m_CheckFtp.SetCheck(1);
			}
			else {
				g_userSettings.m_FTPDirectory.Format("");
				// m_CheckFtp.SetCheck(0);
			}
			token = tokenizer.NextToken();
			continue;
		}
		if (Equals(token, FLAG(str_FTPUsername), strlen(FLAG(str_FTPUsername)))) {
			if (sscanf(token + strlen(FLAG(str_FTPUsername)), "%s", buffer.data())) {
				g_userSettings.m_FTPUsername.Format("%s", buffer.data());
			}
			token = tokenizer.NextToken();
			continue;
		}
		if (Equals(token, FLAG(str_FTPPassword), strlen(FLAG(str_FTPPassword)))) {
			if (sscanf(token + strlen(FLAG(str_FTPPassword)), "%s", buffer.data())) {
				g_userSettings.m_FTPPassword.Format("%s", buffer.data());
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
			if (sscanf(token + strlen(FLAG(str_outputDirectory)), "%[^/*?<>|]", buffer.data())) {
				g_userSettings.m_outputDirectory.Format("%s", buffer.data());
				// make sure that this ends with a trailing '\'
				if (g_userSettings.m_outputDirectory.GetAt(g_userSettings.m_outputDirectory.GetLength() - 1) != '/') {
					g_userSettings.m_outputDirectory.AppendFormat("/");
				}
			}
			token = tokenizer.NextToken();
			continue;
		}

		// The temporary directory
		if (Equals(token, FLAG(str_tempDirectory), strlen(FLAG(str_tempDirectory)))) {
			if (sscanf(token + strlen(FLAG(str_tempDirectory)), "%[^/*?<>]", buffer.data())) {
				g_userSettings.m_tempDirectory.Format("%s", buffer.data());
				// make sure that this ends with a trailing '\'
				if (g_userSettings.m_tempDirectory.GetAt(g_userSettings.m_tempDirectory.GetLength() - 1) != '/') {
					g_userSettings.m_tempDirectory.AppendFormat("/");
				}
			}
			token = tokenizer.NextToken();
			continue;
		}

		// The windField file
		int N = (int)strlen(FLAG(str_windFieldFile));
		if (Equals(token, FLAG(str_windFieldFile), N)) {
			if (sscanf(token + N, "%s", buffer.data())) {
				g_userSettings.m_windFieldFile.Format("%s", buffer.data());
			}
			token = tokenizer.NextToken();
			continue;
		}

		// The processing mode
		if (Equals(token, FLAG(str_processingMode), strlen(FLAG(str_processingMode)))) {
			sscanf(token + strlen(FLAG(str_processingMode)), "%d", (int*)&g_userSettings.m_processingMode);
			token = tokenizer.NextToken();
			continue;
		}

		// the molecule
		if (Equals(token, FLAG(str_molecule), strlen(FLAG(str_molecule)))) {
			if (sscanf(token + strlen(FLAG(str_molecule)), "%s", buffer.data())) {
				if (novac::Equals(buffer.data(), "BrO")) {
					g_userSettings.m_molecule = MOLEC_BRO;
				}
				else if (novac::Equals(buffer.data(), "NO2")) {
					g_userSettings.m_molecule = MOLEC_NO2;
				}
				else if (novac::Equals(buffer.data(), "O3")) {
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