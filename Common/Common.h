/**
  Common.h is a general header file for storing definitions and constants
      which are used throughout the whole program.
*/

#ifndef COMMON_H
#define COMMON_H

#define _CRTDBG_MAP_ALLOC
// #include <stdlib.h>
// #include <crtdbg.h>
// 
// #include <math.h>
// 
// #include <afxtempl.h>

#include "DateTime.h"

#include "../Geometry/PlumeHeight.h"
#include "../Meteorology/WindField.h"
#include "GPSData.h"
#include "../PlumeInScanProperty.h"

// definition used for storing the spectral data
typedef double SpecData;

// ---------------------------------------------------------------
// ---------------- GLOBAL FUNCTIONS -----------------------------
// ---------------------------------------------------------------

/**Get system temperory folder
	*@param folderPath temperory system folder path*/
void GetSysTempFolder(CString& folderPath);

/** A simple function to find out wheather a given file exists or not.
    @param - The filename (including path) to the file.
    @return 0 if the file does not exist.
    @return 1 if the file exist. */
int IsExistingFile(const CString &fileName);

/** Creates a directory structure according to the given path.
		@return 0 on success. */
int CreateDirectoryStructure(const CString &path);

/** Checks if the supplied string is a valid serial-number of a spectrometer. 
    @param serialNumber - the string that should be checked.
    @return 1 if the string is a valid serial number.
    @return 0 if the string is <b>not</b> a valid serial number. */
int IsSerialNumber(const CString &serialNumber);

/** Compares two strings without regard to case.
    @return 1 if the strings are equal. @return 0 if the strings are not equal. */
int Equals(const CString &str1, const CString &str2);

/** Compares at most 'nCharacters' of two strings without regard to case.
    @param nCharacters - The number of characters to compare
    @return 1 if the strings are equal. @return 0 if the strings are not equal. */
int Equals(const CString &str1, const CString &str2, size_t nCharacters);

/** Shows a message in the message box list in the main window */
void ShowMessage(const CString &message);
void ShowMessage(const TCHAR message[]);
void ShowMessage(const CString &message,CString connectionID);

/** Update the top line of list box */
void UpdateMessage(const CString &message);

// ---------------------------------------------------------------
// ---------------- DEFINED CONSTANTS ----------------------------
// ---------------------------------------------------------------

// defining if a function has failed or succeeded
enum RETURN_CODE { FAIL, SUCCESS };

// defining how to get the dark-spectrum
enum	DARK_SPEC_OPTION { MEASURE, MODEL_SOMETIMES, MODEL_ALWAYS, DARK_USER_SUPPLIED };

// defining how to use the dark-current and offset spectra
enum	DARK_MODEL_OPTION { MEASURED, USER_SUPPLIED };

// The list of instrument types available
const enum INSTRUMENT_TYPE {INSTR_GOTHENBURG, INSTR_HEIDELBERG};

// The various kinds of measurement modes that we have
const enum MEASUREMENT_MODE {
	MODE_UNKNOWN, 
	MODE_FLUX, 
	MODE_WINDSPEED, 
	MODE_STRATOSPHERE, 
	MODE_DIRECT_SUN, 
	MODE_COMPOSITION, 
	MODE_LUNAR, 
	MODE_TROPOSPHERE, 
	MODE_MAXDOAS
};

// The options for how to do the post-processing
const enum PROCESSING_MODE{
	PROCESSING_MODE_FLUX,
	PROCESSING_MODE_COMPOSITION,
	PROCESSING_MODE_STRATOSPHERE,
	PROCESSING_MODE_TROPOSPHERE,
	PROCESSING_MODE_GEOMETRY,
	PROCESSING_MODE_DUALBEAM
};

// The maximum number of references that can be fitted to a single spectrum
#define MAX_N_REFERENCES 10

// The maximum number of fit windows that can be handled at any single time
#define MAX_FIT_WINDOWS 5

// the maximum length of any single spectrum
#define MAX_SPECTRUM_LENGTH 4096

// the maximum number of channels that the program can handle
#define MAX_CHANNEL_NUM 8

//size of buffer to receive data
#define SIZEOFBUFFER 2048

/** Not initialized values are set to 'NOT_A_NUMBER' to indicate
	that a value is missing. 
	This can e.g. be the case if only the wind-direction (and not the wind speed)
		is known by an element in the database (which can be the case if a the
		wind direction was calculated by combining two scans).		
	*/
