#include "StdAfx.h"
#include "datetime.h"
#include "Common.h"
#include <time.h>

CDateTime::CDateTime(void)
{
	year = 0;
	month = 0;
	day = 0;
	hour = 0;
	minute = 0;
	second = 0;
}

CDateTime::CDateTime(const CDateTime &t2) {
	year = t2.year;
	month = t2.month;
	day = t2.day;
	hour = t2.hour;
	minute = t2.minute;
	second = t2.second;
}

CDateTime::CDateTime(int y, int mo, int d, int h, int mi, int s) {
	year = y;
	month = mo;
	day = d;
	hour = h;
	minute = mi;
	second = s;
}

CDateTime::~CDateTime(void)
{
}

int CDateTime::operator <(const CDateTime &t2) const {
	if (this->year < t2.year)
		return 1;
	if (this->year > t2.year)
		return 0;
	if (this->month < t2.month)
		return 1;
	if (this->month > t2.month)
		return 0;
	if (this->day < t2.day)
		return 1;
	if (day > t2.day)
		return 0;
	if (this->hour < t2.hour)
		return 1;
	if (this->hour > t2.hour)
		return 0;
	if (this->minute < t2.minute)
		return 1;
	if (this->minute > t2.minute)
		return 0;
	if (this->second < t2.second)
		return 1;
	if (this->second > t2.second)
		return 0;

	// equal
	return 0;
}

int CDateTime::operator <=(const CDateTime &t2) const {
	if (*this < t2)
		return true;
	if (*this == t2)
		return true;

	return false;
}

int CDateTime::operator >(const CDateTime &t2) const {
	if (*this < t2)
		return false;
	if (*this == t2)
		return false;

	return true;
}

int CDateTime::operator >=(const CDateTime &t2) const {
	if (*this < t2)
		return false;

	return true;
}

bool CDateTime::operator==(const CDateTime& t2) const {
	if (this->second != t2.second)
		return false;
	if (this->minute != t2.minute)
		return false;
	if (this->hour != t2.hour)
		return false;
	if (this->day != t2.day)
		return false;
	if (this->month != t2.month)
		return false;
	if (this->year != t2.year)
		return false;
	return true;
}

CDateTime	& CDateTime::operator=(const CDateTime& t2) {
	this->year = t2.year;
	this->month = t2.month;
	this->day = t2.day;
	this->hour = t2.hour;
	this->minute = t2.minute;
	this->second = t2.second;
	return *this;
}

void CDateTime::SetToNow() {
	struct tm *tim;
	time_t t;
	time(&t);
	tim = localtime(&t);

	this->year = 1900 + tim->tm_year;
	this->month = 1 + tim->tm_mon;
	this->day = tim->tm_mday;
	this->hour = tim->tm_hour;
	this->minute = tim->tm_min;
	this->second = tim->tm_sec;
}

/** Calculates the difference, in seconds, between two times.
		If t2 is later than t1, then the result will be negative. */
double	CDateTime::Difference(const CDateTime &t1, const CDateTime &t2) {
	struct tm tid1, tid2;
	tid1.tm_year = t1.year - 1900;
	tid1.tm_mon = t1.month - 1;
	tid1.tm_mday = t1.day;
	tid1.tm_hour = t1.hour;
	tid1.tm_min = t1.minute;
	tid1.tm_sec = t1.second;
	tid1.tm_wday = 0;
	tid1.tm_yday = 0;
	tid1.tm_isdst = 0;

	tid2.tm_year = t2.year - 1900;
	tid2.tm_mon = t2.month - 1;
	tid2.tm_mday = t2.day;
	tid2.tm_hour = t2.hour;
	tid2.tm_min = t2.minute;
	tid2.tm_sec = t2.second;
	tid2.tm_wday = 0;
	tid2.tm_yday = 0;
	tid2.tm_isdst = 0;

	time_t t_1 = mktime(&tid1);
	time_t t_2 = mktime(&tid2);

	return difftime(t_1, t_2);
}

/** Increments the current time with the supplied number of seconds */
void CDateTime::Increment(int secs) {
	// calculate the number of seconds since midnight this is...
	long nSecsSinceMidnight = second + 60 * minute + 3600 * hour;

	if (secs <= (86400 - nSecsSinceMidnight)) {
		// this is a small change, only change within the same day
		nSecsSinceMidnight += secs;

		this->hour = (unsigned char)(nSecsSinceMidnight / 3600);
		this->minute = (unsigned char)((nSecsSinceMidnight - 3600 * hour) / 60);
		this->second = (unsigned char)((nSecsSinceMidnight % 60));

		return;
	}
	else {
		// go to the next before
		IncrementOneDay();

		while (secs > 86400) {
			DecrementOneDay();
			secs -= 86400;
		}

		// we should increment less than 86400 seconds (less than one day)
		nSecsSinceMidnight = secs - (86400 - hour * 3600 - minute * 60 - second);

		this->hour = (unsigned char)(nSecsSinceMidnight / 3600);
		this->minute = (unsigned char)((nSecsSinceMidnight - 3600 * hour) / 60);
		this->second = (unsigned char)((nSecsSinceMidnight % 60));

		return;
	}
}

