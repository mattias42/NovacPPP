#include "PostEvaluationController.h"
#include "ScanEvaluation.h"

// This is the information we need to continue an old processing
#include "../ContinuationOfProcessing.h"

// This is the configuration of the network
#include "../Configuration/NovacPPPConfiguration.h"

// This is the settings for how to do the procesing
#include "../Configuration/UserConfiguration.h"

// ... support for handling the evaluation-log files...
#include "../Common/EvaluationLogFileHandler.h"

// we want to make some statistics on the processing
#include "../PostProcessingStatistics.h"

using namespace Evaluation;
using namespace SpectrumIO;
using namespace FileHandler;

extern Configuration::CNovacPPPConfiguration		g_setup;	   // <-- The settings
extern Configuration::CUserConfiguration			g_userSettings;// <-- The settings of the user
extern CPostProcessingStatistics					g_processingStats;	// <-- The statistics of the processing itself
extern CContinuationOfProcessing					g_continuation;		// <-- Information on what has already been done when continuing an old processing round

CPostEvaluationController::CPostEvaluationController(void)
{
	m_lastResult = nullptr;
}

CPostEvaluationController::~CPostEvaluationController(void)
{
}


/** This function takes care of the evaluation of one scan.	*/
int CPostEvaluationController::EvaluateScan(const novac::CString& pakFileName, const novac::CString &fitWindowName, novac::CString *txtFileName, CPlumeInScanProperty *plumeProperties) {
	clock_t cStart, cFinish;
	novac::CString errorMessage, message, serialNumber;
	Meteorology::CWindField windField;
	novac::CDateTime startTime;
	SpectrumIO::CSpectrumIO reader;
	CSpectrum skySpectrum;
	Configuration::CInstrumentLocation instrLocation;
	CFitWindow fitWindow;
	Configuration::CDarkSettings darkSettings;

	// The CScanFileHandler is a structure for reading the 
	//		spectral information from the scan-file
	CScanFileHandler scan;

	// clock the time it takes to treat one scan
	cStart = clock();

	/** ------------- The process to evaluate a scan --------------- */

	// ------------------ Read the scan file -----------------------
	// --- this to make sure that the spectra in the file are ok ---
	if (SUCCESS != scan.CheckScanFile(&pakFileName)) {
		errorMessage.Format("Could not read recieved pak-file %s. Will not evaulate.", (const char*)pakFileName);
		ShowMessage(errorMessage);
		return 2;
	}

	// ---------- Get the information we need about the instrument ----------

	//		Find the serial number of the spectrometer
//	reader.ReadSpectrum(pakFileName, 0, spec); // TODO: check for errors!!

	//		Find the information in the configuration about this instrument
	if (GetLocationAndFitWindow(&scan, fitWindowName, instrLocation, fitWindow)) {
		return 3;
	}
	//	the settings for how to correct for dark
	if (GetDarkCurrentSettings(&scan, darkSettings)) {
		return 3;
	}

	// Check if we have already evaluated this scan. Only if this is a re-run of
	//	an old processing...
	if (g_userSettings.m_fIsContinuation) {
		if (g_continuation.IsPreviouslyIgnored(pakFileName)) {
			errorMessage.Format(" Scan %s has already been evaluated and was ignored. Will proceed to the next scan", (const char*)pakFileName);
			ShowMessage(errorMessage);

			return 0;
		}
		else {
			novac::CString archivePakFileName, archiveTxtFileName;

			// loop through all possible measurement modes and see if the evaluation log file already exists
			MEASUREMENT_MODE modes[] = { MODE_FLUX, MODE_WINDSPEED, MODE_STRATOSPHERE, MODE_DIRECT_SUN,
										MODE_COMPOSITION, MODE_LUNAR, MODE_TROPOSPHERE, MODE_MAXDOAS };
			for (int k = 0; k < 8; ++k) {
				GetArchivingfileName(archivePakFileName, archiveTxtFileName, fitWindowName, pakFileName, modes[k]);
				if (IsExistingFile(archiveTxtFileName)) {
					errorMessage.Format(" Scan %s has already been evaluated. Will proceed to the next scan", (const char*)pakFileName);
					ShowMessage(errorMessage);

					txtFileName->Format(archiveTxtFileName);

					return 0;
				}
			}
		}
	}

	// ------------- Check that the measurement is good enough to evaluate -----------
	if (!IsGoodEnoughToEvaluate(&scan, fitWindow, instrLocation)) {
		return 5;
	}

	// 6. Evaluate the scan
	CScanEvaluation ev;

	// Perform the evaluation!!
	long spectrumNum = ev.EvaluateScan(&scan, fitWindow, &darkSettings);

	// 7. Check the reasonability of the evaluation
	if (spectrumNum == 0) {
		return 6;
	}

	// 8. Get the result from the evaluation
	if (ev.m_result != nullptr) {
		m_lastResult = new CScanResult();
		*m_lastResult = *ev.m_result;
	}

	// 9. Get the mode of the evaluation
	m_lastResult->CheckMeasurementMode();
	m_lastResult->GetStartTime(0, startTime);

	// 10. Append the results to the evaluation-summary log
	AppendToEvaluationSummaryFile(m_lastResult, &scan, &instrLocation, &fitWindow, windField);
	AppendToPakFileSummaryFile(m_lastResult, &scan, &instrLocation, &fitWindow, windField);

	// 10. If this was a flux-measurement then we need to see the plume for the measurement to be useful
	//		this check should only be performed on the main fit window.
	if (Equals(fitWindow.name, g_userSettings.m_fitWindowsToUse[g_userSettings.m_mainFitWindow])) {
		if (0 == CheckQualityOfFluxMeasurement(m_lastResult, pakFileName)) {
			delete m_lastResult;	m_lastResult = nullptr;
			return 6;
		}
	}

	// 10. Append the result to the log file of the corresponding scanningInstrument
	if (SUCCESS != WriteEvaluationResult(m_lastResult, &scan, &instrLocation, &fitWindow, windField, txtFileName)) {
		errorMessage.Format("Failed to write evaluation log file %s. No result produced", txtFileName);
		ShowMessage(errorMessage);
	}

	// 11. Return the properties of the scan
	if (plumeProperties != nullptr) {
		m_lastResult->GetCalculatedPlumeProperties(*plumeProperties);
	}

	// 16. Calculate the time spent in this function
	cFinish = clock();

	// 18. Clean up
	delete m_lastResult;	m_lastResult = nullptr;

	return 0;
}

