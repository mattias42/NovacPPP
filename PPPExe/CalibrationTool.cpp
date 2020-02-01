#include <iostream>
#include <string>
#include <SpectralEvaluation/Spectra/WavelengthCalibration.h>
#include <SpectralEvaluation/Spectra/Spectrum.h>
#include <SpectralEvaluation/File/File.h>

struct WavelengthCalibrationSettings
{
    std::string measuredspectrum;

    std::string solarAtlas; //< Kurucz

    std::string ozoneReference;

    std::string initialWavelengthToPixelMapping;

    std::string instrumentSlitFunction;
};

int main(int /*argc*/, char* /*argv*/[])
{
    // if (2 != argc || strlen(argv[1]) < 4)
    // {
    //     std::cout << "This is the Novac Calibration tool, pass a configuration file (in xml) as parameter." << std::endl;
    //     return 0;
    // }
    WavelengthCalibrationSettings settings;
    settings.measuredspectrum = "D:/Development/NovacRatioTest/Data/PlumeSpectra_20160331/PlumeSpectra_D2J2124_20160331_181425_0/darkCorrectedSkySpectrum.std";
    settings.solarAtlas = "D:/NOVAC/CrossSections/SOLARFL_296-440nm.xs";
    settings.ozoneReference = "D:/Novac/CrossSections/O3_Voigt(2001)_223K_230-851nm(100mbar).txt";
    settings.initialWavelengthToPixelMapping = "D:/NOVAC/References/D2J2124/D2J2124_Master.clb";
    settings.instrumentSlitFunction = "D:/NOVAC/References/D2J2124/D2J2124_Master_302nm.slf";

    Evaluation::WavelengthCalibrationSetup calibrationSetup;
    {
        FileIo::ReadCrossSectionFile(settings.solarAtlas, calibrationSetup.solarAtlas);

        Evaluation::CCrossSectionData ozoneRef;
        FileIo::ReadCrossSectionFile(settings.ozoneReference, ozoneRef);
        calibrationSetup.crossSections.push_back(ozoneRef);
    }

    Evaluation::SpectrometerCalibration initialCalibration;
    {
        Evaluation::CCrossSectionData initialWpm;
        FileIo::ReadCrossSectionFile(settings.initialWavelengthToPixelMapping, initialWpm, true);
        initialCalibration.wavelengthToPixelMapping = initialWpm.m_waveLength;

        FileIo::ReadCrossSectionFile(settings.instrumentSlitFunction, initialCalibration.slf);
    }

    CSpectrum measuredSpectrum;
    FileIo::ReadSpectrum(settings.measuredspectrum, measuredSpectrum);

    Evaluation::SpectrometerCalibration calibration;
    Evaluation::EstimateWavelengthToPixelMapping(
        calibrationSetup,
        initialCalibration,
        measuredSpectrum,
        calibration);
}