#include "PostCalibration.h"
#include "../Common/Common.h"
#include <SpectralEvaluation/DialogControllers/NovacProgramWavelengthCalibrationController.h>
#include <SpectralEvaluation/DialogControllers/ReferenceCreationController.h>
#include <SpectralEvaluation/File/File.h>
#include <SpectralEvaluation/Calibration/StandardCrossSectionSetup.h>
#include <SpectralEvaluation/File/ScanFileHandler.h>
#include "../Configuration/NovacPPPConfiguration.h"
#include "../Configuration/UserConfiguration.h"
#include "PostCalibrationStatistics.h"
#include <sstream>


extern Configuration::CNovacPPPConfiguration g_setup; // <-- The setup of the instruments
extern Configuration::CUserConfiguration g_userSettings; // <-- The settings of the user for what to process

using namespace novac;

std::string FormatDateAndTimeOfSpectrum(const novac::CSpectrumInfo& spectrumInformation)
{
    CString dateAndTime;

    dateAndTime.Format(
        "%02d%02d%02d_%02d%02d",
        spectrumInformation.m_startTime.year % 1000,
        spectrumInformation.m_startTime.month,
        spectrumInformation.m_startTime.day,
        spectrumInformation.m_startTime.hour,
        spectrumInformation.m_startTime.minute);

    return std::string(dateAndTime);
}

std::string GetCalibrationFileName(const novac::CSpectrumInfo& spectrumInformation)
{
    return "Calibration_" + spectrumInformation.m_device + "_" + FormatDateAndTimeOfSpectrum(spectrumInformation) + ".std";
}

void RunCalibration(NovacProgramWavelengthCalibrationController& calibrationController, const std::string& scanFile, const Configuration::CInstrumentCalibrationConfiguration& calibrationSettings)
{
    calibrationController.m_inputSpectrumFile = scanFile;
    calibrationController.m_solarSpectrumFile = g_userSettings.m_highResolutionSolarSpectrumFile;
    calibrationController.m_initialCalibrationFile = calibrationSettings.m_initialCalibrationFile;
    calibrationController.m_initialLineShapeFile = calibrationSettings.m_instrumentLineshapeFile;
    calibrationController.m_instrumentLineShapeFitOption = (WavelengthCalibrationController::InstrumentLineShapeFitOption)g_userSettings.m_calibrationInstrumentLineShapeFitOption;
    calibrationController.m_instrumentLineShapeFitRegion = g_userSettings.m_calibrationInstrumentLineShapeFitRegion;

    // Does the actual calibration. Throws a std::exception if the calibration fails.
    calibrationController.RunCalibration();
}

std::vector<novac::CReferenceFile> CreateStandardReferences(
    const novac::CSpectrumInfo& spectrumInformation,
    const std::unique_ptr<novac::InstrumentCalibration>& calibration,
    const novac::StandardCrossSectionSetup standardCrossSections,
    const std::string& directoryName)
{
    ReferenceCreationController referenceController;
    std::vector<novac::CReferenceFile> referencesCreated;

    referenceController.m_highPassFilter = false; // TODO: Are the references sometimes filtered here?
    referenceController.m_unitSelection = 0;

    for (size_t ii = 0; ii < standardCrossSections.NumberOfReferences(); ++ii)
    {
        referenceController.m_convertToAir = standardCrossSections.IsReferenceInVacuum(ii);
        referenceController.m_highResolutionCrossSection = standardCrossSections.ReferenceFileName(ii);
        referenceController.m_isPseudoAbsorber = standardCrossSections.IsAdditionalAbsorber(ii);
        referenceController.ConvolveReference(*calibration);

        // Save the result
        const std::string filteringStr = (referenceController.m_highPassFilter) ?
            "_HP500_PPMM" :
            "";
        const std::string dstFileName =
            directoryName +
            spectrumInformation.m_device +
            "_" +
            standardCrossSections.ReferenceSpecieName(ii) +
            filteringStr +
            "_" +
            FormatDateAndTimeOfSpectrum(spectrumInformation) +
            ".txt";
        novac::SaveCrossSectionFile(dstFileName, *(referenceController.m_resultingCrossSection));

        novac::CReferenceFile newReference;
        newReference.m_specieName = standardCrossSections.ReferenceSpecieName(ii);
        newReference.m_path = dstFileName;
        newReference.m_isFiltered = referenceController.m_highPassFilter;
        referencesCreated.push_back(newReference);
    }

    return referencesCreated;
}

// ----------- PostCalibration class -----------