RETURN_CODE CPostEvaluationController::WriteEvaluationResult(const CScanResult *result, const FileHandler::CScanFileHandler *scan, const Configuration::CInstrumentLocation *instrLocation, const Evaluation::CFitWindow *window, Meteorology::CWindField &windField, novac::CString *txtFileName) {
	novac::CString string, string1, string2, string3, string4;
	long itSpectrum, itSpecie; // iterators
	novac::CString pakFile, txtFile, specModel, evalSummaryLog;
	novac::CString wsSrc, wdSrc, phSrc;
	novac::CDateTime dateTime;

	// get the file-name that we want to have	
	GetArchivingfileName(pakFile, txtFile, window->name, scan->GetFileName(), result->GetMeasurementMode());
	if (txtFileName != nullptr) {
		txtFileName->Format(txtFile);
	}

	// the spectrometer	
	CSpectrometerModel::ToString(instrLocation->m_spectrometerModel, specModel);
	double maxIntensity = CSpectrometerModel::GetMaxIntensity(instrLocation->m_spectrometerModel);

	// The date of the measurement & the serial-number of the spectrometer
	result->GetSkyStartTime(dateTime);

	// 0. Create the additional scan-information
	string.Format("\n<scaninformation>\n");
	string.AppendFormat("\tdate=%02d.%02d.%04d\n", scan->m_startTime.day, scan->m_startTime.month, scan->m_startTime.year);
	string.AppendFormat("\tstarttime=%02d:%02d:%02d\n", scan->m_startTime.hour, scan->m_startTime.minute, scan->m_startTime.second);
	string.AppendFormat("\tcompass=%.1lf\n", instrLocation->m_compass);
	string.AppendFormat("\ttilt=%.1lf\n", instrLocation->m_tilt);
	string.AppendFormat("\tlat=%.6lf\n", instrLocation->m_latitude);
	string.AppendFormat("\tlong=%.6lf\n", instrLocation->m_longitude);
	string.AppendFormat("\talt=%d\n", instrLocation->m_altitude);

	string.AppendFormat("\tvolcano=%s\n", (const char*)instrLocation->m_volcano);
	string.AppendFormat("\tsite=%s\n", (const char*)instrLocation->m_locationName);
	//	string.AppendFormat("\tobservatory=%s\n",							m_common.SimplifyString(spectrometer.m_scanner.observatory));

	string.AppendFormat("\tserial=%s\n", (const char*)result->GetSerial());
	string.AppendFormat("\tspectrometer=%s\n", (const char*)specModel);
	string.AppendFormat("\tchannel=%d\n", window->channel);
	string.AppendFormat("\tconeangle=%.1lf\n", instrLocation->m_coneangle);
	string.AppendFormat("\tinterlacesteps=%d\n", scan->GetInterlaceSteps());
	string.AppendFormat("\tstartchannel=%d\n", scan->GetStartChannel());
	string.AppendFormat("\tspectrumlength=%d\n", scan->GetSpectrumLength());
	string.AppendFormat("\tflux=%.2lf\n", result->GetFlux());
	string.AppendFormat("\tbattery=%.2f\n", result->GetBatteryVoltage());
	string.AppendFormat("\ttemperature=%.2f\n", result->GetTemperature());

	// The mode
	if (result->IsDirectSunMeasurement())
		string.AppendFormat("\tmode=direct_sun\n");
	else if (result->IsLunarMeasurement())
		string.AppendFormat("\tmode=lunar\n");
	else if (result->IsWindMeasurement())
		string.AppendFormat("\tmode=wind\n");
	else if (result->IsStratosphereMeasurement())
		string.AppendFormat("\tmode=stratospheric\n");
	else if (result->IsCompositionMeasurement())
		string.AppendFormat("\tmode=composition\n");
	else
		string.AppendFormat("\tmode=plume\n");

	// The type of instrument used...
	if (instrLocation->m_instrumentType == INSTR_GOTHENBURG) {
		string.AppendFormat("\tinstrumenttype=gothenburg\n");
	}
	else if (instrLocation->m_instrumentType == INSTR_HEIDELBERG) {
		string.AppendFormat("\tinstrumenttype=heidelberg\n");
	}

	// Finally, the version of the file and the version of the program
	string.AppendFormat("\tversion=2.2\n");
	string.AppendFormat("\tsoftware=NovacPPP\n");
	string.AppendFormat("\tcompiledate=%s\n", __DATE__);

	string.AppendFormat("</scaninformation>\n");
	// 0a. Write the additional scan-information to the evaluation log
	FILE *f = fopen(txtFile, "w");
	if (f != nullptr) {
		fprintf(f, string);
		fprintf(f, "\n");
	}

	// 0.1	Create an flux-information part and write it to the same file
	windField.GetWindSpeedSource(wsSrc);
	windField.GetWindDirectionSource(wdSrc);
	//	windField.GetPlumeHeightSource(phSrc);

		// Get the information on where the plume is seen
	double plumeEdge1, plumeEdge2;
	double plumeCompleteness = result->GetCalculatedPlumeCompleteness();
	double plumeCentre1 = result->GetCalculatedPlumeCentre(0);
	double plumeCentre2 = result->GetCalculatedPlumeCentre(1);
	result->GetCalculatedPlumeEdges(plumeEdge1, plumeEdge2);

	string.Format("<fluxinfo>\n");
	string.AppendFormat("\tflux=%.4lf\n", result->GetFlux()); // ton/day
	string.AppendFormat("\twindspeed=%.4lf\n", windField.GetWindSpeed());
	string.AppendFormat("\twinddirection=%.4lf\n", windField.GetWindDirection());
	//	string.AppendFormat("\tplumeheight=%.2lf\n",		windField.GetPlumeHeight());
	string.AppendFormat("\twindspeedsource=%s\n", (const char*)wsSrc);
	string.AppendFormat("\twinddirectionsource=%s\n", (const char*)wdSrc);
	string.AppendFormat("\tplumeheightsource=%s\n", (const char*)phSrc);
	if (fabs(instrLocation->m_compass) > 360.0)
		string.AppendFormat("\tcompasssource=compassreading\n");
	else
		string.AppendFormat("\tcompasssource=user\n");

	string.AppendFormat("\tplumecompleteness=%.2lf\n", plumeCompleteness);
	string.AppendFormat("\tplumecentre=%.2lf\n", plumeCentre1);
	if (instrLocation->m_instrumentType == INSTR_HEIDELBERG)
		string.AppendFormat("\tplumecentre_phi=%.2lf\n", plumeCentre2);
	string.AppendFormat("\tplumeedge1=%.2lf\n", plumeEdge1);
	string.AppendFormat("\tplumeedge2=%.2lf\n", plumeEdge2);

	string.AppendFormat("</fluxinfo>");

	// 0.1b Write the flux-information to the evaluation-log
	if (f != nullptr) {
		fprintf(f, string);
		fprintf(f, "\n");
	}


	// 1. write the header
	if (instrLocation->m_instrumentType == INSTR_GOTHENBURG) {
		string.Format("#scanangle\t");
	}
	else if (instrLocation->m_instrumentType == INSTR_HEIDELBERG) {
		string.Format("#observationangle\tazimuth\t");
	}
	string.AppendFormat("starttime\tstoptime\tname\tspecsaturation\tfitsaturation\tcounts_ms\tdelta\tchisquare\texposuretime\tnumspec\t");

	for (itSpecie = 0; itSpecie < window->nRef; ++itSpecie) {
		string.AppendFormat("column(%s)\tcolumnerror(%s)\t", (const char*)window->ref[itSpecie].m_specieName, (const char*)window->ref[itSpecie].m_specieName);
		string.AppendFormat("shift(%s)\tshifterror(%s)\t", (const char*)window->ref[itSpecie].m_specieName, (const char*)window->ref[itSpecie].m_specieName);
		string.AppendFormat("squeeze(%s)\tsqueezeerror(%s)\t", (const char*)window->ref[itSpecie].m_specieName, (const char*)window->ref[itSpecie].m_specieName);
	}
	string.AppendFormat("isgoodpoint\toffset\tflag");

	// 1a. Write the header to the log file
	if (f != nullptr) {
		fprintf(f, string);
		fprintf(f, "\n<spectraldata>\n");
	}

	// ----------------------------------------------------------------------------------------------
	// 2. ----------------- Write the parameters for the sky and the dark-spectra -------------------
	// ----------------------------------------------------------------------------------------------
	CSpectrum sky, dark, darkCurrent, offset;
	string1.Format(""); string2.Format(""); string3.Format(""); string4.Format("");
	scan->GetSky(sky);
	if (sky.m_info.m_interlaceStep > 1)
		sky.InterpolateSpectrum();
	if (sky.m_length > 0) {
		sky.m_info.m_fitIntensity = (float)(sky.MaxValue(window->fitLow, window->fitHigh));
		if (sky.NumSpectra() > 0)
			sky.Div(sky.NumSpectra());
		CEvaluationLogFileHandler::FormatEvaluationResult(&sky.m_info, nullptr, instrLocation->m_instrumentType, maxIntensity*sky.NumSpectra(), window->nRef, string1);
	}
	scan->GetDark(dark);
	if (dark.m_info.m_interlaceStep > 1)
		dark.InterpolateSpectrum();
	if (dark.m_length > 0) {
		dark.m_info.m_fitIntensity = (float)(dark.MaxValue(window->fitLow, window->fitHigh));
		if (dark.NumSpectra() > 0)
			dark.Div(dark.NumSpectra());
		CEvaluationLogFileHandler::FormatEvaluationResult(&dark.m_info, nullptr, instrLocation->m_instrumentType, maxIntensity*dark.NumSpectra(), window->nRef, string2);
	}
	scan->GetOffset(offset);
	if (offset.m_info.m_interlaceStep > 1)
		offset.InterpolateSpectrum();
	if (offset.m_length > 0) {
		offset.m_info.m_fitIntensity = (float)(offset.MaxValue(window->fitLow, window->fitHigh));
		offset.Div(offset.NumSpectra());
		CEvaluationLogFileHandler::FormatEvaluationResult(&offset.m_info, nullptr, instrLocation->m_instrumentType, maxIntensity * offset.NumSpectra(), window->nRef, string3);
	}
	scan->GetDarkCurrent(darkCurrent);
	if (darkCurrent.m_info.m_interlaceStep > 1)
		darkCurrent.InterpolateSpectrum();
	if (darkCurrent.m_length > 0) {
		darkCurrent.m_info.m_fitIntensity = (float)(darkCurrent.MaxValue(window->fitLow, window->fitHigh));
		darkCurrent.Div(darkCurrent.NumSpectra());
		CEvaluationLogFileHandler::FormatEvaluationResult(&darkCurrent.m_info, nullptr, instrLocation->m_instrumentType, maxIntensity*darkCurrent.NumSpectra(), window->nRef, string4);
	}

	// 2b. Write it all to the evaluation log file
	if (f != nullptr) {
		if (strlen(string1) > 0) {
			fprintf(f, string1); fprintf(f, "\n");
		}
		if (strlen(string2) > 0) {
			fprintf(f, string2); fprintf(f, "\n");
		}
		if (strlen(string3) > 0) {
			fprintf(f, string3); fprintf(f, "\n");
		}
		if (strlen(string4) > 0) {
			fprintf(f, string4); fprintf(f, "\n");
		}
	}


	// ----------------------------------------------------------------------------------------------
	// 3. ------------------- Then write the parameters for each spectrum ---------------------------
	// ----------------------------------------------------------------------------------------------
	for (itSpectrum = 0; itSpectrum < result->GetEvaluatedNum(); ++itSpectrum) {
		int nSpectra = result->GetSpectrumInfo(itSpectrum).m_numSpec;

		// 3a. Pretty print the result and the spectral info into a string
		CEvaluationLogFileHandler::FormatEvaluationResult(&result->GetSpectrumInfo(itSpectrum), result->GetResult(itSpectrum), instrLocation->m_instrumentType, maxIntensity * nSpectra, window->nRef, string);

		// 3b. Write it all to the evaluation log file
		if (f != nullptr) {
			fprintf(f, string);
			fprintf(f, "\n");
		}
	}

	if (f != nullptr) {
		fprintf(f, "</spectraldata>\n");
		fclose(f);
	}

	return SUCCESS;
}

