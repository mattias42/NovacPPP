#include "ScanEvaluation.h"

#include <SpectralEvaluation/Evaluation/EvaluationBase.h>
#include <SpectralEvaluation/Spectra/Spectrum.h>
#include <SpectralEvaluation/File/SpectrumIO.h>
#include <SpectralEvaluation/File/STDFile.h>
#include <SpectralEvaluation/File/TXTFile.h>

// we want to make some statistics on the processing
#include "../PostProcessingStatistics.h"

// This is the settings for how to do the procesing
#include "../Configuration/UserConfiguration.h"

#include <cstdint>

extern CPostProcessingStatistics					g_processingStats; // <-- The statistics of the processing itself
extern Configuration::CUserConfiguration			g_userSettings;// <-- The settings of the user

using namespace Evaluation;

CScanEvaluation::CScanEvaluation()
{
}

CScanEvaluation::~CScanEvaluation(void)
{
    if (m_result != NULL) {
        delete(m_result);
        m_result = NULL;
    }
}

long CScanEvaluation::EvaluateScan(FileHandler::CScanFileHandler *scan, const CFitWindow &fitWindow, const Configuration::CDarkSettings *darkSettings) {
    CEvaluationBase *eval = NULL; // the evaluator
    CFitWindow adjustedFitWindow = fitWindow; // we may need to make some small adjustments to the fit-window. This is a modified copy

    // Adjust the fit-low and fit-high parameters according to the spectra
    m_fitLow = adjustedFitWindow.fitLow;
    m_fitHigh = adjustedFitWindow.fitHigh;

    // sometimes the length of the spectra is not what we expect, 
    //	we need to be able to handle this.
    adjustedFitWindow.interlaceStep = scan->GetInterlaceSteps();
    adjustedFitWindow.specLength = scan->GetSpectrumLength() * adjustedFitWindow.interlaceStep;
    adjustedFitWindow.startChannel = scan->GetStartChannel();

    // Now choose what we should do before the real evaluation. Should we;
    //	1) find the shift & squeeze from the Fraunhofer spectrum
    //	2) find the optimal shift & squeeze from the spectrum with the highest column
    //  3) do none of the above

    if (adjustedFitWindow.fraunhoferRef.m_path.size() > 4) {
        ShowMessage("  Determining shift from FraunhoferReference");

        // If we have a solar-spectrum that we can use to determine the shift
        //	& squeeze then fit that first so that we know the wavelength calibration
        if (NULL == (eval = FindOptimumShiftAndSqueeze_Fraunhofer(adjustedFitWindow, scan))) {
            return 0;
        }

    }
    else if (fitWindow.findOptimalShift) {
        //	Find the optimal shift & squeeze from the spectrum with the highest column
        CFitWindow window2 = adjustedFitWindow;
        for (int k = 0; k < window2.nRef; ++k) {
            window2.ref[k].m_shiftOption = SHIFT_FIX;
            window2.ref[k].m_squeezeOption = SHIFT_FIX;
            window2.ref[k].m_shiftValue = 0.0;
            window2.ref[k].m_squeezeValue = 1.0;
        }
        eval = new CEvaluationBase(window2);

        // evaluate the scan one time
        if (-1 == EvaluateOpenedScan(scan, eval, darkSettings)) {
            delete eval;
            return 0;
        }
        else {
            FindOptimumShiftAndSqueeze(eval, adjustedFitWindow, scan, m_result);
        }
    }
    else {
        //  3) do none of the above

        eval = new CEvaluationBase(adjustedFitWindow);
    }

    // Make the real evaluation of the scan
    int nSpectra = EvaluateOpenedScan(scan, eval, darkSettings);

    // Clean up
    delete eval;

    if (nSpectra == -1) {
        return 0;
    }

    // return the number of spectra evaluated
    return m_result->GetEvaluatedNum();
}