bool IsTimeForInstrumentCalibration(
    const CPostCalibrationStatistics& statistics,
    const CSpectrumInfo& skySpectrumInScan)
{
    // Check if the scan lies within the configured interval. Slightly complex logic here since the interval may wrap around midnight UTC.
    // Notice that this logic is shared with the real-time calibration in the NovacProgram.
    const bool calibrationIntervalWrapsMidnight = g_userSettings.m_calibrationIntervalTimeOfDayLow > g_userSettings.m_calibrationIntervalTimeOfDayHigh;
    bool scanTimeLiesWithinCalibrationTimeInterval = false;
    if (calibrationIntervalWrapsMidnight)
    {
        scanTimeLiesWithinCalibrationTimeInterval =
            skySpectrumInScan.m_startTime.SecondsSinceMidnight() >= g_userSettings.m_calibrationIntervalTimeOfDayLow ||
            skySpectrumInScan.m_startTime.SecondsSinceMidnight() <= g_userSettings.m_calibrationIntervalTimeOfDayHigh;
    }
    else
    {
        scanTimeLiesWithinCalibrationTimeInterval =
            skySpectrumInScan.m_startTime.SecondsSinceMidnight() >= g_userSettings.m_calibrationIntervalTimeOfDayLow &&
            skySpectrumInScan.m_startTime.SecondsSinceMidnight() <= g_userSettings.m_calibrationIntervalTimeOfDayHigh;
    }

    if (!scanTimeLiesWithinCalibrationTimeInterval)
    {
        std::stringstream message;
        message << "Measurement time (" << skySpectrumInScan.m_startTime.SecondsSinceMidnight() << ") is outside of configured interval [";
        message << g_userSettings.m_calibrationIntervalTimeOfDayLow << " to " << g_userSettings.m_calibrationIntervalTimeOfDayHigh << "]";
        // AppendMessageToLog(spectrometer, message.str());
        return false;
    }

    // no further objections found.
    return true;
}

int CPostCalibration::RunInstrumentCalibration(const std::vector<std::string>& scanFileList, CPostCalibrationStatistics& statistics)
{
    // TODO: Organize the pak files by scanner and then sort by time
    //  this makes it possible to select the files which are to be calibrated

    int numberOfCalibrations = 0;
    for (const std::string& file : scanFileList)
    {
        if (RunInstrumentCalibration(file, statistics))
        {
            ++numberOfCalibrations;
        }
    }

    // Now that all instruments and scans have been calibrated, we should be able to build up our own set of .exml files - replacing the old ones.

    return numberOfCalibrations;
}

bool CPostCalibration::RunInstrumentCalibration(const std::string& scanFile, CPostCalibrationStatistics& statistics)
{
    try
    {
        CScanFileHandler scan;
        if (SUCCESS != scan.CheckScanFile(scanFile))
        {
            std::stringstream message;
            message << "Could not read recieved pak-file '" << scanFile << "' . Will not perform calibration." << std::endl;
            ShowMessage(message.str());
            return false;
        }

        // Get the sky-spectrum. Read out serial-number and start-time from this
        CSpectrum skySpec;
        if (scan.GetSky(skySpec))
        {
            std::stringstream message;
            message << "Could not read a sky spectrum from pak-file '" << scanFile << "' . Will not perform calibration." << std::endl;
            ShowMessage(message.str());
            return false;
        }

        const auto* instrument = g_setup.GetInstrument(skySpec.m_info.m_device);
        if (instrument == nullptr)
        {
            std::stringstream message;
            message << "Received pak file from not configured instrument: '" << skySpec.m_info.m_device << "' . Will not perform calibration." << std::endl;
            ShowMessage(message.str());
            return false;
        }

        // Use the WavelengthCalibrationController, which is also used when the 
        //  user performs the instrument calibrations using the CCalibratePixelToWavelengthDialog.
        // This makes sure we get the same behavior in the dialog and here.
        NovacProgramWavelengthCalibrationController calibrationController;
        RunCalibration(calibrationController, scanFile, instrument->m_instrumentCalibration);

        // Save new instrument calibration. TODO: More organized layout would be better - not all references directly in the output directory
        const std::string directoryName{ (const char*)g_userSettings.m_outputDirectory };
        const std::string calibrationFileName = directoryName + GetCalibrationFileName(calibrationController.m_calibrationDebug.spectrumInfo);
        calibrationController.SaveResultAsStd(calibrationFileName);

        // Create the standard references.
        const auto finalCalibration = calibrationController.GetFinalCalibration();
        auto referencesCreated = CreateStandardReferences(
            calibrationController.m_calibrationDebug.spectrumInfo,
            finalCalibration,
            m_standardCrossSections,
            directoryName);

        // TODO: Now that all the references have been successfully created, remember the results such that we can build upon the evaluation setting.

        /*
    // All references have successfully been created, replace the references used by the evaluation with the new references.
    if (autoCalibrationSettings.generateReferences && referencesCreated.size() > 0)
    {
        int scannerIdx = 0;
        int spectrometerIdx = 0;
        const std::string serialNumber = std::string(spectrometer.SerialNumber());
        if (IdentifySpectrometer(settings, serialNumber, scannerIdx, spectrometerIdx))
        {
            // Update the settings.
            // Notice that there are multiple copies of the settings found in the CSpectrometer (unclear why)
            //  hence we need to replace all of them for the settings to work (for sure).
            ReplaceReferences(referencesCreated, settings.scanner[scannerIdx].spec[spectrometerIdx]);
            ReplaceReferences(referencesCreated, spectrometer.m_settings);
            ReplaceReferences(referencesCreated, spectrometer.m_scanner.spec[spectrometerIdx]);
            spectrometer.m_fitWindows[0] = spectrometer.m_settings.channel[0].fitWindow;

            // Save the updated settings to file
            FileHandler::CConfigurationFileHandler writer;
            writer.WriteConfigurationFile(settings);
        }
    }

    // Remember the result
    spectrometer.m_history->AppendInstrumentCalibration(
        calibrationController.m_calibrationDebug.spectrumInfo.m_startTime,
        calibrationController.GetFinalCalibration());
        */

        return true;
    }
    catch (std::exception& e)
    {
        ShowError(e.what());
    }
    return false;
}
