#include "gpsdata.h"

#include <math.h>

CGPSData::CGPSData(void)
{
  this->m_altitude  = 0;
  this->m_latitude  = 0.0f;
  this->m_longitude = 0.0f;
}

CGPSData::CGPSData(const CGPSData &gps2){
  this->m_altitude = gps2.m_altitude;
  this->m_latitude = gps2.m_latitude;
  this->m_longitude = gps2.m_longitude;
}

CGPSData::CGPSData(double lat, double lon, double alt){
  this->m_altitude = alt;
  this->m_latitude = lat;
  this->m_longitude = lon;
}

CGPSData::~CGPSData(void)
{
}

CGPSData &CGPSData::operator =(const CGPSData &gps2){
  this->m_altitude = gps2.Altitude();
  this->m_latitude = gps2.Latitude();
  this->m_longitude= gps2.Longitude();
  return *this;
}

bool CGPSData::operator ==(const CGPSData &gps2) const{
	if(fabs(this->m_latitude - gps2.m_latitude) > 1e-2)
		return false;
	if(fabs(this->m_longitude - gps2.m_longitude) > 1e-2)
		return false;
	return true;
}

/* The GPS reports latitude and longitude in the format ddmm.mmmm
  , this function converts this to the format dd.dddd */
double CGPSData::DoubleToAngle(double rawData){
	double degree, remainder, fDegree;

	remainder	= fmod(rawData, 100.0);
	degree		= floor(rawData * 0.01);
	fDegree		= degree + remainder * 0.0166666666666667; // remainder / 60.0

	return fDegree;
}