RETURN_CODE CPostEvaluationController::AppendToEvaluationSummaryFile(const CScanResult *result, const FileHandler::CScanFileHandler *scan, const Configuration::CInstrumentLocation *instrLocation, const Evaluation::CFitWindow *window, Meteorology::CWindField &windField) {
	novac::CString evalSummaryLog;
	bool fWriteHeaderLine = false;

	// we can also write an evaluation-summary log file
	evalSummaryLog.Format("%s\\%s\\EvaluationSummary_%s.txt", (const char*)g_userSettings.m_outputDirectory, (const char*)window->name, (const char*)result->GetSerial());

	if (!IsExistingFile(evalSummaryLog)) {
		fWriteHeaderLine = true;
	}

	FILE *f = fopen(evalSummaryLog, "a");
	if (f == nullptr)
		return FAIL;

	if (fWriteHeaderLine) {
		fprintf(f, "StartTime\tExpTime\tAppliedShift\tTemperature\tCalculatedOffset\tCalculatedPlumeCentre\tCalculatedPlumeCompleteness\t#Spectra\n");
	}

	// the start-time
	fprintf(f, "%04d.%02d.%02dT%02d:%02d:%02d\t", scan->m_startTime.year, scan->m_startTime.month, scan->m_startTime.day, scan->m_startTime.hour, scan->m_startTime.minute, scan->m_startTime.second);

	// The exposure time
	fprintf(f, "%ld\t", result->GetSkySpectrumInfo().m_exposureTime);

	// the shift applied
	fprintf(f, "%.2lf\t", result->GetResult(0)->m_ref[0].m_shift);

	// the temperature of the spectrometer
	fprintf(f, "%.2lf\t", result->GetTemperature());

	// the calculated plume parameters
	fprintf(f, "%.2lf\t", result->GetOffset());
	fprintf(f, "%.2lf\t", result->GetCalculatedPlumeCentre());
	fprintf(f, "%.2lf\t", result->GetCalculatedPlumeCompleteness());

	// the number of evaluated spectra
	fprintf(f, "%ld\t", result->GetEvaluatedNum());

	// make a new line
	fprintf(f, "\n");

	// close the file
	fclose(f);

	return SUCCESS;
}

