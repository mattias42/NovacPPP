// Evaluation.h: interface for the CEvaluation class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_EVALUATION_H__DB88EE51_7ED0_4131_AE07_79F0F0C3106C__INCLUDED_)
#define AFX_EVALUATION_H__DB88EE51_7ED0_4131_AE07_79F0F0C3106C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include <PPPLib/SpectralEvaluation/Evaluation/BasicMath.h>
#include "EvaluationResult.h"
#include "FitWindow.h"
#include "CrossSectionData.h"

#include <PPPLib/SpectralEvaluation/Fit/Vector.h>
#include <PPPLib/SpectralEvaluation/Fit/ReferenceSpectrumFunction.h>

#include <PPPLib/Spectra/Spectrum.h>

namespace Evaluation
{
	/** constants for selection of fit-parameters*/
	enum FIT_PARAMETER{ 
			COLUMN, 
			COLUMN_ERROR, 
			SHIFT, 
			SHIFT_ERROR, 
			SQUEEZE, 
			SQUEEZE_ERROR,
			DELTA};

	/** An object of the <b>CEvaluation</b>-class contains the settings to evaluate
		spectra from <b>one</b> spectrometer. The class contains the parameters 
		necessary to define the fit and a function to perform the actual evaluation. 
	*/
	class CEvaluation : public CBasicMath	
	{
	public:
		/** Sets up the evaluation */
		CEvaluation(const CFitWindow &window);

		/** Default destructor */
		virtual ~CEvaluation();

		// ----------- This is currently not used in the NovacPPP and therefore
		// ---------------- eliminated to save processing time -------
		///** The residual of the latest fit */
		//CVector m_residual;

		// ----------- These are currently not used in the NovacPPP and therefore
		// ---------------- eliminated to save processing time -------
		///** The scaled reference spectra. The first reference spectrum is the fitted
		//	polynomial and the following 'MAX_N_REFERENCES + 1' are the scaled references. */
		//CCrossSectionData m_fitResult[MAX_N_REFERENCES + 2];

		/** Evaluate using the parameters set in the local parameter 'm_window'
			and using the sky-spectrum which has been set by a previous call to
			'SetSkySpectrum'
			@return 0 if all is ok.
			@return 1 if any error occurred, or if the window is not defined. */
		int Evaluate(const CSpectrum &measured, int numSteps = 1000);
		
		/** Evaluate the supplied spectrum using the solarReference found in 'm_window'
			@param measured - the spectrum for which to determine the shift & squeeze
			relative to the solarReference-spectrum found in 'm_window'

			The shift and squeeze between the measured spectrum and the solarReference-spectrum 
				will be determined for the pixel-range 'window.fitLow' and 'window.fitHigh'

			@return 0 if the fit succeeds and the shift & squeeze could be determined
			@return 1 if any error occurred. */
		int EvaluateShift(const CSpectrum &measured, double &shift, double &shiftError, double &squeeze, double &squeezeError);

		/** Returns the evaluation result for the last spectrum
			@return a reference to a 'CEvaluationResult' - data structure which holds the information from the last evaluation */
		const CEvaluationResult& GetEvaluationResult() const;

		/** Returns the polynomial that was fitted in the last evaluation */
		double *GetPolynomial();

		/** Sets the sky-spectrum to use */
		int SetSkySpectrum(const CSpectrum &spec);

		/** Removes the offset from the supplied spectrum */
		void RemoveOffset(double *spectrum, int sumChn, BOOL UV = TRUE);
	
		void GetFitRange(int &fitLow, int &fitHigh){
			fitLow = this->m_window.fitLow;
			fitHigh = this->m_window.fitHigh;
		}
	
		/** Assignment operator */
		CEvaluation &operator = (const CEvaluation &e2);

	private:

		/** The fit window, defines the parameters for the fit */
		CFitWindow m_window;

		/** The result */
		CEvaluationResult m_result;
		
		/** The sky spectrum to use in the evaluation */
		double			m_sky[MAX_SPECTRUM_LENGTH];
		CSpectrum		m_skySpectrum;

		/** Simple vector for holding the channel number information */
		CVector vXData;

		/** Simple function for initializing the vectors used in the evaluation */
		void InitializeVectors(int sumChn);

		// Prepares the spectra for evaluation
		void PrepareSpectra(double *sky, double *meas, const CFitWindow &window);

		// Prepares the spectra for evaluation
		void PrepareSpectra_HP_Div(double *sky, double *meas, const CFitWindow &window);

		// Prepares the spectra for evaluation
		void PrepareSpectra_HP_Sub(double *sky, double *meas, const CFitWindow &window);

		// Prepares the spectra for evaluation
		void PrepareSpectra_Poly(double *sky, double *meas, const CFitWindow &window);

		// The CReferenceSpectrumFunctions are used in the evaluation process to model
		// the reference spectra for the different species that are being fitted.
		CReferenceSpectrumFunction *ref[MAX_N_REFERENCES];
		CReferenceSpectrumFunction *solarSpec;

		// Creates the appropriate CReferenceSpectrumFunction for the fitting
		int CreateReferenceSpectrum();

	};
}
#endif // !defined(AFX_EVALUATION_H__DB88EE51_7ED0_4131_AE07_79F0F0C3106C__INCLUDED_)
