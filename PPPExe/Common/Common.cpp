#include "Common.h"

#include <algorithm>
#include <iostream>
#include <time.h>

#include <Poco/File.h>
#include <Poco/DirectoryIterator.h>
#include <Poco/DateTime.h>
#include <Poco/Message.h>
#include <Poco/Logger.h>

// include the global settings
#include <PPPLib/VolcanoInfo.h>

extern novac::CVolcanoInfo g_volcanoes;					// <-- the list of volcanoes

extern std::string s_exePath;
extern std::string s_exeFileName;

#undef min
#undef max

void GetSysTempFolder(novac::CString& folderPath)
{
	folderPath = novac::CString(Poco::Path::temp());
}

bool IsExistingFile(const novac::CString &fileName) {
	Poco::File file(fileName.c_str());
	return file.exists();
}

int CreateDirectoryStructure(const novac::CString &path)
{
	try
	{
		Poco::File directory(path.c_str());
		directory.createDirectories();

		if (directory.exists()) {
			return 0;
		}
		else {
			return 1; // error
		}
	}
	catch(std::exception& e)
	{
		novac::CString message = "Failed to create directory: ";
		message.AppendFormat("%s", e.what());
		ShowMessage(message);
		return 1;
	}
}


void UpdateMessage(const novac::CString &message) {
	Poco::Logger& log = Poco::Logger::get("NovacPPP");
	log.information(message.std_str());
}

void ShowMessage(const novac::CString &message) {
	Poco::Logger& log = Poco::Logger::get("NovacPPP");
	log.information(message.std_str());
}
void ShowMessage(const novac::CString &message, novac::CString connectionID) {
	novac::CString msg;

	msg.Format("<%s> : %s", (const char*)connectionID, (const char*)message);

	Poco::Logger& log = Poco::Logger::get("NovacPPP");
	log.information(msg.std_str());
}

void ShowMessage(const char message[]) {
	novac::CString msg;
	msg.Format("%s", message);
	ShowMessage(msg);
}

void ShowError(const novac::CString &message)
{
	Poco::Logger& log = Poco::Logger::get("NovacPPP");
	log.fatal(message.std_str());
}
void ShowError(const char message[])
{
	novac::CString msg;
	msg.Format("%s", message);
	ShowError(msg);
}

Common::Common()
	:m_exePath(s_exePath), m_exeFileName(s_exeFileName)
{

}

/** Calculate the distance (in meters) between the two points (lat1, lon1) and
	(lat2, lon2). All latitudes and longitudes should be in degrees. */
double Common::GPSDistance(double lat1, double lon1, double lat2, double lon2) {
	const double R_Earth = 6367000; // radius of the earth
	lat1 = lat1*DEGREETORAD;
	lat2 = lat2*DEGREETORAD;
	lon1 = lon1*DEGREETORAD;
	lon2 = lon2*DEGREETORAD;

	double dLon = lon2 - lon1;
	double dLat = lat2 - lat1;

	if ((dLon == 0) && (dLat == 0))
		return 0;

	double a = std::pow((std::sin(dLat / 2.0)), 2.0) + std::cos(lat1) * std::cos(lat2) * std::pow((std::sin(dLon / 2.0)), 2.0);
	double c = 2 * std::asin(std::min(1.0, std::sqrt(a)));
	double distance = R_Earth * c;

	return distance;
}

/**count the angle from wind to north,also the plume direction compared with north
* the direction is from plume center to the source of the plume
* return degree value
*@lat1 - the latitude of beginning point or plume source, rad
*@lon1 - the longitude of beginning point orplume source,rad
*@lat2   - the latitude of ending point or plume center,rad
*@lon2   - the longitude of ending point or plume center,rad
*/
double Common::GPSBearing(double lat1, double lon1, double lat2, double lon2)
{
	lat1 = lat1*DEGREETORAD;
	lat2 = lat2*DEGREETORAD;
	lon1 = lon1*DEGREETORAD;
	lon2 = lon2*DEGREETORAD;
	double tmpAngle;
	double dLat = lat1 - lat2;
	double dLon = lon1 - lon2;

	if ((dLon == 0) && (dLat == 0))
		return 0;

	tmpAngle = atan2(-sin(dLon)*cos(lat2),
		cos(lat1)*sin(lat2) - sin(lat1)*cos(lat2)*cos(dLon));

	/*  	tmpAngle = atan2(lon1*cos(lat1)-lon2*cos(lat2), lat1-lat2); */

	if (tmpAngle < 0)
	{
		tmpAngle = TWO_PI + tmpAngle;
	}

	tmpAngle = RADTODEGREE*tmpAngle;
	return tmpAngle;
}

/** This function calculates the latitude and longitude for a point
		which is the distance 'dist' m and bearing 'az' degrees from
		the point defied by 'lat1' and 'lon1' */
void Common::CalculateDestination(double lat1, double lon1, double dist, double az, double &lat2, double &lon2) {
	const double R_Earth = 6367000; // radius of the earth

	double dR = dist / R_Earth;

	// convert to radians
	lat1 = lat1 * DEGREETORAD;
	lon1 = lon1 * DEGREETORAD;
	az = az	  * DEGREETORAD;

	// calculate the second point
	lat2 = asin(sin(lat1)*cos(dR) + cos(lat1)*sin(dR)*cos(az));

	lon2 = lon1 + atan2(sin(az)*sin(dR)*cos(lat1), cos(dR) - sin(lat1)*sin(lat2));

	// convert back to degrees
	lat2 = lat2 * RADTODEGREE;
	lon2 = lon2 * RADTODEGREE;
}

