#pragma once

#include <SpectralEvaluation/DateTime.h>
#include "Geometry/GeometryCalculator.h"
#include "Meteorology/WindDataBase.h"
#include "Geometry/PlumeDataBase.h"
#include "Flux/FluxResult.h"
#include "Evaluation/ExtendedScanResult.h"
#include <PPPLib/CList.h>
#include <PPPLib/CString.h>

/** The class <b>CPostProcessing</b> is the main class in the NovacPPP
	This is where all the processing takes place (or at least the control
	of the processing). 
	See the functions 'DoPostProcessing_Flux', 'DoPostProcessing_Strat'
	and 'DoPostProcessing_Geometry'
*/

namespace Evaluation {
    class CReferenceFile;
}

class CPostProcessing
{
public:
	CPostProcessing(void);
	~CPostProcessing(void);

	// ----------------------------------------------------------------------
	// ---------------------- PUBLIC DATA -----------------------------------
	// ----------------------------------------------------------------------
	
	/** This is the directory of the executable. */
	novac::CString m_exePath;

	// ----------------------------------------------------------------------
	// --------------------- PUBLIC METHODS ---------------------------------
	// ----------------------------------------------------------------------

	/** Performs an post processing of the data in order to extract
		good flux data */
	void DoPostProcessing_Flux();

	/** Performs an post processing of the data in order to extract
		good stratospheric data */
	void DoPostProcessing_Strat();
	
	/** Performs an post processing of already evaluated data in order
		to generate plume heights and wind directions */
	void DoPostProcessing_Geometry();
protected:

	// ----------------------------------------------------------------------
	// ---------------------- PRIVATE DATA ----------------------------------
	// ----------------------------------------------------------------------
	
	/** The database of wind-fields to use for the flux calculations */
	Meteorology::CWindDataBase m_windDataBase;

	/** The database of plume-heights to use for the flux calculations */
	Geometry::CPlumeDataBase m_plumeDataBase;



	// ----------------------------------------------------------------------
	// --------------------- PRIVATE METHODS --------------------------------
	// ----------------------------------------------------------------------


	/** Prepares for the post-processing by first making sure that
		all settings found in the configuration are ok and that
		they make sense. 
		@return 0 if all settings are ok and we should continue, 
			otherwise non-zero*/
	int CheckSettings();

	/** Prepares for the evaluation of the spectra 
		by reading in all the reference files that are
		needed.
		@return 0 on success otherwise non-zero */
	int PrepareEvaluation();

	/** Prepares for the flux calculations by reading in the relevant
		wind-field file.
		@return 0 on success, otherwise non-zero */
	int ReadWindField();
	
	/** Prepares for the flux calculation by setting up a reasonable
		set of plume heights. This could also read in a set from file...?
		@return 0 on success, otherwese non-zero */
	int PreparePlumeHeights();
	
	/** Scans through the given directory in search for .pak-files
		@param path - the directory (on the local computer) where 
			to search for files
		@param fileList - will be appended with the path's and 
		file-names of the found .pak-files */
	void CheckForSpectraInDir(const novac::CString &path, 
		novac::CList <novac::CString, novac::CString&> &fileList);

	/** Scans through the FTP-server (using the IP,username and password
		found in g_userSettings) in search for .pak-files
		The files will be downloaded to the local computer (to the 
		temporary directory) and the returned path's will be pointing
		there.

		@param fileList - will be appended with the path's and 
		file-names of the found .pak-files (these will be in the TEMP directory) */
	void CheckForSpectraOnFTPServer(novac::CList <novac::CString, novac::CString&> &fileList);
	
	/** Runs through the supplied list of .pak-files and evaluates 
		each one using the setups found in the global settings. 
		@param pakFileList - the list of pak-files to evaluate.
		@param evalLogFiles - will on successful return be filled 
			with the path's and filenames of each evaluation log 
			file generated and the properties of each scan.
		*/
	void EvaluateScans(novac::CList <novac::CString, novac::CString &> &pakFileList,
		novac::CList <Evaluation::CExtendedScanResult, Evaluation::CExtendedScanResult &> &evalLogFiles);
	