RETURN_CODE CPostEvaluationController::AppendToPakFileSummaryFile(const CScanResult *result, const FileHandler::CScanFileHandler *scan, const Configuration::CInstrumentLocation *instrLocation, const Evaluation::CFitWindow *window, Meteorology::CWindField &windField) {
	novac::CString pakSummaryLog;
	bool fWriteHeaderLine = false;

	// we can also write an evaluation-summary log file
	pakSummaryLog.Format("%s\\PakfileSummary.txt", (const char*)g_userSettings.m_outputDirectory);

	if (!IsExistingFile(pakSummaryLog)) {
		fWriteHeaderLine = true;
	}

	FILE *f = fopen(pakSummaryLog, "a");
	if (f == nullptr)
		return FAIL;

	if (fWriteHeaderLine) {
		fprintf(f, "Serial\tStartTime\tLat\tLong\tAlt\tExpTime\tBatteryVoltage\tTemperature\tElectronicOffset\n");
	}

	// the serial of the instrument
	fprintf(f, "%s\t", (const char*)result->GetSerial());

	// the start-time
	fprintf(f, "%04d.%02d.%02dT%02d:%02d:%02d\t", scan->m_startTime.year, scan->m_startTime.month, scan->m_startTime.day, scan->m_startTime.hour, scan->m_startTime.minute, scan->m_startTime.second);

	// the location
	const CGPSData &gps = scan->GetGPS();
	fprintf(f, "%.5lf\t%.5lf\t%.5lf\t", gps.m_latitude, gps.m_longitude, gps.m_altitude);

	// The exposure time
	fprintf(f, "%ld\t", result->GetSkySpectrumInfo().m_exposureTime);

	// the input-voltage at the time of measurement
	fprintf(f, "%.2lf\t", result->GetBatteryVoltage());

	// the temperature of the spectrometer
	fprintf(f, "%.2lf\t", result->GetTemperature());

	// the offset of the AD converter
	fprintf(f, "%.2lf", result->GetElectronicOffset(0));

	// make a new line
	fprintf(f, "\n");

	// close the file
	fclose(f);

	return SUCCESS;
}