long CScanEvaluation::EvaluateOpenedScan(FileHandler::CScanFileHandler *scan, CEvaluationBase *eval, const Configuration::CDarkSettings *darkSettings) {
    novac::CString message;	// used for ShowMessage messages
    int	index = 0;		// keeping track of the index of the current spectrum into the .pak-file
    double highestColumn = 0.0;	// the highest column-value in the evaluation

    // variables for storing the sky, dark and the measured spectra
    CSpectrum sky, dark, current;

    // ----------- Get the sky spectrum --------------
    // Get the sky and dark spectra and divide them by the number of 
    //     co-added spectra in it
    if (SUCCESS != GetSky(scan, sky)) {
        return -1;
    }
    CSpectrum original_sky = sky; // original_sky is the sky-spectrum without dark-spectrum corrections...

    if (g_userSettings.m_skyOption != SKY_USER) {
        // Get the dark-spectrum and remove it from the sky
        if (SUCCESS != GetDark(scan, sky, dark, darkSettings))
        {
            return -1;
        }
        sky.Sub(dark);
    }

    if (sky.NumSpectra() > 0 && !m_averagedSpectra) {
        sky.Div(sky.NumSpectra());
        original_sky.Div(original_sky.NumSpectra());
    }

    // tell the evaluator which sky-spectrum to use
    eval->SetSkySpectrum(sky);

    // Adjust the fit-low and fit-high parameters according to the spectra
    m_fitLow -= sky.m_info.m_startChannel;
    m_fitHigh -= sky.m_info.m_startChannel;

    index = -1; // we're at spectrum number 0 in the .pak-file
    m_indexOfMostAbsorbingSpectrum = -1;	// as far as we know, there's no absorption in any spectrum...

    // the data structure to keep track of the evaluation results
    if (m_result != NULL)
        delete m_result;
    m_result = new CScanResult();
    m_result->SetSkySpecInfo(original_sky.m_info);
    m_result->SetDarkSpecInfo(dark.m_info);

    // Make sure that we'll start with the first spectrum in the scan
    scan->ResetCounter();

    // Evaluate all the spectra in the scan.
    while (1) {
        // remember which spectrum we're at
        int	spectrumIndex = current.ScanIndex();

        // a. Read the next spectrum from the file
        int ret = scan->GetNextSpectrum(current);

        if (ret == 0) {
            // if something went wrong when reading the spectrum
            if (scan->m_lastError == SpectrumIO::CSpectrumIO::ERROR_SPECTRUM_NOT_FOUND || scan->m_lastError == SpectrumIO::CSpectrumIO::ERROR_EOF) {
                // at the end of the file, quit the 'while' loop
                break;
            }
            else {
                novac::CString errMsg;
                errMsg.Format("Faulty spectrum found in %s", scan->GetFileName().c_str());
                switch (scan->m_lastError) {
                case SpectrumIO::CSpectrumIO::ERROR_CHECKSUM_MISMATCH:
                    errMsg.AppendFormat(", Checksum mismatch. Spectrum ignored"); break;
                case SpectrumIO::CSpectrumIO::ERROR_DECOMPRESS:
                    errMsg.AppendFormat(", Decompression error. Spectrum ignored"); break;
                default:
                    ShowMessage(", Unknown error. Spectrum ignored");
                }
                ShowMessage(errMsg);
                // remember that this spectrum is corrupted
                m_result->MarkAsCorrupted(spectrumIndex);
                continue;
            }
        }

        ++index;	// we'have just read the next spectrum in the .pak-file

        // If the read spectrum is the sky or the dark spectrum, 
        //	then don't evaluate it...
        if (current.ScanIndex() == sky.ScanIndex() || current.ScanIndex() == dark.ScanIndex()) {
            continue;
        }

        // If the spectrum is read out in an interlaced way then interpolate it back to it's original state
        if (current.m_info.m_interlaceStep > 1)
            current.InterpolateSpectrum();

        // b. Get the dark spectrum for this measured spectrum
        if (SUCCESS != GetDark(scan, current, dark, darkSettings))
        {
            delete scan;
            delete eval;
            return 0;
        }

        // b. Calculate the intensities, before we divide by the number of spectra
        //		and before we subtract the dark
        current.m_info.m_peakIntensity = (float)current.MaxValue(0, current.m_length - 2);
        current.m_info.m_fitIntensity = (float)current.MaxValue(m_fitLow, m_fitHigh);

        // c. Divide the measured spectrum with the number of co-added spectra
        //     The sky and dark spectra should already be divided before this loop.
        if (current.NumSpectra() > 0 && !m_averagedSpectra)
            current.Div(current.NumSpectra());

        // d. Get the dark spectrum
        if (dark.NumSpectra() > 0 && !m_averagedSpectra)
            dark.Div(dark.NumSpectra());

        // e. Check if this spectrum is worth evaluating
        if (Ignore(current, dark, m_fitLow, m_fitHigh)) {
            message.Format("  - Ignoring spectrum %d in scan %s.", current.ScanIndex(), scan->GetFileName().c_str());
            ShowMessage(message);
            continue;
        }

        // f. The spectrum is ok, remove the dark.
        current.Sub(dark);

        // e. Evaluate the spectrum
        if (eval->Evaluate(current)) {
            message.Format("Failed to evaluate spectrum %d out of %d in scan %s from spectrometer %s.",
                current.ScanIndex(), current.SpectraPerScan(), scan->GetFileName().c_str(), current.m_info.m_device.c_str());
            ShowMessage(message);
        }

        // e. Save the evaluation result
        m_result->AppendResult(eval->GetEvaluationResult(), current.m_info);

        // f. Check if this was an ok data point (CScanResult)
        m_result->CheckGoodnessOfFit(current.m_info);

        // g. If it is ok, then check if the value is higher than any of the previous ones
        if (m_result->IsOk(m_result->GetEvaluatedNum() - 1) && fabs(m_result->GetColumn(m_result->GetEvaluatedNum() - 1, 0)) > highestColumn) {
            highestColumn = fabs(m_result->GetColumn(m_result->GetEvaluatedNum() - 1, 0));
            m_indexOfMostAbsorbingSpectrum = index;
        }
    } // end while(1)

    return m_result->GetEvaluatedNum();
}