int IsSerialNumber(const novac::CString &serialNumber) {
	return (strlen(serialNumber) > 0);
}

/* pretty prints the current date into the string 'txt' */
void Common::GetDateText(novac::CString &txt)
{
	struct tm *tim;
	time_t t;

	time(&t);
	tim = localtime(&t);
	txt.Format("%04d.%02d.%02d", tim->tm_year + 1900, tim->tm_mon + 1, tim->tm_mday);
}
int Common::GetHour()
{
	struct tm *tim;
	time_t t;
	time(&t);
	tim = localtime(&t);
	return tim->tm_hour;
}
int Common::GetMinute()
{
	struct tm *tim;
	time_t t;
	time(&t);
	tim = localtime(&t);
	return tim->tm_min;
}
int Common::GetSecond()
{
	struct tm *tim;
	time_t t;
	time(&t);
	tim = localtime(&t);
	return tim->tm_sec;
}

/** Converts a time, given in seconds since midnight to hour, minutes and seconds */
void Common::ConvertToHMS(const int time, int &hours, int &minutes, int &seconds) {
	hours = (int)(time / 3600);
	minutes = (time - hours * 3600) / 60;
	seconds = time % 60;
}

/* pretty prints the current time into the string 'txt' */
void Common::GetTimeText(novac::CString &txt)
{
	struct tm *tim;
	time_t t;
	time(&t);
	tim = localtime(&t);
	txt.Format("%02d:%02d:%02d", tim->tm_hour, tim->tm_min, tim->tm_sec);
}
/* pretty prints the current time into the string 'txt' */
void Common::GetTimeText(novac::CString &txt, char* seperator)
{
	struct tm *tim;
	time_t t;
	time(&t);
	tim = localtime(&t);
	txt.Format("%02d%s%02d%s%02d", tim->tm_hour, seperator, tim->tm_min, seperator, tim->tm_sec);
}
/* pretty prints the current date and time into the string 'txt' */
void Common::GetDateTimeText(novac::CString &txt)
{
	struct tm *tim;
	time_t t;
	time(&t);
	tim = localtime(&t);
	txt.Format("%04d.%02d.%02d  %02d:%02d:%02d", tim->tm_year + 1900, tim->tm_mon + 1, tim->tm_mday, tim->tm_hour, tim->tm_min, tim->tm_sec);
}

/** Returns the current year */
int Common::GetYear()
{
	struct tm *tim;
	time_t t;
	time(&t);
	tim = localtime(&t);
	return (tim->tm_year + 1900);
}

/** Returns the current month */
int Common::GetMonth()
{
	struct tm *tim;
	time_t t;
	time(&t);
	tim = localtime(&t);
	return (tim->tm_mon + 1);
}

/** Returns the current day of the month */
int Common::GetDay()
{
	struct tm *tim;
	time_t t;
	time(&t);
	tim = localtime(&t);
	return (tim->tm_mday);
}

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
RETURN_CODE Common::ConvertToLocalTime(unsigned short date[3], int &hr, CGPSData &gps) {
	// Direction is -1 if the local time is after the GMT-time, otherwise positive.
	int		 direction = (gps.Longitude() > 0) ? 1 : -1;

	// The absolute number of degrees from Greenwitch
	double degreesToGreenwitch = fabs(gps.Longitude());

	// The number of hours that differ between the local time and GMT
	int hoursToGreenwitch = (int)round((12.0 / 180.0) * degreesToGreenwitch);

	// Change the hour
	hr += direction * hoursToGreenwitch;

	// If necessary, change the date.
	if (hr < 0) {
		RETURN_CODE ret = DecreaseDate(date, 1 + (-hr / 24));
		hr = 24 - (-hr) % 24;
		return ret;
	}
	else if (hr >= 24) {
		RETURN_CODE ret = IncreaseDate(date, hr / 24);
		hr %= 24;
		return ret;
	}

	// all done, return!
	return SUCCESS;
}

/** Decreases the given date by the given number of days.
		If nDays is negative, the date will be increased instead. */
RETURN_CODE Common::DecreaseDate(unsigned short date[3], int nDays) {
	// Check for illegal dates
	if (date[1] < 1 || date[1] > 12)
		return FAIL;
	if (date[2] < 1 || date[2] > novac::DaysInMonth(date[0], date[1]))
		return FAIL;

	// If we should not change the date, return without doing anything
	if (nDays == 0)
		return SUCCESS;

	// Check if we instead should increase the date
	if (nDays < 0)
		IncreaseDate(date, -nDays);

	unsigned short *day = &date[2];
	unsigned short *month = &date[1];
	unsigned short *year = &date[0];

	// reduce the day of the month
	*day -= nDays;

	// Check the day of the month
	while (*day < 1) { // <-- if we've passed to the month before
		--*month; // go the month before

		while (*month < 1) { // <-- if we've passed to the year before
			*year -= 1;		// go to the year before
			*month += 12;
		}

		*day += novac::DaysInMonth(*year, *month);
	}
	// Check the month 
	while (*month < 1) {
		*year -= 1; // go to the year before
		*month += 12;
	}

	return SUCCESS;
}

/** Increases the given date by the given number of days.
		If nDays is negative, the date will be decreased instead. */