RETURN_CODE CPostEvaluationController::GetArchivingfileName(novac::CString &pakFile, novac::CString &txtFile, const novac::CString &fitWindowName, const novac::CString &temporaryScanFile, MEASUREMENT_MODE mode) {
	CSpectrumIO reader;
	CSpectrum tmpSpec;
	novac::CString serialNumber, dateStr, timeStr, dateStr2, modeStr, userMessage;

	// 0. Make an initial assumption of the file-names
	int i = 0;
	while (1) {
		pakFile.Format("%s\\UnknownScans\\%d.pak", (const char*)g_userSettings.m_outputDirectory, ++i);
		if (!IsExistingFile(pakFile))
			break;
	}
	txtFile.Format("%s\\UnknownScans\\%d.txt", (const char*)g_userSettings.m_outputDirectory, i);

	// 1. Read the first spectrum in the scan
	if (SUCCESS != reader.ReadSpectrum(temporaryScanFile, 0, tmpSpec))
		return FAIL;
	CSpectrumInfo &info = tmpSpec.m_info;
	int channel = info.m_channel;

	// 1a. If the GPS had no connection with the satelites when collecting the sky-spectrum,
	//			then try to find a spectrum in the file for which it had connection...
	i = 1;
	while (info.m_startTime.year == 2004 && info.m_startTime.month == 3 && info.m_startTime.second == 22) {
		if (SUCCESS != reader.ReadSpectrum(temporaryScanFile, i++, tmpSpec))
			break;
		info = tmpSpec.m_info;
	}

	// 2. Get the serialNumber of the spectrometer
	serialNumber.Format("%s", (const char*)info.m_device);

	// 3. Get the time and date when the scan started
	dateStr.Format("%02d%02d%02d", info.m_startTime.year % 1000, info.m_startTime.month, info.m_startTime.day);
	dateStr2.Format("%04d.%02d.%02d", info.m_startTime.year, info.m_startTime.month, info.m_startTime.day);
	timeStr.Format("%02d%02d", info.m_startTime.hour, info.m_startTime.minute);


	// 4. Write the archiving name of the spectrum file

	// 4a. Write the folder name
	pakFile.Format("%s%s\\%s\\%s\\", (const char*)g_userSettings.m_outputDirectory, (const char*)fitWindowName, (const char*)dateStr2, (const char*)serialNumber);
	txtFile.Format("%s", (const char*)pakFile);

	// 4b. Make sure that the folder exists
	int ret = CreateDirectoryStructure(pakFile);
	if (ret) {
		userMessage.Format("Could not create directory for archiving .pak-file: %s", (const char*)pakFile);
		ShowMessage(userMessage);
		return FAIL;
	}

	// 4c. Write the code for the measurement mode
	switch (mode) {
	case MODE_FLUX:			modeStr.Format("flux"); break;
	case MODE_WINDSPEED:	modeStr.Format("wind"); break;
	case MODE_STRATOSPHERE: modeStr.Format("stra"); break;
	case MODE_DIRECT_SUN:	modeStr.Format("dsun"); break;
	case MODE_COMPOSITION:  modeStr.Format("comp"); break;
	case MODE_LUNAR:		modeStr.Format("luna"); break;
	case MODE_TROPOSPHERE:  modeStr.Format("trop"); break;
	case MODE_MAXDOAS:		modeStr.Format("maxd"); break;
	default:				modeStr.Format("unkn"); break;
	}

	// 4c. Write the name of the archiving file itself
	if (channel < 128 && channel > MAX_CHANNEL_NUM)
		channel = channel % 16;
	pakFile.AppendFormat("%s_%s_%s_%1d_%4s.pak", (const char*)serialNumber, (const char*)dateStr, (const char*)timeStr, channel, (const char*)modeStr);
	txtFile.AppendFormat("%s_%s_%s_%1d_%4s.txt", (const char*)serialNumber, (const char*)dateStr, (const char*)timeStr, channel, (const char*)modeStr);

	if (strlen(pakFile) > MAX_PATH)
		return FAIL;

	return SUCCESS;
}