RETURN_CODE CScanEvaluation::GetDark(FileHandler::CScanFileHandler *scan, const CSpectrum &spec, CSpectrum &dark, const Configuration::CDarkSettings *darkSettings)
{
    m_lastErrorMessage = "";
    const bool successs = ScanEvaluationBase::GetDark(*scan, spec, dark, darkSettings);

    if (m_lastErrorMessage.size() > 0)
    {
        novac::CString message;
        message.Format("%s", m_lastErrorMessage.c_str());
        ShowMessage(message);
    }

    if (successs)
        return SUCCESS;
    else
        return FAIL;
}

/** This returns the sky spectrum that is to be used in the fitting. */
RETURN_CODE CScanEvaluation::GetSky(FileHandler::CScanFileHandler *scan, CSpectrum &sky) {
    novac::CString errorMsg;

    // If the sky spectrum is the first spectrum in the scan
    if (g_userSettings.m_skyOption == SKY_SCAN) {
        scan->GetSky(sky);

        if (sky.m_info.m_interlaceStep > 1)
            sky.InterpolateSpectrum();

        return SUCCESS;
    }

    // If the sky spectrum is the average of all credible spectra
    if (g_userSettings.m_skyOption == SKY_AVERAGE_OF_GOOD) {
        int interlaceSteps = scan->GetInterlaceSteps();
        int startChannel = scan->GetStartChannel();
        int fitLow = m_fitLow / interlaceSteps - startChannel;
        int fitHigh = m_fitHigh / interlaceSteps - startChannel;

        CSpectrum tmp;
        scan->GetSky(tmp);
        scan->ResetCounter();
        double intens = tmp.MaxValue(fitLow, fitHigh);
        if (intens < 4095 * tmp.NumSpectra() && !tmp.IsDark())
            sky = tmp;
        else
            sky.Clear();
        while (scan->GetNextSpectrum(tmp)) {
            intens = tmp.MaxValue(fitLow, fitHigh);
            if (intens < 4095 * tmp.NumSpectra() && !tmp.IsDark())
                sky.Add(tmp);
        }
        scan->ResetCounter();

        if (sky.m_info.m_interlaceStep > 1)
            sky.InterpolateSpectrum();

        return SUCCESS;
    }

    // If the user wants to use another spectrum than 'sky' as reference-spectrum...
    if (g_userSettings.m_skyOption == SKY_INDEX) {
        if (0 == scan->GetSpectrum(sky, g_userSettings.m_skyIndex))
            return FAIL;

        if (sky.m_info.m_interlaceStep > 1)
            sky.InterpolateSpectrum();

        return SUCCESS;
    }

    // If the user has supplied a special sky-spectrum to use
    if (g_userSettings.m_skyOption == SKY_USER) {
        if (Equals(g_userSettings.m_skySpectrumFromUser.Right(4), ".pak", 4)) {
            // If the spectrum is in .pak format
            SpectrumIO::CSpectrumIO reader;
            const std::string userSkySpectrumFil((const char*)g_userSettings.m_skySpectrumFromUser);
            if (reader.ReadSpectrum(userSkySpectrumFil, 0, sky))
                return SUCCESS;
            else
                return FAIL;
        }
        else if (Equals(g_userSettings.m_skySpectrumFromUser.Right(4), ".std", 4)) {
            // If the spectrum is in .std format
            const std::string fileNameStr((const char*)g_userSettings.m_skySpectrumFromUser);
            if (SpectrumIO::CSTDFile::ReadSpectrum(sky, fileNameStr))
                return SUCCESS;
            else
                return FAIL;
        }
        else {
            // If we don't recognize the sky-spectrum format
            errorMsg.Format("Unknown format for sky spectrum. Please use .pak or .std");
            ShowError(errorMsg);
            return FAIL;
        }
    }

    return FAIL;
}