RETURN_CODE Common::IncreaseDate(unsigned short date[3], int nDays) {
	// Check for illegal dates
	if (date[1] < 1 || date[1] > 12)
		return FAIL;
	if (date[2] < 1 || date[2] > novac::DaysInMonth(date[0], date[1]))
		return FAIL;

	// If we should not change the date, return without doing anything
	if (nDays == 0)
		return SUCCESS;

	// Check if we instead should decrease the date
	if (nDays < 0)
		DecreaseDate(date, -nDays);

	unsigned short *day = &date[2];
	unsigned short *month = &date[1];
	unsigned short *year = &date[0];

	// increase the day of the month
	*day += nDays;

	// Check the day of the month
	while (*day > novac::DaysInMonth(*year, *month)) { // <-- if we've passed to the next month
		*day -= novac::DaysInMonth(*year, *month);
		++*month; // go the next month

		while (*month > 12) { // <-- if we've passed to the next year
			*year += 1;		// go to the nex year
			*month -= 12;
		}
	}
	// Check the month 
	while (*month > 12) { // <-- if we've passed to the next year
		*year += 1;		// go to the nex year
		*month -= 12;
	}

	return SUCCESS;
}

/** Retrieves the solar zenith angle (SZA) and the solar azimuth angle (SAZ)
		for the site specified by (lat, lon) and for the time given in gmtTime.
		Note that the returned angles are in degrees and that the specified
		time _must_ be GMT-time. */
RETURN_CODE Common::GetSunPosition(const novac::CDateTime &gmtTime, double lat, double lon, double &SZA, double &SAZ) {
	SZA = SAZ = 0; // reset the numbers

	// Get the julian day
	double D = novac::JulianDay(gmtTime) - 2451545.0;

	// Get the Equatorial coordinates...
	double	RA; //	the right ascension (deg)
	double	dec; // the declination	(deg)
	double	EqT;	// the equation of time (hours)
	EquatorialCoordinates(D, RA, dec, EqT);

	// Get the hour angle
	double fractionalHour = (double)gmtTime.hour + gmtTime.minute / 60.0 + gmtTime.second / 3600.0;
	double H = GetHourAngle(fractionalHour, lon, EqT);

	// Get the horizontal coordinates
	double	elev, sAzim; // The elevation and azimuth (towards south);
	HorizontalCoordinates(lat, H, dec, elev, sAzim);

	// Convert the elevation into sza
	SZA = 90.0 - elev;

	// Convert the azimuth to a value counted from the north and 
	SAZ = fmod(180.0 + sAzim, 360.0);

	return SUCCESS;
}


/** Sorts a list of strings in either ascending or descending order */
void Common::Sort(novac::CList <novac::CString> &strings, bool files, bool ascending) {
	unsigned long nStrings = (unsigned long)strings.GetCount(); // number of elements
	unsigned long it = 0; // <-- iterator

	if (nStrings <= 1) {
		return; // <-- We're actually already done
	}
	else {
		novac::CList <novac::CString> left;
		novac::CList <novac::CString> right;

		// Make two copies of the list, one of the first half and one of the second half
		auto pos = strings.GetHeadPosition();
		while (it < nStrings / 2) {
			left.AddTail(strings.GetNext(pos));
			++it;
		}
		while (pos != NULL) {
			right.AddTail(strings.GetNext(pos));
		}

		// Sort each of the two halves
		Sort(left, files, ascending);
		Sort(right, files, ascending);

		// Merge the two...
		MergeLists(left, right, strings, files, ascending);
	}
}

/** Merges the two lists 'list1' and 'list2' in a sorted way and stores
		the result in the output-list 'result' */
void Common::MergeLists(const novac::CList <novac::CString> &list1, const novac::CList <novac::CString> &list2, novac::CList <novac::CString> &result, bool files, bool ascending) {
	novac::CString	name1, name2, fullName1, fullName2;
	int comparison;

	auto pos_1 = list1.GetHeadPosition();
	auto pos_2 = list2.GetHeadPosition();

	// Clear the output-list
	result.RemoveAll();

	// 1. As long as there are elements in both lists, do this
	while (pos_1 != NULL && pos_2 != NULL) {
		// Get the file-names of the first and the second 
		fullName1.Format(list1.GetAt(pos_1));	// position k
		fullName2.Format(list2.GetAt(pos_2));	// position k+1

		if (files) {
			// Extract the file-names only
			name1.Format(fullName1);
			name2.Format(fullName2);
			Common::GetFileName(name1);
			Common::GetFileName(name2);

			// Compare the two names
			comparison = name1.Compare(name2);
		}
		else {
			// Compare the two names
			comparison = fullName1.Compare(fullName2);
		}

		if (comparison == 0) {
			// if equal
			result.AddTail(fullName1);
			list1.GetNext(pos_1);
			continue;
		}
		else if (comparison < 0) {
			// fullName1 < fullName2
			if (ascending) {
				result.AddTail(fullName1);
				list1.GetNext(pos_1);
				continue;
			}
			else {
				result.AddTail(fullName2);
				list2.GetNext(pos_2);
				continue;
			}
		}
		else {
			// fullName1 > fullName2
			if (ascending) {
				result.AddTail(fullName2);
				list2.GetNext(pos_2);
				continue;
			}
			else {
				result.AddTail(fullName1);
				list1.GetNext(pos_1);
				continue;
			}
		}

	}

	// 2. If we're out of elements in list 2 but not in list 1, do this
	while (pos_1 != NULL) {
		fullName1.Format(list1.GetNext(pos_1));
		result.AddTail(fullName1);
	}

	// 3. If we're out of elements in list 1 but not in list 2, do this
	while (pos_2 != NULL) {
		fullName2.Format(list2.GetNext(pos_2));
		result.AddTail(fullName2);
	}

}