#define NOT_A_NUMBER -9999.0

// The options for the sky spectrum
enum SKY_OPTION {
	SKY_SCAN, 
	SKY_AVERAGE_OF_GOOD, 
	SKY_INDEX, 
	SKY_USER
};

// ----------------------------------------------------------------
// ---------------- MATHEMATICAL CONSTANTS ------------------------
// ----------------------------------------------------------------

// converts degrees to radians
#define DEGREETORAD 0.017453 

// converts radians to degrees
#define RADTODEGREE 57.295791

// a quite familiar constant
#define TWO_PI 6.28318
#define HALF_PI 1.5708
#ifndef M_PI
	#define M_PI 3.141592
#endif

// ----------------------------------------------------------------
// ------------ SIMPLE MATHEMATICAL FUNCTIONS  --------------------
// ----------------------------------------------------------------
// Commented out since this conflicts with the definition in cmath.h
// #define round(x) (x < 0 ? ceil((x)-0.5) : floor((x)+0.5))


// -----------------------------------------------------------------
// -------------------------- MESSAGES -----------------------------
// -----------------------------------------------------------------

// signals to the 'NovacMasterProgramView' - class that a string should be added to the message list
#define WM_SHOW_MESSAGE           WM_USER + 10

//signals to the 'NovacMasterProgramView' - class that a string should be updated to the message list's top string
#define WM_UPDATE_MESSAGE			WM_USER + 11

// signals to the 'NovacMasterProgramView' -class that a scan has been sucessfully evaluated
//  also signals to the reevaluation dialog that a spectrum has been sucessfully evaluated
#define WM_EVAL_SUCCESS		        WM_USER + 12

// signals to the 'NovacMasterProgramView' -class that a scan has falied to evaluate
#define WM_EVAL_FAILURE		        WM_USER + 13

// signals that something has been done
#define WM_DONE                   WM_USER + 14

// Signals that a 'cancel' button has been pressed
#define WM_CANCEL                 WM_USER + 15

// Signals a zooming event
#define WM_ZOOM						WM_USER + 33


// -------------------------------------------------------
// ---------------- CLASS COMMON.H -----------------------
// -------------------------------------------------------

/** The class <b>Common</b> contains misc. functions that we need but
	do not really fit anywhere else.
*/

class Common{

public:
	// --------------------------------------------------------------------
	// ------------------------- FILE -------------------------------------
	// --------------------------------------------------------------------

	/** Get file size in bytes.
		@param - The file name (including path)
		@return - The file size (in bytes)
	*/
	static long RetrieveFileSize(CString& fileName);	

	/** Compares two files to see if their contents are the same */
	static bool AreIdenticalFiles(const CString &fileName1, const CString &fileName2);

	/** If there's a file with the given input name, then it will be renamed to
		PATH\\FILENAME_creationDate_creationTime.FILEENDING */
	static bool ArchiveFile(const CString &fileName);

	// --------------------------------------------------------------------
	// ------------------------ SYSTEM FUNCTIONS  -------------------------
	// --------------------------------------------------------------------

	static bool FormatErrorCode(DWORD error, CString &string);

	// --------------------------------------------------------------------
	// ------------------------- PATH -------------------------------------
	// --------------------------------------------------------------------
	
	/** Take out the file name from a long path 
			@param fileName path of the file	*/
	static void GetFileName(CString& fileName);

	/** Take out the directory from a long path name.
	    @param fileName - the complete path of the file */
	static void GetDirectory(CString &fileName);

	/** Check if a process is running.
	    @param exeName - the name of the executable (e.g. "txzm.exe") 
	    @param pid - the function will search for processes with a 
	      process ID higher than 'pid' 
	    @return - processID of the found process, -1 if no process is found */
	static int CheckProcessExistance(CString& exeName, int pid = -1);

	/** Get all Process-ID's running with a given executable-name
	    @param exeName - the name of the executable (e.g. "txzm.exe") 
	    @param startPid - the function will search for processes with a 
	      process ID higher than 'pid' 
	    @param pIDs[1024] - will on successful return be filled with
	      all pID's found (first empty item will be -1)
	    @return - number of processID found */
	static int GetAllProcessIDs(CString& exeName, int pIDs[1024], int startPid = -1);
	
		/** Kill a process with the supplied processID.
	    @return TRUE if the process was killed.*/
	BOOL WINAPI KillProcess(IN DWORD dwProcessId);