/** Returns true if the spectrum should be ignored */
bool CScanEvaluation::Ignore(const CSpectrum &spec, const CSpectrum &dark, int fitLow, int fitHigh) {

    // check if the intensity is below the given limit
    double maxIntensity = spec.MaxValue(fitLow, fitHigh) - dark.MinValue(fitLow, fitHigh);

    double dynamicRange = CSpectrometerModel::GetMaxIntensity(spec.m_info.m_specModel);

    if (maxIntensity < (dynamicRange * g_userSettings.m_minimumSaturationInFitRegion)) {
        return true;
    }

    return false;
}


/** Finds the optimum shift and squeeze for an evaluated scan */
void CScanEvaluation::FindOptimumShiftAndSqueeze(CEvaluationBase *eval, const CFitWindow &fitWindow, FileHandler::CScanFileHandler *scan, CScanResult *result) {
    int k;
    CSpectrum spec, sky, dark;
    int specieNum = 0;
    novac::CString message;
    CEvaluationBase *eval2 = NULL;
    CFitWindow fitWindow2;

    // 1. Find the spectrum with the highest column value
    if (m_indexOfMostAbsorbingSpectrum < 0) {
        ShowMessage("Could not determine optimal shift & squeeze. No good spectra in scan. %s", scan->GetFileName());
        return; // <-- no good data-point found. Quit it!
    }

    // 2. Make sure that this spectrum was ok and that the column-value is high enough
    double columnError = result->GetColumnError(m_indexOfMostAbsorbingSpectrum, specieNum); // <-- the column error that corresponds to the highest column-value
    double highestColumn = result->GetColumn(m_indexOfMostAbsorbingSpectrum, specieNum);
    if (highestColumn < 2 * columnError) {
        ShowMessage("Could not determine optimal shift & squeeze. Maximum column is too low.");
        return;
    }

    // 2. Tell the user
    message.Format("ReEvaluating spectrum number %d to determine optimum shift and squeeze", m_indexOfMostAbsorbingSpectrum);
    ShowMessage(message);

    // 3. Evaluate this spectrum again with free (and linked) shift
    fitWindow2 = fitWindow;
    fitWindow2.ref[0].m_shiftOption = SHIFT_FREE;
    fitWindow2.ref[0].m_squeezeOption = SHIFT_FIX;
    fitWindow2.ref[0].m_squeezeValue = 1.0;
    for (k = 1; k < fitWindow2.nRef; ++k) {
        if (novac::Equals(fitWindow2.ref[k].m_specieName, "FraunhoferRef"))
            continue;

        fitWindow2.ref[k].m_shiftOption = SHIFT_LINK;
        fitWindow2.ref[k].m_squeezeOption = SHIFT_LINK;
        fitWindow2.ref[k].m_shiftValue = 0.0;
        fitWindow2.ref[k].m_squeezeValue = 0.0;
    }
    // Get the sky-spectrum
    GetSky(scan, sky);
    if (sky.NumSpectra() > 0 && !m_averagedSpectra)
        sky.Div(sky.NumSpectra());

    // Get the dark-spectrum
    GetDark(scan, sky, dark);
    if (dark.NumSpectra() > 0 && !m_averagedSpectra)
        dark.Div(dark.NumSpectra());

    // Subtract the dark...
    sky.Sub(dark);

    // create the new evaluator
    eval2 = new CEvaluationBase(fitWindow2);
    eval2->SetSkySpectrum(sky);

    // Get the measured spectrum
    scan->GetSpectrum(spec, 2 + m_indexOfMostAbsorbingSpectrum); // The two comes from the sky and the dark spectra in the beginning
    if (spec.m_info.m_interlaceStep > 1)
        spec.InterpolateSpectrum();
    if (spec.NumSpectra() > 0 && !m_averagedSpectra)
        spec.Div(spec.NumSpectra());

    // Get the dark-spectrum and remove it
    GetDark(scan, spec, dark);
    spec.Sub(dark);

    // Evaluate
    eval2->Evaluate(spec, 5000);

    // 4. See what the optimum value for the shift turned out to be.
    CEvaluationResult newResult = eval2->GetEvaluationResult();
    double optimumShift = newResult.m_referenceResult[0].m_shift;
    double optimumSqueeze = newResult.m_referenceResult[0].m_squeeze;

    // 5. Set the shift for all references to this value
    for (k = 0; k < fitWindow2.nRef; ++k) {
        if (novac::Equals(fitWindow2.ref[k].m_specieName, "FraunhoferRef"))
            continue;

        fitWindow2.ref[k].m_shiftOption = SHIFT_FIX;
        fitWindow2.ref[k].m_squeezeOption = SHIFT_FIX;
        fitWindow2.ref[k].m_shiftValue = optimumShift;
        fitWindow2.ref[k].m_squeezeValue = optimumSqueeze;
    }
    delete eval;
    eval = new CEvaluationBase(fitWindow2);
    eval->SetSkySpectrum(sky);

    // 6. We're done!
    message.Format("Optimum shift set to : %.2lf. Optimum squeeze set to: %.2lf ", optimumShift, optimumSqueeze);
    ShowMessage(message);
    return;
}

