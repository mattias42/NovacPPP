// Evaluation.cpp: implementation of the CEvaluation class.
//
//////////////////////////////////////////////////////////////////////

#include "Evaluation.h"
#include <iostream>

// include all required fit objects
#include <PPPLib/SpectralEvaluation/Fit/ReferenceSpectrumFunction.h>
#include <PPPLib/SpectralEvaluation/Fit/SimpleDOASFunction.h>
#include <PPPLib/SpectralEvaluation/Fit/StandardMetricFunction.h>
#include <PPPLib/SpectralEvaluation/Fit/StandardFit.h>
#include <PPPLib/SpectralEvaluation/Fit/ExpFunction.h>
#include <PPPLib/SpectralEvaluation/Fit/LnFunction.h>
#include <PPPLib/SpectralEvaluation/Fit/PolynomialFunction.h>
#include <PPPLib/SpectralEvaluation/Fit/NegateFunction.h>
#include <PPPLib/SpectralEvaluation/Fit/MulFunction.h>
#include <PPPLib/SpectralEvaluation/Fit/DivFunction.h>
#include <PPPLib/SpectralEvaluation/Fit/GaussFunction.h>
#include <PPPLib/SpectralEvaluation/Fit/DiscreteFunction.h>
#include <PPPLib/SpectralEvaluation/Fit/DOASVector.h>
#include <PPPLib/SpectralEvaluation/Fit/NonlinearParameterFunction.h>

#include <PPPLib/CString.h>
#include "../Common/Common.h"

// use the MathFit namesapce, since all fit objects are contained in this namespace
using namespace MathFit;
// also use the Evaluation namespace, since all datastructures for saving the results are contained there
using namespace Evaluation;

using namespace std;


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

/** Sets up the evaluation */
CEvaluation::CEvaluation(const CFitWindow &window){
	this->m_window = window;

	for(int i = 0; i < MAX_N_REFERENCES; ++i)
		ref[i] = NULL;

	solarSpec = NULL;
	
	memset(m_sky, 0, MAX_SPECTRUM_LENGTH * sizeof(double));
	
    CreateXDataVector(MAX_SPECTRUM_LENGTH);
}

CEvaluation::~CEvaluation()
{
	for(int i = 0; i < MAX_N_REFERENCES; ++i){
		if(ref[i] != NULL){
			delete ref[i];
			ref[i] = NULL;
		}
	}
}

/** Evaluate using the parameters set in the local parameter 'm_window'
	and using the sky-spectrum which has been set by a previous call to
	'SetSkySpectrum'
	@return 0 if all is ok.
	@return 1 if any error occured, or if the window is not defined. */