double Common::CalculateFlux(const double *scanAngle, const double *scanAngle2, const double *column, double offset, int nDataPoints, const Meteorology::CWindField &wind, const Geometry::CPlumeHeight &relativePlumeHeight, double compass, INSTRUMENT_TYPE type, double coneAngle, double tilt) {
	if (type == INSTR_HEIDELBERG) {
		return CalculateFlux_HeidelbergFormula(scanAngle, scanAngle2, column, offset, nDataPoints, wind, relativePlumeHeight, compass);
	}
	else if (type == INSTR_GOTHENBURG) {
		if (fabs(coneAngle - 90.0) < 1.0)
			return CalculateFlux_FlatFormula(scanAngle, column, offset, nDataPoints, wind, relativePlumeHeight, compass);
		else
			return CalculateFlux_ConeFormula(scanAngle, column, offset, nDataPoints, wind, relativePlumeHeight, compass, coneAngle, tilt);
	}
	else {
		return 0.0; // unsupported instrument-type
	}
}

/** Calculates the flux using the supplied data using the old algorithm */
double Common::CalculateFlux_FlatFormula(const double *scanAngle, const double *column, double offset, int nDataPoints, const Meteorology::CWindField &wind, const Geometry::CPlumeHeight &relativePlumeHeight, double compass) {
	double avgVCD, VCD1, VCD2, TAN1, TAN2, distance;
	double flux = 0;
	double partialFlux;

	// the wind factor
	double windFactor = fabs(cos(DEGREETORAD*(wind.GetWindDirection() - compass)));

	// now calculate the flux
	for (int i = 0; i < nDataPoints - 1; ++i) {
		if (fabs(fabs(scanAngle[i]) - 90.0) < 0.5)
			continue; // the distance-calculation has a singularity at +-90 degrees so just skip those points!
		if (fabs(fabs(scanAngle[i + 1]) - 90.0) < 0.5)
			continue; // the distance-calculation has a singularity at +-90 degrees so just skip those points!

		// The vertical columns
		VCD1 = (column[i] - offset)	 * cos(DEGREETORAD * scanAngle[i]);
		VCD2 = (column[i + 1] - offset) * cos(DEGREETORAD * scanAngle[i + 1]);

		// calculating the horisontal distance
		TAN1 = tan(DEGREETORAD * scanAngle[i]);
		TAN2 = tan(DEGREETORAD * scanAngle[i + 1]);
		distance = relativePlumeHeight.m_plumeAltitude * fabs(TAN2 - TAN1);

		// The average vertical column
		avgVCD = (VCD1 + VCD2) * 0.5;

		// The flux...
		partialFlux = distance * avgVCD * wind.GetWindSpeed() * windFactor;
		flux += partialFlux;
	}

	return fabs(flux);
}

/** Calculates the flux using the supplied data using the cone-scanner algorithm */
double Common::CalculateFlux_ConeFormula(const double *scanAngle, const double *column, double offset, int nDataPoints, const Meteorology::CWindField &wind, const Geometry::CPlumeHeight &relativePlumeHeight, double compass, double coneAngle, double tilt) {
	double flux = 0;
	double partialFlux, columnAmplification;

	// convert the angles to radians
	tilt *= DEGREETORAD;
	coneAngle *= DEGREETORAD;

	// local-data buffer to store the intermediate calculations
	double	*alpha = new double[nDataPoints];
	double	*scd = new double[nDataPoints];
	double	*columnCorrection = new double[nDataPoints];
	double	*x = new double[nDataPoints];
	double	*y = new double[nDataPoints];

	// Temporary variables, to do less computations
	double	tan_coneAngle = tan(coneAngle);
	double	sin_tilt = sin(tilt);
	double	cos_tilt = cos(tilt);

	// First prepare the buffers before we calculate anything
	for (int i = 0; i < nDataPoints - 1; ++i) {
		// The slant columns
		scd[i] = column[i] - offset;

		// The scan-angles, in radians
		alpha[i] = scanAngle[i] * DEGREETORAD;

		// cosine and sine of the scan-angle
		double cos_alpha = cos(alpha[i]);
		double sin_alpha = sin(alpha[i]);

		// Calculate the AMF in order to get vertical columns
		double x_term = pow(cos_tilt / tan_coneAngle - cos_alpha*sin_tilt, 2);
		double y_term = pow(sin_alpha, 2);
		double divisor = pow(cos_alpha*cos_tilt + sin_tilt / tan_coneAngle, 2);
		columnAmplification = sqrt((x_term + y_term) / divisor + 1);
		columnCorrection[i] = 1 / columnAmplification;

		// Calculate the projections of the intersection points in the ground-plane
		double commonDenominator = cos_alpha*cos_tilt + sin_tilt / tan_coneAngle;
		x[i] = (cos_tilt / tan_coneAngle - cos_alpha*sin_tilt) / commonDenominator;
		y[i] = (sin_alpha) / commonDenominator;
	}

	// Now make the actual flux-calculation
	for (int i = 0; i < nDataPoints - 2; ++i) {
		if (fabs(fabs(alpha[i]) - HALF_PI) < 1e-2 || fabs(fabs(alpha[i + 1]) - HALF_PI) < 1e-2)
			continue;// This algorithm does not work very well for scanangles around +-90 degrees

		// The average vertical column
		double avgVCD = (scd[i] * columnCorrection[i] + scd[i + 1] * columnCorrection[i + 1]) * 0.5;

		// The horizontal distance
		double S = relativePlumeHeight.m_plumeAltitude * sqrt(pow(x[i + 1] - x[i], 2) + pow(y[i + 1] - y[i], 2));

		// The local compass-direction [radians] due to the curvature of the cone
		double coneCompass = atan2(y[i + 1] - y[i], x[i + 1] - x[i]);

		// The wind-factor 
		double windFactor = fabs(sin(DEGREETORAD * (wind.GetWindDirection() - compass) - coneCompass));

		// The partial flux
		partialFlux = avgVCD * S * wind.GetWindSpeed() * windFactor;

		// The total flux
		flux += partialFlux;
	}

	delete[] alpha;
	delete[] scd;
	delete[] columnCorrection;
	delete[] x;
	delete[] y;

	return fabs(flux);
}

