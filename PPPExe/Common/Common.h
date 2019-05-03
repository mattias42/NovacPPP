/**
  Common.h is a general header file for storing definitions and constants
      which are used throughout the whole program.
*/

#ifndef COMMON_H
#define COMMON_H

#include <PPPLib/PPPLib.h>
#include <PPPLib/CString.h>
#include <PPPLib/CList.h>
#include <PPPLib/Measurement.h>
#include <SpectralEvaluation/DateTime.h>
#include <SpectralEvaluation/Spectra/Spectrum.h>

#include "../Geometry/PlumeHeight.h"
#include "../Meteorology/WindField.h"
#include <cmath>

// ---------------------------------------------------------------
// ---------------- GLOBAL FUNCTIONS -----------------------------
// ---------------------------------------------------------------

/**Get system temperory folder
    *@param folderPath temperory system folder path*/
void GetSysTempFolder(novac::CString& folderPath);

/** A simple function to find out wheather a given file exists or not.
    @param - The filename (including path) to the file.
    @return 0 if the file does not exist.
    @return 1 if the file exist. */
bool IsExistingFile(const novac::CString &fileName);

/** Creates a directory structure according to the given path.
        @return 0 on success. */
int CreateDirectoryStructure(const novac::CString &path);

/** Checks if the supplied string is a valid serial-number of a spectrometer.
    @param serialNumber - the string that should be checked.
    @return 1 if the string is a valid serial number.
    @return 0 if the string is <b>not</b> a valid serial number. */
int IsSerialNumber(const novac::CString &serialNumber);



/** Appends an information / warning message to the logs */
void ShowMessage(const novac::CString &message);
void ShowMessage(const char message[]);
void ShowMessage(const novac::CString &message, novac::CString connectionID);

/** Appends an error message to the logs */
void ShowError(const novac::CString &message);
void ShowError(const char message[]);


/** Update the top line of list box */
void UpdateMessage(const novac::CString &message);

// ---------------------------------------------------------------
// ---------------- DEFINED CONSTANTS ----------------------------
// ---------------------------------------------------------------

// The list of instrument types available
enum INSTRUMENT_TYPE { INSTR_GOTHENBURG, INSTR_HEIDELBERG };


// The options for how to do the post-processing
enum PROCESSING_MODE {
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

class Common {

public:
    Common();

    // --------------------------------------------------------------------
    // ------------------------- FILE -------------------------------------
    // --------------------------------------------------------------------

    /** Get file size in bytes.
        @param - The file name (including path)
        @return - The file size (in bytes)
    */
    static long RetrieveFileSize(novac::CString& fileName);

    /** Compares two files to see if their contents are the same */
    static bool AreIdenticalFiles(const novac::CString &fileName1, const novac::CString &fileName2);

    /** If there's a file with the given input name, then it will be renamed to
        PATH\\FILENAME_creationDate_creationTime.FILEENDING */
    static bool ArchiveFile(const novac::CString &fileName);

    // --------------------------------------------------------------------
    // ------------------------ SYSTEM FUNCTIONS  -------------------------
    // --------------------------------------------------------------------

    // --------------------------------------------------------------------
    // ------------------------- PATH -------------------------------------
    // --------------------------------------------------------------------

    /** Take out the file name from a long path
            @param fileName path of the file	*/
    static void GetFileName(novac::CString& fileName);

    /** Take out the directory from a long path name.
        @param fileName - the complete path of the file */
    static void GetDirectory(novac::CString &fileName);

    /** Copies the file to the new file location */
    static void CopyFile(const novac::CString& oldName, const novac::CString& newName);

    /** m_exePath will be set to the path where the application resides,
        This is set once at application startup and should never be changed. */
    const novac::CString m_exePath;

    /** m_exeFileName will be set to the filename of the program.
        This is set once at application startup and should never be changed. */
    const novac::CString m_exeFileName;

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
    static void GetDateText(novac::CString &txt);

    /** pretty prints the current time into the string 'txt' */
    static void GetTimeText(novac::CString &txt);

    /** pretty prints the current time into the string 'txt' with the seperator*/
    static void GetTimeText(novac::CString &txt, char* seperator);

    /** pretty prints the current date and time into the string 'txt' */
    static void GetDateTimeText(novac::CString &txt);

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
    static void EquatorialCoordinates(double D, double &RA, double &dec, double &EQT);

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



    /** Sorts a list of strings in either ascending or descending order.
            The algorithm is based on MergeSort (~O(NlogN)). */
    static void Sort(novac::CList <novac::CString> &strings, bool files, bool ascending = true);
private:
    novac::CString m_string[3];