	/** This function sets the name of a given thread. The name
		is useful for debugging purposes. */
	static void SetThreadName(DWORD dwThreadID, LPCTSTR szThreadName);

	/** Retrieves the current path, and the filename of the program. 
	    Result is stored in 'm_exePath', and 'm_exeFileName' */
	void GetExePath();

	/** m_exePath will be set to the current path after a call to 'GetExePath()' */
	CString   m_exePath;

	/** m_exeFileName will be set to the filename of the program after a call to 'GetExeFileName()' */
	CString   m_exeFileName;

	/** Opens a dialog window and lets the user browse for a file */
	bool BrowseForFile(TCHAR *filter, CString &fileName);

	/** Opens a dialog window and lets the user browse for a filename to save to */
	bool BrowseForFile_SaveAs(TCHAR *filter, CString &fileName);

	/** Opens a dialog window and lets the user browse for a directory.
			@return true if all is ok,
			@return false otherwise */
	bool BrowseForDirectory(CString &folderName);

	// --------------------------------------------------------------------
	// ---------------------- DATE & TIME ---------------------------------
	// --------------------------------------------------------------------
	/** Returns the current year */
	static int  GetYear();

	/** Returns the current month */
	static int   GetMonth();

	/** Returns the current day of the month */
	static int   GetDay();

	/** Returns the current hour */
	static int   GetHour();

	/** Returns the current minute */
	static int   GetMinute();

	/** Returns the current second */
	static int   GetSecond();

	/** Converts a time, given in seconds since midnight to hour, minutes and seconds */
	static void		ConvertToHMS(const int time, int &hours, int &minutes, int &seconds);

	/** pretty prints the current date into the string 'txt' */
	static void GetDateText(CString &txt);

	/** pretty prints the current time into the string 'txt' */
	static void GetTimeText(CString &txt);

	/** pretty prints the current time into the string 'txt' with the seperator*/
	static void GetTimeText(CString &txt, char* seperator);

	/** pretty prints the current date and time into the string 'txt' */
	static void GetDateTimeText(CString &txt);

	/** Converts the given time to local time using the information in the CGPSData.
			The date is stored as in the CSpectrumInfo-class with date[0] as 4-digit year,
			date[1] is month (1-12) and date[2] is day of month (1-31).
			@param date - the date in GMT
			@param hr - the hour in GMT
			@param gps - the GPS-information with latitude and longitude for the site
				where the local time is wanted
			@return - SUCCES if all is ok. Upon successful return, the parameters
				date and hr will be filled with the local time and date.
			NB!! daylight-saving time is not taken into account in these computations
			NB!! This calculation is only based on the distance to longitude=0. Thefore
				the resulting hour can have an error of up to +- 3 hours from the real local-time.
			*/
	static RETURN_CODE ConvertToLocalTime(unsigned short date[3], int &hr, CGPSData &gps);

	/** Decreases the given date by the given number of days.
			If nDays is negative, the date will be increased instead. */
	static RETURN_CODE DecreaseDate(unsigned short date[3], int nDays);

	/** Increases the given date by the given number of days.
			If nDays is negative, the date will be decreased instead. */
	static RETURN_CODE IncreaseDate(unsigned short date[3], int nDays);

	/** Takes a given year and month and returns the number of days in that month. 
			The month ranges from 1 to 12. Any illegal values in the month will return 0. */
	static int	DaysInMonth(int year, int month);

	/** Takes a given date and calculates the day of the year. 
			An illegal day will return 0. */
	static int	DayNr(const unsigned short day[3]);

	/** Takes a given date and calculates the day of the year. 
			An illegal day will return 0. */
	static int	DayNr(const CDateTime &day);

	/** Returns the Julian Day corresponding to the given date and time of day. */
	static double JulianDay(const CDateTime &utcTime);

	// --------------------------------------------------------------------
	// -------------------- SUN - FUNCTIONS -------------------------------
	// --------------------------------------------------------------------

	/** Retrieves the solar zenith angle (SZA) and the solar azimuth angle (SAZ)
			for the site specified by (lat, lon) and for the time given in gmtTime. 
			Note that the returned angles are in degrees and that the specified
			time _must_ be GMT-time. */
	static RETURN_CODE GetSunPosition(const CDateTime &gmtTime, double lat, double lon, double &SZA, double &SAZ);