/** Looks in the configuration of the instruments and searches
	for a configured location and fit-window (for evaluation) which
	is valid for the spectrometer that collected the given scan and
	is also valid at the time when the scan was made.
	@return 0 if successful otherwise non-zero
*/
int CPostEvaluationController::GetLocationAndFitWindow(FileHandler::CScanFileHandler *scan, const novac::CString &fitWindowName, Configuration::CInstrumentLocation &instrLocation, Evaluation::CFitWindow &window) {
	CSpectrum skySpec;
	novac::CDateTime day, evalValidFrom, evalValidTo;
	Configuration::CInstrumentConfiguration *instrumentConf = nullptr;
	Configuration::CInstrumentLocation singleLocation;
	novac::CString serialNumber, errorMessage;

	// Get the sky-spectrum. Read out serial-number and start-time from this
	scan->GetSky(skySpec);
	day = skySpec.m_info.m_startTime;
	serialNumber.Format(skySpec.m_info.m_device);

	// Find the instrument location that is valid for this date
	if (g_setup.GetInstrumentLocation(serialNumber, day, instrLocation)) {
		return 1;
	}

	// Then find the evaluation fit-window that is valid for this date
	if (g_setup.GetFitWindow(serialNumber, scan->m_channel, day, window, &fitWindowName)) {
		return 1;
	}

	return 0;
}