    // This is the string which is returned after a call to SimplifyString
    novac::CString m_simpleString;

    /** Merges the two lists 'list1' and 'list2' in a sorted way and stores
            the result in the output-list 'result' */
    static void MergeLists(const novac::CList <novac::CString> &list1, const novac::CList <novac::CString> &list2, novac::CList <novac::CString> &result, bool files, bool ascending = true);
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

    // ---------------------------- MISC ----------------------------------

    /** Guesses the name of the specie from the name of the reference file. */
    static void GuessSpecieName(const novac::CString &string, novac::CString &specie);



};// end of class Common

// --------------------------------------------------------------------
// --------------- TEMPLATE ARRAY FUNCTIONS ---------------------------
// --------------------------------------------------------------------

/** Searches for the maximum element in the array.
        @param pBuffer - The array in which to search for an element.
        @param bufLen - The length of the array.
        @return - The maximum value in the array */
template <class T> T Max(T *pBuffer, long bufLen) {
    T maxValue = pBuffer[0];
    for (long i = 1; i < bufLen; ++i) {
        if (pBuffer[i] > maxValue)
            maxValue = pBuffer[i];
    }
    return maxValue;
}

/** Searches for the minimum element in the array.
        @param pBuffer - The array in which to search for an element.
        @param bufLen - The length of the array.
        @return - The minimum value in the array */
template <class T> T Min(T *pBuffer, long bufLen) {
    T minValue = pBuffer[0];
    for (long i = 1; i < bufLen; i++) {
        if (pBuffer[i] < minValue)
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
template <class T> int FindInArray(T array[], T element, long nElements) {
    for (long i = 0; i < nElements; ++i) {
        if (array[i] == element)
            return i;
    }
    return -1;
}

/** A simple function to check if all elements in a 'bool' array are all same
        @param array - the array to investigate
        @param nElements - the length of the array
        @return true if all elements are equal, otherwise false */
template <class T> bool AllSame(T array[], long nElements) {
    if (nElements < 0)
        return false;
    if (nElements <= 1)
        return true;
    T firstValue = array[0];
    for (long i = 1; i < nElements; ++i) {
        if (array[i] != firstValue)
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
template <class T> bool FindNHighest(const T array[], long nElements, T output[], int N, int *indices = NULL) {
    for (int i = 0; i < N; ++i)
        output[i] = array[0]; // to get some initial value

    // loop through all elements in the array
    for (int i = 0; i < nElements; ++i) {

        // compare this element with all elements in the output array.
        for (int j = 0; j < N; ++j) {
            if (array[i] > output[j]) {
                // If we found a higher value, shift all other values down one step...
                for (int k = N - 1; k > j; --k) {
                    output[k] = output[k - 1];
                    if (indices)							indices[k] = indices[k - 1];
                }
                output[j] = array[i];
                if (indices)								indices[j] = i;
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
template <class T> bool FindNLowest(const T array[], long nElements, T output[], int N, int *indices = NULL) {
    for (int i = 0; i < N; ++i)
        output[i] = 1e16; // to get some initial value

    // loop through all elements in the array
    for (int i = 0; i < nElements; ++i) {

        // compare this element with all elements in the output array.
        for (int j = 0; j < N; ++j) {
            if (array[i] < output[j]) {
                // If we found a higher value, shift all other values down one step...
                for (int k = N - 1; k > j; --k) {
                    output[k] = output[k - 1];
                    if (indices)							indices[k] = indices[k - 1];
                }
                output[j] = array[i];
                if (indices)								indices[j] = i;
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
template <class T> double Average(T array[], long nElements) {
    if (nElements <= 0)
        return 0.0;

    double sum = 0;
    for (int k = 0; k < nElements; ++k) {
        sum += array[k];
    }
    return (sum / nElements);
}

/** This function calculates the variance of all the elements
        in the supplied array.
        @param array - the array of which to calculate the average value
        @param nElements - the length of the array. */
template <class T> double Variance(T array[], long nElements) {
    if (nElements <= 0)
        return 0.0;

    // First get the mean-value
    T mean = Average(array, nElements);

    double sum = 0;
    for (int k = 0; k < nElements; ++k) {
        sum += (array[k] - mean) * (array[k] - mean);
    }
    sum = sum / nElements;
    return sum;
}

/** This function calculates the standard deviation of all the elements
        in the supplied array.
        @param array - the array of which to calculate the average value
        @param nElements - the length of the array. */
template <class T> double Std(T array[], long nElements) {
    if (nElements <= 0)
        return 0.0;

    return std::sqrt(Variance(array, nElements));
}

#endif