/** Decrements the current time with the supplied number of seconds */
void CDateTime::Decrement(int secs) {
	// calculate the number of seconds since midnight this is...
	long nSecsSinceMidnight = second + 60 * minute + 3600 * hour;

	if (secs <= nSecsSinceMidnight) {
		// this is a small change, only change within the same day
		nSecsSinceMidnight -= secs;

		this->hour = (unsigned char)(nSecsSinceMidnight / 3600);
		this->minute = (unsigned char)((nSecsSinceMidnight - 3600 * hour) / 60);
		this->second = (unsigned char)((nSecsSinceMidnight % 60));

		return;
	}
	else {
		// go to the day before
		DecrementOneDay();

		while (secs > 86400) {
			DecrementOneDay();
			secs -= 86400;
		}

		// we should decrement less than 86400 seconds (less than one day)
		nSecsSinceMidnight = 86400 - secs;

		this->hour = (unsigned char)(nSecsSinceMidnight / 3600);
		this->minute = (unsigned char)((nSecsSinceMidnight - 3600 * hour) / 60);
		this->second = (unsigned char)((nSecsSinceMidnight % 60));

		return;
	}
}

/** Decrements the current time to the same time the day before */
void CDateTime::IncrementOneDay() {
	long daysInThisMonth = Common::DaysInMonth(this->year, this->month);

	if (day < daysInThisMonth) {
		day += 1;
		return;
	}
	else {
		if (this->month < 12) {
			day = Common::DaysInMonth(this->year, this->month);
			month += 1;
		}
		else {
			day = 01;
			month = 01;
			year += 1;
		}
	}
}

/** Decrements the current time to the same time the day before */
void CDateTime::DecrementOneDay() {
	if (day > 1) {
		day -= 1;
		return;
	}
	else {
		if (this->month > 1) {
			day = Common::DaysInMonth(this->year, this->month - 1);
			month -= 1;
		}
		else {
			day = 31;
			month = 12;
			year -= 1;
		}
	}
}

/** Attempts to parse the date found in the string 'dateStr', the resulting
	date is (on successful parsing) returned in 't'
	The date MUST be in either of the formats YYYY.MM.DD / YYYY:MM:DD or YYYYMMDD
	NOTE only the date is parsed not the time!!!!
	@return true on success else false.
*/
bool CDateTime::ParseDate(const novac::CString &dateStr, CDateTime &t) {
	char *pt = NULL, *pt2 = NULL;
	char str[256]; // local copy
	_snprintf(str, 255, dateStr);
	int y, m, d;
	y = m = d = 0;

	// look if this is a function...
	if (Equals(dateStr, "TODAY(", 6)) {
		int numberOfDays = 0;
		t.SetToNow();
		sprintf(str, dateStr.Right(dateStr.GetLength() - 6));
		char *right = strchr(str, ')');
		if (NULL != right) {
			right[0] = '\0';
		}
		if (sscanf(str, "%d", &numberOfDays)) {
			if (numberOfDays < 0) {
				for (int k = 0; k > numberOfDays; --k) {
					t.DecrementOneDay();
				}
			}
			else if (numberOfDays > 0) {
				for (int k = 0; k < numberOfDays; ++k) {
					t.IncrementOneDay();
				}
			}
		}
		return true;
	}

	// guess that the year-month-day might be separated with dots
	pt = strstr(str, ".");
	if (pt != NULL) {
		pt2 = strstr(pt + 1, ".");
		if (pt2 == NULL) {
			return false;
		}
		else {
			if (3 == sscanf(str, "%d.%d.%d", &y, &m, &d)) {
				t.year = y;
				t.month = m;
				t.day = d;
				return true;
			}
			else {
				return false;
			}
		}
	}

	// guess that the year-month-day might be separated with colons
	pt = strstr(str, ":");
	if (pt != NULL) {
		pt2 = strstr(pt + 1, ":");
		if (pt2 == NULL) {
			return false;
		}
		else {
			if (3 == sscanf(str, "%d:%d:%d", &y, &m, &d)) {
				t.year = y;
				t.month = m;
				t.day = d;
				return true;
			}
			else {
				return false;
			}
		}
	}

	// guess that the year-month-day might be separated with nothing
	int N = (int)strlen(str);
	if (N != 8)
		return false;

	if (0 == sscanf(&str[N - 2], "%d", &d))
		return false;
	str[N - 2] = '\0';
	if (0 == sscanf(&str[N - 4], "%d", &m))
		return false;
	str[N - 4] = '\0';
	if (0 == sscanf(&str[0], "%d", &y))
		return false;

	t.year = y;
	t.month = m;
	t.day = d;
	return true;
}