	/** ???? 
		D  is the JulianDay counted from the 1st of January 2000 @ midnight
		Ra is the  Right Ascencion
		dec	is the declination
		EQT is the equation of time (in hours)
	*/
	static void EquatorialCoordinates(double D, double &RA, double &dec,double &EQT);

	/** ???
			lat - latitude in degrees
			H		- The hour angle, in degrees
			dec - The declination, degres
			elev - the returned elevation of the sun above the horizon
			azim - the returned azimuth angle of the sun, counted from south to west			*/
	static void HorizontalCoordinates(double lat, double H, double dec, double &elev, double &azim);

	/** Returns the hour angle given the longitude and equation of time. */
	static double GetHourAngle(double hr, double lon, double EqT);

	// --------------------------------------------------------------------
	// -------------------- GPS FUNCTIONS ---------------------------------
	// --------------------------------------------------------------------

	/* This function returns the distance in <b>meters</b> between the two points defined
		by (lat1,lon1) and (lat2, lon2). <b>All angles must be in degrees</b> */
	static double GPSDistance(double lat1, double lon1, double lat2, double lon2);

	/* This function returns the initial bearing (<b>degrees</b>) when travelling from
	  the point defined by (lat1, lon1) to the point (lat2, lon2). <b>All angles must be in degrees</b> */
	static double GPSBearing(double lat1, double lon1, double lat2, double lon2);

	/** This function calculates the latitude and longitude for point
			which is the distance 'dist' m and bearing 'az' degrees from 
			the point defied by 'lat1' and 'lon1' */
	void	CalculateDestination(double lat1, double lon1, double dist, double az, double &lat2, double &lon2);

	// --------------------------------------------------------------------
	// --------------------------- STRINGS --------------------------------
	// --------------------------------------------------------------------

	/** This reformats an UINT from the string table into a CString in a convenient way */
	const CString &GetString(const UINT uID);

	/** This function takes a string and simplifies it so that it is more easily readable 
			by a machine. changes made are: 
			1 - Spaces are replaced with '_' (underscore)
			2 - All characters are converted to lower-case
			3 - accents are removed ('ó' -> 'o') */
	CString &SimplifyString(const CString &in);

	/** This function takes a string and removes any 'special' (ASCII code < 32) 
			characters in it */
	static void CleanString(const CString &in, CString &out);
	static void CleanString(const char *in, CString &out);

	/** Sorts a list of strings in either ascending or descending order.
			The algorithm is based on MergeSort (~O(NlogN)). */
	static void Sort(CList <CString, CString&> &strings, bool files, bool ascending = true);
private:
	CString m_string[3];
	
	// This is the string which is returned after a call to SimplifyString
	CString m_simpleString;

	/** Merges the two lists 'list1' and 'list2' in a sorted way and stores
			the result in the output-list 'result' */
	static void MergeLists(const CList <CString, CString&> &list1, const CList <CString, CString&> &list2, CList <CString, CString&> &result, bool files, bool ascending = true);
public:

	// --------------------------------------------------------------------
	// -------------------- UNIT CONVERSION -------------------------------
	// --------------------------------------------------------------------

	// --------------------------------------------------------------------
	// -------------------- CALCULATING FLUX ------------------------------
	// --------------------------------------------------------------------
	
	/** Calculates the flux using the supplied data. Automatically decides which
			algorithm to use based on the given cone angle. */
	static double CalculateFlux(const double *scanAngle, const double *scanAngle2, const double *column, double offset, int nDataPoints, const Meteorology::CWindField &wind, const Geometry::CPlumeHeight &relativePlumeHeight, double compass, INSTRUMENT_TYPE type, double coneAngle = 90.0, double tilt = 0.0);

	/** Calculates the flux for the supplied data using the old algorithm */
	static double CalculateFlux_FlatFormula(const double *scanAngle, const double *column, double offset, int nDataPoints, const Meteorology::CWindField &wind, const Geometry::CPlumeHeight &relativePlumeHeight, double compass);

	/** Calculates the flux for the supplied data using the new algorithm */
	static double CalculateFlux_ConeFormula(const double *scanAngle, const double *column, double offset, int nDataPoints, const Meteorology::CWindField &wind, const Geometry::CPlumeHeight &relativePlumeHeight, double compass, double coneAngle, double tilt);