int CEvaluation::Evaluate(const CSpectrum &measured, int numSteps){
	novac::CString message;
	int fitLow, fitHigh; // the limits for the DOAS fit
	int i; // iterator


	// Check so that the length of the spectra agree with each other
	if(m_window.specLength != measured.m_length)
		return(1);

	// Get the correct limits for the fit
	if(measured.m_info.m_startChannel != 0 || measured.m_length < m_window.specLength){
		// Partial spectra
		fitLow	= m_window.fitLow - measured.m_info.m_startChannel;
		fitHigh	= m_window.fitHigh - measured.m_info.m_startChannel;
		if(fitLow < 0 || fitHigh > measured.m_length)
			return 1;
	}else{
		fitLow	= m_window.fitLow;
		fitHigh	= m_window.fitHigh;
	}

	// Vectors to store the data
	CVector vMeas, vSky;

	// Make a local copy of the data
	double *measArray = (double *)calloc(measured.m_length, sizeof(double));
	memcpy(measArray, measured.m_data, measured.m_length*sizeof(double));

	//----------------------------------------------------------------
	// --------- prepare the spectrum for evaluation -----------------
	//----------------------------------------------------------------

	PrepareSpectra(m_sky, measArray, m_window);

	//----------------------------------------------------------------

	// Copy the measured spectrum to vMeas
	vMeas.Copy(measArray, m_window.specLength, 1);

	// To perform the fit we need to extract the wavelength (or pixel)
	//	information from the vXData-vector
	CVector vXSec(fitHigh - fitLow);
	vXSec.Copy(vXData.SubVector(fitLow, fitHigh - fitLow));

	////////////////////////////////////////////////////////////////////////////
	// now we start building the model function needed for fitting.
	//
	// First we create a function object that represents our measured spectrum. Since we do not
	// need any interpolation on the measured data its enough to use a CDiscreteFunction object.
	CDiscreteFunction dataTarget;

	// now set the data of the measured spectrum in regard to the wavelength information
	{
		auto temp = vXData.SubVector(measured.m_info.m_startChannel, m_window.specLength);
		dataTarget.SetData(temp, vMeas);
	}
	
	// since the DOAS model function consists of the sum of all reference spectra and a polynomial,
	// we first create a summation object
	CSimpleDOASFunction cRefSum;

	// now we add the required CReferenceSpectrumFunction objects that actually represent the 
	// reference spectra used in the DOAS model function
	for(i = 0; i < m_window.nRef; ++i){
		cRefSum.AddReference(*ref[i]); // <-- at last add the reference to the summation object
	}

	// create the additional polynomial with the correct order
	//	and add it to the summation object, too
	CPolynomialFunction cPoly(m_window.polyOrder);
	cRefSum.AddReference(cPoly);

	// the last step in the model function will be to define how the difference between the measured data and the modeled
	// data will be determined. In this case we will use the CStandardMetricFunction which actually just calculate the difference
	// between the measured data and the modeled data channel by channel. The fit will try to minimize these differences.
	// So we create the metric object and set the measured spectrum function object and the DOAS model function object as parameters
	CStandardMetricFunction cDiff(dataTarget, cRefSum);

	/////////////////////////////////////////////////////////////////
	// Now its time to create the fit object. The CStandardFit object will 
	// provide a combination of a linear Least Square Fit and a nonlinear Levenberg-Marquardt Fit, which
	// should be sufficient for most needs.
	CStandardFit cFirstFit(cDiff);

	// don't forget to the the already extracted fit range to the fit object!
	// without a valid fit range you'll get an exception.
	cFirstFit.SetFitRange(vXSec);

	// limit the number of fit iteration to 5000. This can still take a long time! More convinient values are
	// between 100 and 1000
	cFirstFit.GetNonlinearMinimizer().SetMaxFitSteps(numSteps);
	cFirstFit.GetNonlinearMinimizer().SetMinChiSquare(0.0001);

	try
	{
		// prepare everything for fitting
		cFirstFit.PrepareMinimize();

		// actually do the fitting
		if(!cFirstFit.Minimize()){
			message.Format("Fit Failed!");
			ShowMessage(message);
			free(measArray);
			return 1;
		}

		// finalize the fitting process. This will calculate the error measurements and other statistical stuff
		cFirstFit.FinishMinimize();

			// get the basic fit results
		m_result.m_stepNum    = (long)cFirstFit.GetFitSteps();
		m_result.m_chiSquare  = (double)cFirstFit.GetChiSquare();
        m_result.m_referenceResult.resize(m_window.nRef);

		for(int tmpInt = 0; tmpInt <= m_window.polyOrder; ++tmpInt)
			m_result.m_polynomial[tmpInt] = (double)cPoly.GetCoefficient(tmpInt);

			//// get residuum vector and expand it to a DOAS vector object. Do NOT assign the vector data to the new object!
			//// display some statistical stuff about the residual data
			CDOASVector vResiduum;
			vResiduum.Attach(cFirstFit.GetResiduum(), false);
			//m_residual.SetSize(vResiduum.GetSize());
			//m_residual.Zero();
			//m_residual.Add(vResiduum);

			m_result.m_delta = (double)vResiduum.Delta();

			// get the fitResult for the polynomial
			//CVector tmpVector;
			//tmpVector.SetSize(specLen);
			//cPoly.GetValues(vXData.SubVector(measured.startChannel, window.specLength), tmpVector);
			//m_fitResult[0].Set(tmpVector, specLen);

			// finally display the fit results for each reference spectrum including their appropriate error
			for(i = 0; i < m_window.nRef; i++)
			{
				m_result.m_referenceResult[i].m_specieName    = std::string(m_window.ref[i].m_specieName);
				m_result.m_referenceResult[i].m_column        = (double)ref[i]->GetModelParameter(CReferenceSpectrumFunction::CONCENTRATION);
				m_result.m_referenceResult[i].m_columnError   = (double)ref[i]->GetModelParameterError(CReferenceSpectrumFunction::CONCENTRATION);
				m_result.m_referenceResult[i].m_shift         = (double)ref[i]->GetModelParameter(CReferenceSpectrumFunction::SHIFT);
				m_result.m_referenceResult[i].m_shiftError    = (double)ref[i]->GetModelParameterError(CReferenceSpectrumFunction::SHIFT);
				m_result.m_referenceResult[i].m_squeeze       = (double)ref[i]->GetModelParameter(CReferenceSpectrumFunction::SQUEEZE);
				m_result.m_referenceResult[i].m_squeezeError  = (double)ref[i]->GetModelParameterError(CReferenceSpectrumFunction::SQUEEZE);

				//// get the final fit result
				//CVector tmpVector;
				//tmpVector.SetSize(specLen);
				//ref[i]->GetValues(vXData.SubVector(measured.startChannel, measured.specLength), tmpVector);
				//m_fitResult[i+1].Set(tmpVector, specLen);
			}

			// clean up the evaluation
			free(measArray);
			
			return 0;
		}
		catch(CFitException e)
		{
			// in case that something went wrong, display the error to the user.
			// normally you will get error in two cases:
			//
			// 1. You forgot to set a valid fit range before you start fitting
			//
			// 2. A matrix inversion failed for some reason inside the fitting loop. Matrix inversions
			//    normally fail when there are linear dependecies in the matrix respectrively you have linear
			//    dependencies in your reference spectrum. Eg. you tried to fit the same reference spectrum twice at once.

		//  e.ReportError();
		//	std::cout << "Failed: " << ++iFalseCount << std::endl;
		//	std::cout << "Steps: " << cFirstFit.GetNonlinearMinimizer().GetFitSteps() << " - Chi: " << cFirstFit.GetNonlinearMinimizer().GetChiSquare() << std::endl;

			message.Format("A Fit Exception has occurred. Are the reference files OK?");
			ShowMessage(message);

			// clean up the evaluation
			free(measArray);

			return (1);
		}
}
		