/** Calculates the flux using an instrument of type Heidelberg = general algorithm for calculating the flux using cartesian coordinates and angles defined as
		elev	= scanAngle 1 (elevation, for elevation=0° azimuth=90° pointing to North)
		azim	= scanAngle 2 (azimuth from North, clockwise)*/
double Common::CalculateFlux_HeidelbergFormula(const double *scanAngle1, const double *scanAngle2, const double *column, double offset, int nDataPoints, const Meteorology::CWindField &wind, const Geometry::CPlumeHeight &relativePlumeHeight, double compass) {
	double flux = 0;
	double partialFlux;

	/*TODO: writing the Log, what needs to be included? */
#ifdef _DEBUG
	FILE *f = fopen("Debug_fluxLog_HD.txt", "w");
	if (f != NULL) {
		fprintf(f, "Calculating flux for Heidelberg instrument with %d spectra\n", nDataPoints);
		fprintf(f, "windDirection=%lf\ncompass=%lf\noffset=%lf\n\n",
			wind.GetWindDirection(), compass, offset);
		fprintf(f, "ScanAngle1_Elev[deg]\tScanAngle2_Azim[deg]\tColumn[ppmm]\tAvgVCD[ppmm]\tHorizontalDistance\tConeCompass\tWindFactor\tPartialFlux\n");
		fclose(f);
	}
#endif

	// local-data buffer to store the intermediate calculations
	// double	*alpha							= new double[nDataPoints];

	double	*elev = new double[nDataPoints];
	double	*azim = new double[nDataPoints];

	double	*scd = new double[nDataPoints];
	double	*columnCorrection = new double[nDataPoints];
	double	*x = new double[nDataPoints];
	double	*y = new double[nDataPoints];

	// First prepare the buffers before we calculate anything
	for (int i = 0; i < nDataPoints - 1; ++i) {
		// The slant columns
		scd[i] = column[i + 1] - offset;

		// The scan-angles, in radians
		elev[i] = scanAngle1[i] * DEGREETORAD;
		azim[i] = scanAngle2[i] * DEGREETORAD;

		double tan_elev = tan(elev[i]);
		double cos_elev = cos(elev[i]);
		double sin_azim = sin(azim[i]);
		double cos_azim = cos(azim[i]);

		// Calculate the AMF in order to get vertical columns
		columnCorrection[i] = cos_elev;


		// Calculate the projections of the intersection points in the ground-plane
		double x_term = tan_elev * cos_azim;
		double y_term = tan_elev * sin_azim;

		x[i] = x_term;
		y[i] = y_term;
	}

	// Now make the actual flux-calculation
/*TODO: flux calculations for Heidelberg instrument differ from Gothenborg instrument because local and global coordinate system do not differ!!!! Define another loop for Heidelberg?*/
	for (int i = 0; i < nDataPoints - 2; ++i) {
		if (fabs(fabs(elev[i]) - HALF_PI) < 1e-2 || fabs(fabs(elev[i + 1]) - HALF_PI) < 1e-2)
			continue;// This algorithm does not work very well for scanangles around +-90 degrees

		// The average vertical column
		double avgVCD = (scd[i] * columnCorrection[i] + scd[i + 1] * columnCorrection[i + 1]) * 0.5;

		// The horizontal distance
		double S = relativePlumeHeight.m_plumeAltitude * sqrt(pow(x[i + 1] - x[i], 2) + pow(y[i + 1] - y[i], 2));

		// The local compass-direction [radians] due to the curvature of the cone
		double DirectionCompass = atan2(y[i + 1] - y[i], x[i + 1] - x[i]);

		// The wind-factor HD: compass=azim[i]
		double windFactor = fabs(sin(DEGREETORAD * wind.GetWindDirection() - DirectionCompass));

		// The partial flux
		partialFlux = avgVCD * S * wind.GetWindSpeed() * windFactor;

		// The total flux
		flux += partialFlux;

#ifdef _DEBUG
		FILE *f = fopen("Debug_fluxLog_HD.txt", "a+");
		if (f != NULL) {
			fprintf(f, "%lf\t%lf\t%lf\t%lf\t", scanAngle1[i], scanAngle2[i], scd[i], avgVCD);
			fprintf(f, "%lf\t%lf\t%lf\t", S, DirectionCompass * RADTODEGREE, windFactor);
			fprintf(f, "%lf\n", partialFlux);
			fclose(f);
		}
#endif

	}
	//HD constants are to be deleted as well
	delete[] scd;
	delete[] columnCorrection;
	delete[] x;
	delete[] y;
	delete[] elev;
	delete[] azim;

	return fabs(flux);
}