	/** Calculates the flux for the Heidelberg-instrument for the supplied data using the general algorithm */
	static double CalculateFlux_HeidelbergFormula(const double *scanAngle1, const double *scanAngle2, const double *column, double offset, int nDataPoints, const Meteorology::CWindField &wind, const Geometry::CPlumeHeight &relativePlumeHeight, double compass);

	// --------------------------------------------------------------------
	// ------------- CALCULATING OFFSET FOR A SCAN ------------------------
	// --------------------------------------------------------------------

	static double CalculateOffset(const double *columns, const bool *badEvaluation, long numPoints);

	// --------------------------------------------------------------------
	// -------------- CALCULATING IF WE SEE THE PLUME ---------------------
	// --------------------------------------------------------------------

	/** Finds the plume in the supplied scan. Return value is true if there is a plume, otherwise false 
			@param scanAngles - the scanAngles for the measurements.
			@param columns - the slant columns for the measurements. Must be from a normal scan
			@param columnErrors - the corresponding slant column errors for the measurement.
			@param badEvaluation - the result of the quality judgement of the measured columns, 
				badEvaluation[i] = true means a bad value
			@param numPoints - the number of points in the scan. Must also be the length 
				of the vectors 'columns', 'columnErrors', and 'badEvaluation'
			@param plumeCentre - Will on successful return be filled with the scan angle 
				which hits the centre of the plume
			@param plumeWidth - will on successful return be filled with the 
				estimated width of the plume (same unit as the scanAngles)
			@param plumeEdge_low - will on successful return be filled with the 
				lower edge  of the plume (same unit as the scanAngles)
			@param plumeEdge_high - will on successful return be filled with the 
				higher edge  of the plume (same unit as the scanAngles)	*/
	static bool FindPlume(const double *scanAngles, const double *phi, const double *columns, const double *columnErrors, const bool *badEvaluation, long numPoints, CPlumeInScanProperty &plumeProperties);

	/** Tries to calculate the completeness of the given scan.
			The completeness is 1.0 if the entire plume can be seen and 0.0 if the plume
				cannot be seen at all.
			Return value is true if there is a plume, otherwise false 
			@param scanAngles - the scanAngles for the measurements.
			@param columns - the slant columns for the measurements. Must be from a normal scan
			@param columnErrors - the corresponding slant column errors for the measurement.
			@param badEvaluation - the result of the quality judgement of the measured columns, 
				badEvaluation[i] = true means a bad value
			@param numPoints - the number of points in the scan. Must also be the length 
				of the vectors 'columns', 'columnErrors', and 'badEvaluation'
			@param completeness - Will on successful return be filled with the completeness of the plume */
	static bool CalculatePlumeCompleteness(const double *scanAngles, const double *phi, const double *columns, const double *columnErrors, const bool *badEvaluation, double offset, long numPoints, CPlumeInScanProperty &plumeProperties);

	// ---------------------------- MISC ----------------------------------

	/** Guesses the name of the specie from the name of the reference file. */
	static void GuessSpecieName(const CString &string, CString &specie);

	/** Retrieves the interlace step and the spectrometer channel (if a single) 
			that the spectrum originates from. 
			@return 0-7 if the spectrum originates from a single spectrometer channel
			@return -1 if the spectrum is a mix of two or more spectra
			@param channel - the channel number from the spectrum header
			@param interlaceSteps - the difference in pixel number between two 
				adjacent data points in the spectrum. E.g. 2 if the spectrum contains
				every other pixel from the spectrometer. */
	static int GetInterlaceSteps(int channel, int &interlaceSteps);

};// end of class Common

// --------------------------------------------------------------------
// --------------- TEMPLATE ARRAY FUNCTIONS ---------------------------
// --------------------------------------------------------------------

/** Searches for the maximum element in the array.
		@param pBuffer - The array in which to search for an element.
		@param bufLen - The length of the array.
		@return - The maximum value in the array */
template <class T> T Max(T *pBuffer, long bufLen){
	T maxValue = pBuffer[0];
	for(long i = 1; i < bufLen; ++i){
		if(pBuffer[i] > maxValue)
			maxValue = pBuffer[i];
	}
	return maxValue;
}

/** Searches for the minimum element in the array.
		@param pBuffer - The array in which to search for an element.
		@param bufLen - The length of the array.
		@return - The minimum value in the array */
template <class T> T Min(T *pBuffer, long bufLen){
	T minValue = pBuffer[0];
	for(long i = 1; i < bufLen; i++){
		if(pBuffer[i] < minValue)
			minValue = pBuffer[i];
	}
	return minValue;
}