/** Evaluate the supplied spectrum using the solarReference found in 'window'
	@param measured - the spectrum for which to determine the shift & squeeze
	relative to the solarReference-spectrum found in 'm_window'

	The shift and squeeze between the measured spectrum and the solarReference-spectrum 
		will be determined for the pixel-range 'window.fitLow' and 'window.fitHigh'

	@return 0 if the fit succeeds and the shift & squeeze could be determined
	@return 1 if any error occurred. */
int CEvaluation::EvaluateShift(const CSpectrum &measured, double &shift, double &shiftError, double &squeeze, double &squeezeError){
	novac::CString message;
	int i;
	CVector vMeas;
	CVector yValues;
	int fitLow	= m_window.fitLow;
	int fitHigh	= m_window.fitHigh;

	// Check so that the length of the spectra agree with each other
	if(m_window.specLength != measured.m_length)
		return(1);

	// Check that we have a solar-spectrum to check against
	if(m_window.fraunhoferRef.m_path.size() < 6)
		return 1;

	if(measured.m_info.m_startChannel != 0 || measured.m_length < m_window.specLength){
		// Partial spectra
		fitLow	= m_window.fitLow  - measured.m_info.m_startChannel;
		fitHigh	= m_window.fitHigh - measured.m_info.m_startChannel;
		if(fitLow < 0 || fitHigh > measured.m_length)
			return 1;
	}else{
		fitLow	= m_window.fitLow;
		fitHigh	= m_window.fitHigh;
	}

	// initialize the solar-spectrum function
	solarSpec = new CReferenceSpectrumFunction();

	// Make a local copy of the data
	double *measArray = (double *)calloc(measured.m_length, sizeof(double));
	memcpy(measArray, measured.m_data, measured.m_length*sizeof(double));

	//----------------------------------------------------------------
	// --------- prepare the spectrum for evaluation -----------------
	//----------------------------------------------------------------

	RemoveOffset(measArray, m_window.specLength, m_window.UV);
	if(m_window.fitType == FIT_HP_DIV || m_window.fitType == FIT_HP_SUB){
		HighPassBinomial(measArray,	m_window.specLength,	500);
	}
	Log(measArray,m_window.specLength);
	
	if(m_window.fitType == FIT_POLY){
		for(int j = 0; j < m_window.specLength; ++j)
			measArray[j] *= -1.0;
	}

	// --------- also prepare the solar-spectrum for evaluation -----------------
	CVector localSolarSpectrumData;
	localSolarSpectrumData.SetSize(m_window.fraunhoferRef.m_data->GetSize());
	for(int j = 0; j < m_window.specLength; ++j){
		localSolarSpectrumData.SetAt(j, m_window.fraunhoferRef.m_data->GetAt(j));
	}

	//----------------------------------------------------------------

	// Copy the measured spectrum to vMeas
	vMeas.Copy(measArray,m_window.specLength,1);

	// To perform the fit we need to extract the wavelength (or pixel)
	//	information from the vXData-vector
	CVector vXSec(fitHigh - fitLow);
	vXSec.Copy(vXData.SubVector(fitLow, fitHigh - fitLow));

	////////////////////////////////////////////////////////////////////////////
	// now we start building the model function needed for fitting.
	//
	// First we create a function object that represents our measured spectrum. Since we do not
	// need any interpolation on the measured data its enough to use a CDiscreteFunction object.
	CDiscreteFunction dataTarget;

	// now set the data of the measured spectrum in regard to the wavelength information
	{
		auto temp = vXData.SubVector(measured.m_info.m_startChannel, m_window.specLength);
		dataTarget.SetData(temp, vMeas);
	}
	
	// since the DOAS model function consists of the sum of all reference spectra and a polynomial,
	// we first create a summation object
	CSimpleDOASFunction cRefSum;

	// reset all reference's parameters
	solarSpec->ResetLinearParameter();
	solarSpec->ResetNonlinearParameter();

	// enable amplitude normalization. This should normally be done in order to avoid numerical
	// problems during fitting.
	solarSpec->SetNormalize(true);

	// set the spectral data of the reference spectrum to the object. This also causes an internal
	// transformation of the spectral data into a B-Spline that will be used to interpolate the 
	// reference spectrum during shift and squeeze operations
	auto tempXVec = vXData.SubVector(0, localSolarSpectrumData.GetSize());
	if(!solarSpec->SetData(tempXVec, localSolarSpectrumData))
	{
		Error0("Error initializing spline object!");
		free(measArray);
		delete solarSpec;
		return(1);
	}

	// Chech the options for the column value
	if(m_window.fitType == FIT_POLY)
		solarSpec->FixParameter(CReferenceSpectrumFunction::CONCENTRATION, -1.0 * solarSpec->GetAmplitudeScale());
	else
		solarSpec->FixParameter(CReferenceSpectrumFunction::CONCENTRATION, 1.0 * solarSpec->GetAmplitudeScale());

	// Free the shift
	//solarSpec->SetParameterLimits(CReferenceSpectrumFunction::SHIFT,	(TFitData)-6.0, (TFitData)6.0, (TFitData)1e25);
	//solarSpec->FixParameter(CReferenceSpectrumFunction::SHIFT,	(TFitData)1.4);

	// Fix the squeeze
	solarSpec->FixParameter(CReferenceSpectrumFunction::SQUEEZE, (TFitData)1.0);

	// Add the solar-reference to the fit
	cRefSum.AddReference(*solarSpec); // <-- at last add the reference to the summation object

	// Link the shifts of the 'normal' cross sections to the shift of the solar spectrum
	for(i = 0; i < m_window.nRef; ++i){
		// Link the shift and squeeze to the solar-reference
		solarSpec->LinkParameter(CReferenceSpectrumFunction::SHIFT,	  *ref[i], CReferenceSpectrumFunction::SHIFT);
		solarSpec->LinkParameter(CReferenceSpectrumFunction::SQUEEZE, *ref[i], CReferenceSpectrumFunction::SQUEEZE);
		
		cRefSum.AddReference(*ref[i]); // <-- at last add the reference to the summation object
	}

	// create the additional polynomial with the correct order
	//	and add it to the summation object, too
	CPolynomialFunction cPoly(2);
	cRefSum.AddReference(cPoly);

	// the last step in the model function will be to define how the difference between the measured data and the modeled
	// data will be determined. In this case we will use the CStandardMetricFunction which actually just calculate the difference
	// between the measured data and the modeled data channel by channel. The fit will try to minimize these differences.
	// So we create the metric object and set the measured spectrum function object and the DOAS model function object as parameters
	CStandardMetricFunction cDiff(dataTarget, cRefSum);

	/////////////////////////////////////////////////////////////////
	// Now its time to create the fit object. The CStandardFit object will 
	// provide a combination of a linear Least Square Fit and a nonlinear Levenberg-Marquardt Fit, which
	// should be sufficient for most needs.
	CStandardFit cFirstFit(cDiff);

	// don't forget to the the already extracted fit range to the fit object!
	// without a valid fit range you'll get an exception.
	cFirstFit.SetFitRange(vXSec);

	// limit the number of fit iteration to 5000.
	cFirstFit.GetNonlinearMinimizer().SetMaxFitSteps(5000);
	cFirstFit.GetNonlinearMinimizer().SetMinChiSquare(0.0001);

	try
	{
		// prepare everything for fitting
		cFirstFit.PrepareMinimize();

		// actually do the fitting
		if(!cFirstFit.Minimize()){
			message.Format("Fit Failed!");
			ShowMessage(message);
			free(measArray);
			delete solarSpec;
			return 1;
		}

		// finalize the fitting process. This will calculate the error measurements and other statistical stuff
		cFirstFit.FinishMinimize();

		// get the basic fit results
		//long stepNum				= (long)cFirstFit.GetFitSteps();
		// double chiSquare			= (double)cFirstFit.GetChiSquare();
		// unsigned long speciesNum	= (unsigned long)m_window.nRef;

		//// get residuum vector and expand it to a DOAS vector object. Do NOT assign the vector data to the new object!
		//// display some statistical stuff about the residual data
		CDOASVector vResiduum;
		vResiduum.Attach(cFirstFit.GetResiduum(), false);

		m_result.m_delta = (double)vResiduum.Delta();

		// finally get the fit-result
		// double column       = (double)solarSpec->GetModelParameter(CReferenceSpectrumFunction::CONCENTRATION);
		// double columnError  = (double)solarSpec->GetModelParameterError(CReferenceSpectrumFunction::CONCENTRATION);
		shift               = (double)solarSpec->GetModelParameter(CReferenceSpectrumFunction::SHIFT);
		shiftError          = (double)solarSpec->GetModelParameterError(CReferenceSpectrumFunction::SHIFT);
		squeeze             = (double)solarSpec->GetModelParameter(CReferenceSpectrumFunction::SQUEEZE);
		squeezeError        = (double)solarSpec->GetModelParameterError(CReferenceSpectrumFunction::SQUEEZE);

		// clean up the evaluation
		free(measArray);
		delete solarSpec;
		
		return 0;
	}
	catch(CFitException e)
	{
			// in case that something went wrong, display the error to the user.
			// normally you will get error in two cases:
			//
			// 1. You forgot to set a valid fit range before you start fitting
			//
			// 2. A matrix inversion failed for some reason inside the fitting loop. Matrix inversions
			//    normally fail when there are linear dependecies in the matrix respectrively you have linear
			//    dependencies in your reference spectrum. Eg. you tried to fit the same reference spectrum twice at once.

			//  e.ReportError();
			//	std::cout << "Failed: " << ++iFalseCount << std::endl;
			//	std::cout << "Steps: " << cFirstFit.GetNonlinearMinimizer().GetFitSteps() << " - Chi: " << cFirstFit.GetNonlinearMinimizer().GetChiSquare() << std::endl;

			message.Format("A Fit Exception has occurred. Are the reference files OK?");
			ShowMessage(message);

			// clean up the evaluation
			free(measArray);
			delete solarSpec;

			return (1);
		}
}