CEvaluationBase *CScanEvaluation::FindOptimumShiftAndSqueeze_Fraunhofer(const CFitWindow &fitWindow, FileHandler::CScanFileHandler *scan) {
    double shift, shiftError, squeeze, squeezeError;
    CSpectrum sky, spectrum, dark;
    novac::CString message;
    double fitIntensity, fitSaturation, maxInt;
    double bestSaturation = -1.0;
    int curIndex = 0;
    const int INDEX_OF_SKYSPECTRUM = -1;
    const int NO_SPECTRUM_INDEX = -2;
    CFitWindow improvedFitWindow = fitWindow;

    // 1. Find the spectrum for which we should determine shift & squeeze
    //			This spectrum should have high enough intensity in the fit-region
    //			without being saturated.
    int indexOfMostSuitableSpectrum = NO_SPECTRUM_INDEX;
    scan->GetSky(sky);
    fitIntensity = sky.MaxValue(fitWindow.fitLow, fitWindow.fitHigh);
    maxInt = CSpectrometerModel::GetMaxIntensity(sky.m_info.m_specModel);
    if (sky.NumSpectra() > 0) {
        fitSaturation = fitIntensity / (sky.NumSpectra() * maxInt);
    }
    else {
        fitSaturation = fitIntensity / (maxInt * sky.NumSpectra());
    }
    if (fitSaturation < 0.9 && fitSaturation > 0.1) {
        indexOfMostSuitableSpectrum = INDEX_OF_SKYSPECTRUM; // sky-spectrum
        bestSaturation = fitSaturation;
    }
    scan->ResetCounter(); // start from the beginning
    while (scan->GetNextSpectrum(spectrum)) {
        fitIntensity = spectrum.MaxValue(fitWindow.fitLow, fitWindow.fitHigh);
        maxInt = CSpectrometerModel::GetMaxIntensity(spectrum.m_info.m_specModel);

        // Get the saturation-ratio for this spectrum
        if (spectrum.NumSpectra() > 0) {
            fitSaturation = fitIntensity / (spectrum.NumSpectra() * maxInt);
        }
        else {
            fitSaturation = fitIntensity / (maxInt * spectrum.NumSpectra());
        }

        // Check if this spectrum is good...
        if (fitSaturation < 0.9 && fitSaturation > 0.1) {
            if (fitSaturation > bestSaturation) {
                indexOfMostSuitableSpectrum = curIndex;
                bestSaturation = fitSaturation;
            }
        }

        // Go to the next spectrum
        ++curIndex;
    }

    // 2. Get the spectrum we should evaluate...
    if (indexOfMostSuitableSpectrum == NO_SPECTRUM_INDEX) {
        ShowMessage("  Could not find any suitable spectrum to determine shift from.");
        return NULL; // we could not find any good spectrum to use...
    }
    else if (indexOfMostSuitableSpectrum == INDEX_OF_SKYSPECTRUM) {
        scan->GetSky(spectrum);
        message.Format("Determining shift and squeeze from sky-spectrum");
    }
    else {
        scan->GetSpectrum(spectrum, indexOfMostSuitableSpectrum);
        message.Format("Determining shift and squeeze from spectrum %d", indexOfMostSuitableSpectrum);
    }
    if (spectrum.NumSpectra() > 0 && !m_averagedSpectra)
        spectrum.Div(spectrum.NumSpectra());
    if (SUCCESS != GetDark(scan, spectrum, dark)) {
        return NULL; // fail
    }
    if (dark.NumSpectra() > 0 && !m_averagedSpectra)
        dark.Div(dark.NumSpectra());
    spectrum.Sub(dark);

    ShowMessage(message);

    // 3a. Release the shift of all the references
    for (int it = 0; it < improvedFitWindow.nRef; ++it) {
        improvedFitWindow.ref[it].m_shiftOption = SHIFT_FREE;
        improvedFitWindow.ref[it].m_squeezeOption = SHIFT_FIX;
        improvedFitWindow.ref[it].m_squeezeValue = 1.0;
    }

    // 3. Do the evaluation.
    CEvaluationBase eval(improvedFitWindow);
    eval.SetSkySpectrum(sky);

    if (eval.EvaluateShift(spectrum, shift, shiftError, squeeze, squeezeError)) {
        // We failed to make the fit, what shall we do now??
        ShowMessage("Failed to determine shift and squeeze in scan %s. Will proceed with default parameters.", scan->GetFileName());
    }
    else {
        if (fabs(shiftError) < 1 && fabs(squeezeError) < 0.01) {
            // The fit is good enough to use the values
            for (int it = 0; it < improvedFitWindow.nRef; ++it) {
                improvedFitWindow.ref[it].m_shiftOption = SHIFT_FIX;
                improvedFitWindow.ref[it].m_squeezeOption = SHIFT_FIX;
                improvedFitWindow.ref[it].m_shiftValue = shift;
                improvedFitWindow.ref[it].m_squeezeValue = squeeze;
            }
            message.Format("  Shift: %.2lf +- %.2lf; Squeeze: %.2lf +- %.2lf", shift, shiftError, squeeze, squeezeError);
            ShowMessage(message);
        }
        else {
            ShowMessage("Fit not good enough. Will proceed with default parameters.");
        }
    }

    return new CEvaluationBase(improvedFitWindow);
}