/** A simple function to find an element in an array. The items in the array must be comparable (operator= must be defined).
		@param array - the array to search in.
		@param element - the element to find.
		@param nElements - the length of the array.
		@return the zero-based index into the array where the element is.
		@return -1 if the element was not found.	*/
template <class T> int FindInArray(T array[], T element, long nElements){
	for(long i = 0; i < nElements; ++i){
		if(array[i] == element)
			return i;
	}
	return -1;
}

/** A simple function to check if all elements in a 'bool' array are all same
		@param array - the array to investigate 
		@param nElements - the length of the array
		@return true if all elements are equal, otherwise false */
template <class T> bool AllSame(T array[], long nElements){
	if(nElements < 0)
		return false;
	if(nElements <= 1)
		return true;
	T firstValue = array[0];
	for(long i = 1; i < nElements; ++i){
		if(array[i] != firstValue)
			return false;
	}
	return true;
}

/** This function finds the 'N' highest values in the supplied array. 
		On successfull return the array 'output' will be filled with the N highest
		values in the input array, sorted in descending order.
		@param array - the array to look into
		@param nElements - the number of elements in the supplied array
		@param output - the output array, must be at least 'N'- elements long
		@param N - the number of values to take out. 	
		@param indices - if specified, this will on return the indices for the highest elements. Must have length 'N' */
template <class T> bool FindNHighest(const T array[], long nElements, T output[], int N, int *indices = NULL){
	for(int i = 0; i < N; ++i)
		output[i] = array[0]; // to get some initial value

	// loop through all elements in the array
	for(int i = 0; i < nElements; ++i){

		// compare this element with all elements in the output array.
		for(int j = 0; j < N; ++j){
			if(array[i] > output[j]){
				// If we found a higher value, shift all other values down one step...
				for(int k = N-1; k > j; --k){
					output[k] = output[k-1];
					if(indices)							indices[k] = indices[k-1];
				}
				output[j] = array[i];
				if(indices)								indices[j] = i;
				break;
			}
		}
	}
	return true;
}

/** This function finds the 'N' lowest values in the supplied array. 
		On successfull return the array 'output' will be filled with the N lowest
		values in the input array, sorted in ascending order.
		@param array - the array to look into
		@param nElements - the number of elements in the supplied array
		@param output - the output array, must be at least 'N'- elements long
		@param N - the number of values to take out. 	*/
template <class T> bool FindNLowest(const T array[], long nElements, T output[], int N, int *indices = NULL){
	for(int i = 0; i < N; ++i)
		output[i] = 1e16; // to get some initial value

	// loop through all elements in the array
	for(int i = 0; i < nElements; ++i){

		// compare this element with all elements in the output array.
		for(int j = 0; j < N; ++j){
			if(array[i] < output[j]){
				// If we found a higher value, shift all other values down one step...
				for(int k = N-1; k > j; --k){
					output[k] = output[k-1];
					if(indices)							indices[k] = indices[k-1];
				}
				output[j] = array[i];
				if(indices)								indices[j] = i;
				break;
			}
		}
	}
	return true;
}

/** This function calculates the average value of all the elements
		in the supplied array.
		@param array - the array of which to calculate the average value
		@param nElements - the length of the array. */
template <class T> double Average(T array[], long nElements){
	if(nElements <= 0)
		return 0.0;

	double sum = 0;
	for(int k = 0; k < nElements; ++k){
		sum += array[k];
	}
	return (sum / nElements);
}

/** This function calculates the variance of all the elements
		in the supplied array.
		@param array - the array of which to calculate the average value
		@param nElements - the length of the array. */
template <class T> double Variance(T array[], long nElements){
	if(nElements <= 0)
		return 0.0;

	// First get the mean-value
	T mean = Average(array, nElements);

	double sum = 0;
	for(int k = 0; k < nElements; ++k){
		sum += (array[k] - mean) * (array[k] - mean);
	}
	sum = sum / nElements;
	return sum;
}

/** This function calculates the standard deviation of all the elements
		in the supplied array.
		@param array - the array of which to calculate the average value
		@param nElements - the length of the array. */
template <class T> double Std(T array[], long nElements){
	if(nElements <= 0)
		return 0.0;

	return sqrt(Variance(array, nElements));
}

#endif