double Common::CalculateOffset(const double *columns, const bool *badEvaluation, long numPoints) {
	SpecData avg;
	Common common;
	long i;

	// calculate the offset as the average of the three lowest column values 
	//    that are not considered as 'bad' values

	double *testColumns = new double[numPoints];

	int numColumns = 0;
	for (i = 0; i < numPoints; ++i) {
		if (badEvaluation[i])
			continue;

		testColumns[numColumns++] = columns[i];
	}

	if (numColumns <= 5) {
		delete[] testColumns;
		return 0.0;
	}

	// Find the N lowest column values
	int N = (int)(0.2 * numColumns);
	SpecData *m = new SpecData[N];
	memset(m, (int)1e6, N * sizeof(SpecData));
	if (FindNLowest(testColumns, numColumns, m, N)) {
		avg = Average(m, N);
		delete[] testColumns;
		delete[] m;
		return avg;
	}

	delete[] testColumns;
	delete[] m;

	// could not calculate a good offset.
	return 0;
}

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
			width of the plume (same unit as the scanAngles)	*/
bool Common::FindPlume(const double *scanAngles, const double *phi, const double *columns, const double *columnErrors, const bool *badEvaluation, long numPoints, CPlumeInScanProperty &plumeProperties) {

	// There is a plume iff there is a region, where the column-values are considerably
	//	much higher than in the rest of the scan

	// Make a local copy of the values, picking out only the good ones
	double *col = new double[numPoints];
	double *colE = new double[numPoints];
	double *angle = new double[numPoints];
	double *p = new double[numPoints];
	double minColumn = 1e23;
	int		nCol = 0; // <-- the number of ok column values
	for (int k = 0; k < numPoints; ++k) {
		if (!badEvaluation[k]) {
			col[nCol] = columns[k];
			colE[nCol] = columnErrors[k];
			angle[nCol] = scanAngles[k];
			p[nCol] = phi[k];
			minColumn = std::min(minColumn, columns[k]);
			++nCol;
		}
	}
	if (nCol <= 5) { // <-- if too few ok points, then there's no plume
		delete[] col;		delete[] colE;		delete[] angle;	delete[] p;
		return false;
	}

	// Add the smallest column-value to all other columns, this to make an approximate offset
	for (int k = 0; k < nCol; ++k) {
		col[k] -= minColumn;
	}

	// Try different divisions of the scan to see if there is a region of at least
	//	'minWidth' values where the column-value is considerably higher than the rest
	double highestDifference = -1e16;
	long minWidth = 5;
	int regionLow = 0, regionHigh = 0;
	for (int low = 0; low < nCol; ++low) {
		for (int high = low + minWidth; high < nCol; ++high) {
			// The width of the region has to be at least 'minWidth' values, otherwise there's no idea to search
			//	There must also be at least 'minWidth' values outside of the region...
			if ((high - low < minWidth) || (nCol - high + low < minWidth))
				continue;

			// the average column value in the region we're testing
			double avgInRegion = Average(col + low, high - low);

			// the average column value outside of the region we're testing
			double avgOutRegion = (Average(col, low)*low + Average(col + high, nCol - high)*(nCol - high)) / (low + nCol - high);

			if (avgInRegion - avgOutRegion > highestDifference) {
				highestDifference = avgInRegion - avgOutRegion;
				regionLow = low;
				regionHigh = high;
			}
		}
	}

	// Calculate the average column error, for the good measurement points
	double avgColError = Average(colE, nCol);

	if (highestDifference > 5 * avgColError) {
		// the plume centre is the average of the scan-angles in the 'plume-region'
		//	weighted with the column values
		double sumAngle_alpha = 0, sumAngle_phi = 0, sumWeight = 0;
		for (int k = regionLow; k < regionHigh; ++k) {
			sumAngle_alpha += angle[k] * col[k];
			sumAngle_phi += p[k] * col[k];
			sumWeight += col[k];
		}
		plumeProperties.m_plumeCentre[0] = sumAngle_alpha / sumWeight;
		plumeProperties.m_plumeCentre[1] = sumAngle_phi / sumWeight; // if phi == NULL then this will be non-sense

		// The edges of the plume and the error in the estimated plume centres
		plumeProperties.m_plumeEdge_low = angle[0];
		plumeProperties.m_plumeEdge_high = angle[nCol - 1];
		double peakLow = angle[0];
		double peakHigh = angle[nCol - 1];
		double maxCol_div_e = Max(col, nCol) * 0.3679;
		double maxCol_90 = Max(col, nCol) * 0.90;

		// Search for the lower edge of the plume ...
		for (int k = 0; k < nCol - 2; ++k) {
			if (angle[k] > plumeProperties.m_plumeCentre[0]) {
				break;
			}
			else if (Average(col + k, 2) < maxCol_div_e) {
				plumeProperties.m_plumeEdge_low = angle[k];
			}
			if ((col[k] < maxCol_90) && (col[k + 1] >= maxCol_90)) {
				peakLow = angle[k];
			}
		}

		// .. and then the upper edge
		for (int k = nCol - 1; k >= 1; --k) {
			if (angle[k] <= plumeProperties.m_plumeCentre[0]) {
				break;
			}
			else if (Average(col + k - 1, 2) < maxCol_div_e) {
				plumeProperties.m_plumeEdge_high = angle[k];
			}
			if ((col[k] < maxCol_90) && (col[k - 1] >= maxCol_90)) {
				peakHigh = angle[k];
			}
		}
		plumeProperties.m_plumeCentreError[0] = (peakHigh - peakLow) / 2;

		delete[] col;	delete[] colE;
		delete[] angle; delete[] p;
		return true;
	}
	else {
		delete[] col;	delete[] colE;
		delete[] angle; delete[] p;
		return false;
	}
}

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
bool Common::CalculatePlumeCompleteness(const double *scanAngles, const double *phi, const double *columns, const double *columnErrors, const bool *badEvaluation, double offset, long numPoints, CPlumeInScanProperty &plumeProperties) {
	int nDataPointsToAverage = 5;

	// Check if there is a plume at all...
	bool inPlume = FindPlume(scanAngles, phi, columns, columnErrors, badEvaluation, numPoints, plumeProperties);
	if (!inPlume) {
		plumeProperties.m_completeness = 0.0; // <-- no plume at all
		return false;
	}

	// Calculate the average of the 'nDataPointsToAverage' left-most values
	double avgLeft = 0.0;
	int nAverage = 0;
	for (int k = 0; k < numPoints; ++k) {
		if (!badEvaluation[k]) {
			avgLeft += columns[k] - offset;
			++nAverage;
			if (nAverage == nDataPointsToAverage)
				break;
		}
	}
	if (nAverage < nDataPointsToAverage) {
		// not enough data-points to make an ok average, return fail
		plumeProperties.m_completeness = 0.0; // <-- no plume at all
		return false;
	}
	avgLeft /= nDataPointsToAverage;

	// Calculate the average of the 'nDataPointsToAverage' right-most values
	double avgRight = 0.0;
	nAverage = 0;
	for (int k = numPoints - 1; k > 0; --k) {
		if (!badEvaluation[k]) {
			avgRight += columns[k] - offset;
			++nAverage;
			if (nAverage == nDataPointsToAverage)
				break;
		}
	}
	if (nAverage < nDataPointsToAverage) {
		// not enough data-points to make an ok average, return fail
		plumeProperties.m_completeness = 0.0; // <-- no plume at all
		return false;
	}
	avgRight /= nDataPointsToAverage;

	// Find the maximum column value
	double maxColumn = 0.0;
	for (int k = 0; k < numPoints; ++k) {
		if (!badEvaluation[k]) {
			maxColumn = std::max(maxColumn, columns[k] - offset);
		}
	}

	// The completeness
	plumeProperties.m_completeness = 1.0 - 0.5 * std::max(avgLeft, avgRight) / maxColumn;
	if (plumeProperties.m_completeness > 1.0)
		plumeProperties.m_completeness = 1.0;

	return true;
}

