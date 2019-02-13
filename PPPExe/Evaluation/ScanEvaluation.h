#pragma once

#include "ScanResult.h"
#include "../Configuration/DarkSettings.h"

#include "../Common/Common.h"
#include <PPPLib/Spectra/ScanFileHandler.h>

namespace Evaluation
{
    class CFitWindow;
    class CEvaluationBase;

	/** 
		An object of the <b>CScanEvaluation</b>-class handles the evaluation of one
		scan. The actual evaluation is performed by calling the function 
		<b>EvaluateScan</b> with the name of the .pak-file that we want to evaluate,
		an evaluator to use for the evaluation and parameters for the dark correction.
	*/
	class CScanEvaluation
	{

	public:
		/** Default constructor */
		CScanEvaluation(void);

		/** Default destructor */
		~CScanEvaluation(void);

		/** if pView != NULL then after the evaluation of a spectrum, a 'WM_EVAL_SUCCESS'
			message will be sent to pView. */
		// CWnd *pView;

		/** The evaluation results from the last scan evaluated */
		CScanResult *m_result;

		/** Called to evaluate one scan.
				@return the number of spectra evaluated. */
		long EvaluateScan(FileHandler::CScanFileHandler *scan, const CFitWindow &fitWindow, const Configuration::CDarkSettings *darkSettings = NULL);

		/** Setting the option for wheather the spectra are averaged or not. */
		void SetOption_AveragedSpectra(bool averaged);
	private:

		// ----------------------- PRIVATE METHODS ---------------------------

		/** Performs the evaluation using the supplied evaluator 
			@return the number of spectra evaluated 
			@return -1 if something goes wrong */
		long EvaluateOpenedScan(FileHandler::CScanFileHandler *scan, CEvaluationBase *eval, const Configuration::CDarkSettings *darkSettings = NULL);

		/** This returns the sky spectrum that is to be used in the fitting. */
		RETURN_CODE GetSky(FileHandler::CScanFileHandler *scan, CSpectrum &sky);

		/** This returns the dark spectrum that is to be used in the fitting. 
			@param scan - the scan-file handler from which to get the dark spectrum
			@param spec - the spectrum for which the dark spectrum should be retrieved
			@param dark - will on return be filled with the dark spectrum 
			@param darkSettings - the settings for how to get the dark spectrum from this spectrometer */
		RETURN_CODE GetDark(FileHandler::CScanFileHandler *scan, const CSpectrum &spec, CSpectrum &dark, const Configuration::CDarkSettings *darkSettings = NULL);

		/** checks the spectrum to the settings and returns 'true' if the spectrum should not be evaluated.
			The spectra 'spec' and 'dark' should be divided by the number of co-adds before calling this function.
		 */
		bool Ignore(const CSpectrum &spec, const CSpectrum &dark, int fitLow, int fitHigh);

		/** Finds the optimum shift and squeeze for an evaluated scan 
					by looking at the spectrum with the highest absorption of the evaluated specie
					and evaluate it with shift and squeeze free */
		void FindOptimumShiftAndSqueeze(CEvaluationBase *eval, const CFitWindow &fitWindow, FileHandler::CScanFileHandler *scan, CScanResult *result);

		/** Finds the optimum shift and squeeze for an scan by evaluating
			with a solar-reference spectrum and studying the shift of the 
			Fraunhofer-lines. 
			@param eval - the evaluator to use for the evaluation. On successful determination
				of the shift and squeeze then the shift and squeeze of the reference-files
				in the CEvaluation-objects CFitWindow will be fixed to the optimum value found
			@param scan - a handle to the spectrum file. */
        CEvaluationBase *FindOptimumShiftAndSqueeze_Fraunhofer(const CFitWindow &fitWindow, FileHandler::CScanFileHandler *scan);

		// ------------------------ THE PARAMETERS FOR THE EVALUATION ------------------

		/** This is the fit region */
		long m_fitLow;
		long m_fitHigh;

		/** True if the spectra are averaged, not summed */
		bool m_averagedSpectra;

		/** Remember the index of the spectrum with the highest absorption, to be able to
			adjust the shift and squeeze with it later */
		int m_indexOfMostAbsorbingSpectrum;

		/** how many spectra there are in the current scan-file (for showing the progress) */
		long m_prog_SpecNum;

		/** which spectrum we are on in the current scan-file (for showing the progress) */
		long m_prog_SpecCur;
	};
}