/** Looks in the configuration of the instrument and searches
	for the settings on how the dark-current should be removed
	from the collected spectra.
	On successful return will the settings be stored in 'settings'
	@return 0 on successful, otherwise non-zero. */
int CPostEvaluationController::GetDarkCurrentSettings(FileHandler::CScanFileHandler *scan, Configuration::CDarkSettings &settings) {
	CSpectrum skySpec;
	novac::CString serialNumber, errorMessage;
	Configuration::CInstrumentConfiguration *instrumentConf = nullptr;

	// Get the sky-spectrum. Read out serial-number and start-time from this
	scan->GetSky(skySpec);
	serialNumber.Format(skySpec.m_info.m_device);

	return g_setup.GetDarkCorrection(serialNumber, skySpec.m_info.m_startTime, settings);
}


int CPostEvaluationController::CheckQualityOfFluxMeasurement(CScanResult *result, const novac::CString &pakFileName) const {
	novac::CString errorMessage;

	// Check if this is a flux measurement at all
	if (MODE_FLUX != result->GetMeasurementMode()) {
		return -1;
	}
	if (0 == result->CalculateOffset(CMolecule(g_userSettings.m_molecule))) {
		if (0 == result->CalculatePlumeCentre(CMolecule(g_userSettings.m_molecule))) {
			// no plume found!
			errorMessage.Format(" - Scan %s does not see the plume. Scan ignored.", (const char*)pakFileName);
			ShowMessage(errorMessage);
			return 0;
		}
	}
	return 1;
}