	/** Runs through the supplied list of evaluation - logs and performs
		geometry calculations on the ones which does match. The results
		are returned in the list geometryResults. 
		@param evalLogs - list of CExtendedScanResult, each holding the full path and filename 
			of an evaluation-log file that should be considered for geometrical 
			calculations and the properties of the scan (plume centre position etc)
		@param geometryResults - will on successfull return be filled with the
			calculated plume heights and wind-directions.
		*/
	void CalculateGeometries(novac::CList <Evaluation::CExtendedScanResult,
		Evaluation::CExtendedScanResult &> &evalLogs, novac::CList <Geometry::CGeometryResult*,
		Geometry::CGeometryResult*> &geometryResults);

	/** Writes each of the calculated geometry results to the GeometryLog file */
	void WriteCalculatedGeometriesToFile(
		novac::CList <Geometry::CGeometryResult*, Geometry::CGeometryResult*> &geometryResults);
	
	/** Inserts the calculated geometry results into the databases.
		The wind directions will be inserted into m_windDataBase
		The plume altitudes will be inserted into m_plumeDataBase */
	void InsertCalculatedGeometriesIntoDataBase(
		novac::CList <Geometry::CGeometryResult*, Geometry::CGeometryResult*> &geometryResults);


	/** Runs through the supplied list of evaluation - logs and performs
		AC-DC corrections on the derived columns. The results are not
		returned, instead the files are re-written with the updated 
		column values. 
		@param evalLogs - list of CExtendedScanResult, each holding the full path and filename 
			of an evaluation-log file that should be considered
		@param geometryResults - list of calculated geometrical results. These will
			be used to apply the radiative corrections to the columns. 
		@return - true if so large changes are made that the geometries would need to
			be re-calculated. Otherwise false.
		*/	
	bool ApplyACDCCorrections(
		novac::CList <Evaluation::CExtendedScanResult, Evaluation::CExtendedScanResult &> &evalLogs,
		novac::CList <Geometry::CGeometryResult*, Geometry::CGeometryResult*> &geometryResults);


	/** This calculates the wind speeds from the dual-beam measurements that has been made
		@param evalLogs - list of CExtendedScanResult, each holding the full path and filename 
			of an evaluation-log file. Only the measurements containing a 
			dual-beam measurement will be considered.
		The plume heights are taken from the database 'm_plumeDataBase' and the 
			results are written to the database 'm_windDataBase'
	*/
	void CalculateDualBeamWindSpeeds(novac::CList <Evaluation::CExtendedScanResult,
		Evaluation::CExtendedScanResult &> &evalLogs);

	/** Runs through the supplied list of evaluation-results and 
		calculates the flux for each scan. The resulting fluxes are written
		to a flux-log file in the output directory. 
		@param evalLogs - list of CStrings, each holding the full path and filename 
			of an evaluation-log file that should be considered for geometrical 
			calculations
		The wind speeds and wind directions will be taken from 'm_windDataBase'
		The plume heigths will be taken from 'm_plumeDataBase'
		*/
	void CalculateFluxes(novac::CList <Evaluation::CExtendedScanResult, Evaluation::CExtendedScanResult &> &evalLogs);
	
	
	/** Sorts the evaluation logs in order of increasing time 
		(this is mostly done since this speeds up the geometry calculations enormously) */
	void SortEvaluationLogs(novac::CList <Evaluation::CExtendedScanResult, Evaluation::CExtendedScanResult &> &evalLogs);

	/** Writes the calculated fluxes to the flux result file */
	void WriteFluxResult_XML(novac::CList <Flux::CFluxResult, Flux::CFluxResult &> &calculatedFluxes);
	void WriteFluxResult_Txt(novac::CList <Flux::CFluxResult, Flux::CFluxResult &> &calculatedFluxes);
	
	/** Takes care of uploading the result files to the FTP server */
	void UploadResultsToFTP();

    novac::CString GetAbsolutePathFromRelative(const novac::CString& path);

    /** Creates a reference file by convolving a high-res cross section with a slit-function and resamples it 
        to a given wavelength calibration. The instrument serial is provided since the result is 
        saved to a local file, for reference. */
    bool ConvolveReference(Evaluation::CReferenceFile& ref, const novac::CString& instrumentSerial);
};