void Common::GuessSpecieName(const novac::CString &fileName, novac::CString &specie) {
	specie.Format("");
	novac::CString spc[] = { "SO2", "NO2", "O3", "O4", "HCHO", "RING", "H2O", "CLO", "BRO", "CHOCHO", "Glyoxal", "Formaldehyde", "HONO", "NO3" };
	int nSpecies = 12;

	int index = fileName.ReverseFind('\\');
	if (index == 0)
		return;

	novac::CString fil;
	fil.Format("%s", (const char*)fileName.Right((int)strlen(fileName) - index - 1));
	fil.MakeUpper();

	for (int i = 0; i < nSpecies; ++i) {
		if (strstr(fil, spc[i])) {
			specie.Format("%s", (const char*)spc[i]);
			return;
		}
	}

	// nothing found
	return;
}



/** Take out the exe name from a long path
	  @param fileName path of the exe file	*/
void Common::GetFileName(novac::CString& fileName)
{
	// look for slashes in the path
	int position = std::max(fileName.ReverseFind('\\'), fileName.ReverseFind('/'));
	int length = fileName.GetLength();
	fileName = fileName.Right(length - position - 1);
}

/** Take out the directory from a long path name.
	@param fileName - the complete path of the file */
void Common::GetDirectory(novac::CString &fileName) {
	int position = fileName.ReverseFind('\\');
	if (position >= 0)
	{
		fileName = fileName.Left(position + 1);
	}
}

void Common::CopyFile(const novac::CString& oldName, const novac::CString& newName)
{
	Poco::File oldFile(oldName.std_str());

	oldFile.copyTo(newName.std_str());
}

long Common::RetrieveFileSize(novac::CString& fileName)
{
	Poco::File file(fileName.std_str());
	return file.getSize();
}


/** Compares two files to see if their contents are the same */
bool Common::AreIdenticalFiles(const novac::CString &fileName1, const novac::CString &fileName2) {
	if (Equals(fileName1, fileName2))
		return true; // a file is always identical to itself

	FILE *f1 = fopen(fileName1, "r");
	if (f1 == NULL)
		return false;

	FILE *f2 = fopen(fileName2, "r");
	if (f2 == NULL)
		return false;

	while (1) {
		int c1 = fgetc(f1);
		int c2 = fgetc(f2);

		if (c1 == EOF) {
			fclose(f1); fclose(f2);
			if (c2 == EOF) {
				return true;
			}
			else {
				return false;
			}
		}

		if (c1 != c2)
			return false;
	}


	fclose(f1); fclose(f2);

	// should never reach this point...
	return false;
}

/** If there's a file with the given input name, then it will be renamed to
	PATH\\FILENAME_creationDate_creationTime.FILEENDING */