/** Returns the evaluation result for the last spectrum
  @return a reference to the evaluation result */
const CEvaluationResult& CEvaluation::GetEvaluationResult() const{
	return m_result;
}


/** Returns the polynomial that was fitted to the last evaluation result */
double *CEvaluation::GetPolynomial(){
	return m_result.m_polynomial;
}

/** Assignment operator */
CEvaluation &CEvaluation::operator = (const CEvaluation &e2){

	// copy the fit window
	m_window					= e2.m_window;
	
	// copy the sky-spectrum
	this->SetSkySpectrum(e2.m_skySpectrum);

	return *this;
}

/** Sets the sky-spectrum to use */
int CEvaluation::SetSkySpectrum(const CSpectrum &spec){
	
	// make sure that these make sense
	if(m_window.specLength != spec.m_length){
		return 1;
	}

	// set the data
	m_skySpectrum = spec;
	memcpy(m_sky, m_skySpectrum.m_data, m_skySpectrum.m_length * sizeof(double));

	// --------- prepare the sky spectrum for evaluation ---------
	// Remove any remaining offset of the sky-spectrum
	RemoveOffset(m_sky, m_skySpectrum.m_length, m_window.UV);

	// High-pass filter the sky-spectrum
	if(m_window.fitType == FIT_HP_SUB)
		HighPassBinomial(m_sky, m_skySpectrum.m_length, 500);

	// Logaritmate the sky-spectrum
	if(m_window.fitType != FIT_HP_DIV)
		Log(m_sky, m_skySpectrum.m_length);	
	
	
	// ----------- initialize the reference spectrum functions ------------
	for(int k = 0; k < MAX_N_REFERENCES; ++k){
		if(ref[k] != NULL){
			delete ref[k];
		}
		ref[k] = new CReferenceSpectrumFunction();
	}

	this->CreateReferenceSpectra();
	
	return 0;
}