/** Checks the supplied scan if it's good enough to bother evaluating.
	@returns false if the scan is too bad and should be ignored. Else return true. */
bool CPostEvaluationController::IsGoodEnoughToEvaluate(const FileHandler::CScanFileHandler *scan, const Evaluation::CFitWindow &fitWindow, Configuration::CInstrumentLocation &instrLocation) {
	CSpectrum skySpectrum;
	novac::CString errorMessage;

	// Check that the sky-spectrum is ok
	scan->GetSky(skySpectrum);
	if (skySpectrum.IsDark()) {
		errorMessage.Format(" - Sky spectrum in scan %s is dark. Will not evaluate scan", (const char*)scan->GetFileName());
		ShowMessage(errorMessage);

		// update the statistics
		g_processingStats.InsertRejection(scan->m_device, CPostProcessingStatistics::SKY_SPEC_DARK);

		return false;
	}

	if ((instrLocation.m_instrumentType == INSTR_GOTHENBURG && skySpectrum.ExposureTime() > g_userSettings.m_maxExposureTime_got) ||
		(instrLocation.m_instrumentType == INSTR_HEIDELBERG && skySpectrum.ExposureTime() > g_userSettings.m_maxExposureTime_hei)) {
		errorMessage.Format(" - Sky spectrum in scan %s has too long exposure time (%ld ms). Will not evaluate scan", (const char*)scan->GetFileName(), skySpectrum.ExposureTime());
		ShowMessage(errorMessage);

		// update the statistics
		g_processingStats.InsertRejection(scan->m_device, CPostProcessingStatistics::SKY_SPEC_TOO_LONG_EXPTIME);

		return false;
	}

	double dynamicRange = skySpectrum.NumSpectra() * CSpectrometerModel::GetMaxIntensity(instrLocation.m_spectrometerModel);
	if (skySpectrum.MaxValue(fitWindow.fitLow, fitWindow.fitHigh) >= dynamicRange) {
		errorMessage.Format(" - Sky spectrum in scan %s is saturated in fit region. Will not evaluate scan", (const char*)scan->GetFileName());
		ShowMessage(errorMessage);

		// update the statistics
		g_processingStats.InsertRejection(scan->m_device, CPostProcessingStatistics::SKY_SPEC_SATURATION);

		return false;
	}

	return true;
}