bool Common::ArchiveFile(const novac::CString &fileName) {
	novac::CString newFileName, errorMsg;

	// Search for the file
	Poco::File oldFile(fileName.std_str());

	if (!oldFile.exists()) {
		return false; // file does not exist
	}

	// Get the time the file was created
	// TODO: Is this the local time or the UTC time stamp??
	Poco::DateTime creationTime = Poco::DateTime(oldFile.created());

	// build the new file-name
	int lastDot = fileName.ReverseFind('.');
	if (lastDot == -1) {
		newFileName.Format("%s_%04d%02d%02d_%02d%02d", (const char*)fileName,
			creationTime.year(), creationTime.month(), creationTime.day(), creationTime.hour(), creationTime.minute());
	}
	else {
		newFileName.Format("%s_%04d%02d%02d_%02d%02d%s", (const char*)fileName.Left(lastDot),
			creationTime.year(), creationTime.month(), creationTime.day(), creationTime.hour(), creationTime.minute(), (const char*)fileName.Right(fileName.GetLength() - lastDot));
	}

	// move the file
	oldFile.moveTo(newFileName.std_str());

	return true;
}

int Common::GetInterlaceSteps(int channel, int &interlaceSteps) {
	// if the spectrum is a mix of several spectra
	if (channel >= 129) {
		interlaceSteps = channel - 127;
		return -1;
	}

	// special case, channel = 128 is same as channel = 0
	if (channel == 128)
		channel = 0;

	// If the spectrum is a single spectrum
	interlaceSteps = (channel / 16) + 1; // 16->31 means interlace=2, 32->47 means interlace=3 etc.
	return (channel % 16); // the remainder tells the channel number
}

/*EQUATORIAL COORDINATES:RIGHT ASCENSION AND DECLINATION*/
void Common::EquatorialCoordinates(double D, double &RA, double &dec, double &EQT)
{
	double g_deg, q_deg, L_deg;				/*ANGLES IN DEGREES*/
	double g_rad, q_rad, L_rad;				/*ANGLES IN	RADIANS*/
	double R;												/*DISTANCE SUN-EARTH IN A.U*/
	double obliq_deg, obliq_rad;			/*OBLIQUITY OF THE ECLIPTIC*/
	double RA_rad, dec_rad;					/*EQUATORIAL COORDINATES IN RADIANS*/

	g_deg = fmod(357.529 + 0.98560028 * D, 360.0);
	g_rad = g_deg	* DEGREETORAD;
	q_deg = fmod(280.459 + 0.98564736*D, 360.0);
	q_rad = q_deg * DEGREETORAD;

	L_deg = q_deg + 1.915*sin(g_rad) + 0.02*sin(2 * g_rad);
	L_rad = L_deg * DEGREETORAD;

	// The distance between the sun and the earth (in Astronomical Units)
	R = 1.00014 - 0.01671*cos(g_rad) - 0.00014*cos(2 * g_rad);

	// The obliquity of the earth's orbit:
	obliq_deg = 23.439 - 0.00000036 * D;
	obliq_rad = obliq_deg * DEGREETORAD;

	// The right ascension (RA)
	RA_rad = atan(cos(obliq_rad) * sin(L_rad) / cos(L_rad));
	if (RA_rad < 0)
		RA_rad = TWO_PI + RA_rad;

	if (fabs(RA_rad - L_rad) > 1.570796)
		RA_rad = M_PI + RA_rad;

	dec_rad = asin(sin(obliq_rad) * sin(L_rad));
	RA = fmod(RA_rad * RADTODEGREE, 360.0);		// The right ascension

	// The declination
	dec = dec_rad * RADTODEGREE;

	// The Equation of Time
	EQT = q_deg / 15.0 - RA / 15.0;
}

void Common::HorizontalCoordinates(double lat, double H, double dec, double &elev, double &azim) {
	double H_rad = H	 * DEGREETORAD;
	double lat_rad = lat * DEGREETORAD;
	double dec_rad = dec * DEGREETORAD;

	double elev_rad, azim_rad, cazim_rad, sazim_rad;

	// The elevation angle
	elev_rad = asin(cos(H_rad)*cos(dec_rad)*cos(lat_rad) + sin(dec_rad)*sin(lat_rad));

	// The cosine of the azimuth - angle
	cazim_rad = (cos(H_rad)*cos(dec_rad)*sin(lat_rad) - sin(dec_rad)*cos(lat_rad)) / cos(elev_rad);

	// The sine of the azimuth - angle
	sazim_rad = (sin(H_rad)*cos(dec_rad)) / cos(elev_rad);

	if (cazim_rad > 0 && sazim_rad > 0)
		azim_rad = asin(sazim_rad);						// azim is in the range 0 - 90 degrees
	else if (cazim_rad < 0 && sazim_rad > 0)
		azim_rad = M_PI - asin(sazim_rad);			// azim is in the range 90 - 180 degrees
	else if (cazim_rad < 0 && sazim_rad < 0)
		azim_rad = M_PI - asin(sazim_rad);		// azim is in the range 180 - 270 degrees
	else if (cazim_rad > 0 && sazim_rad < 0)
		azim_rad = TWO_PI + asin(sazim_rad);		// azim is in the range 270 - 360 degrees

	elev = elev_rad * RADTODEGREE;
	azim = azim_rad * RADTODEGREE;

	//printf("\n\nHORIZONTAL COORDINATES:\nAZIMUTH (from South to West)=%f\nELEVATION=%f\n\n",*pazim,*pelev);
}

/** Returns the hour angle given the longitude and equation of time. */
double Common::GetHourAngle(double hr, double lon, double EqT) {
	double H = 15.0 * (hr + lon / 15 + EqT - 12);
	//    printf("HOUR ANGLE (from noon,increasing with time): %f\n",H);
	return(H);
}

