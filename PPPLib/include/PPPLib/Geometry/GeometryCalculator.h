#pragma once

#include <SpectralEvaluation/Log.h>
#include <SpectralEvaluation/GPSData.h>
#include <SpectralEvaluation/Flux/PlumeInScanProperty.h>
#include <PPPLib/Meteorology/WindField.h>
#include <PPPLib/Geometry/GeometryResult.h>
#include <PPPLib/Geometry/PlumeHeight.h>
#include <PPPLib/Configuration/InstrumentLocation.h>
#include <PPPLib/MFC/CString.h>

namespace Configuration
{
class CUserConfiguration;
}

namespace Geometry
{

/** <b>CGeometryCalculator</b> contains generic methods for performing
        geometric calculations on the instrumental setup. */

class CGeometryCalculator
{
public:

    CGeometryCalculator(novac::ILogger& log, const Configuration::CUserConfiguration& userSettings);

    /** The class 'CGeometryCalculationInfo' is an auxiliary data storage
            to be able to handle the calculation of plume height and plume direction */
    class CGeometryCalculationInfo
    {
    public:
        CGeometryCalculationInfo();
        ~CGeometryCalculationInfo();
        void Clear();
        novac::CGPSData	scanner[2];
        double		plumeCentre[2];
        CGeometryCalculationInfo& operator=(const CGeometryCalculationInfo& info2);
    };

    /** Retrieve the plume height from a measurement using one scanning-instrument
            with an given assumption of the wind-direction 	*/
    static double GetPlumeHeight_OneInstrument(const novac::CGPSData source, const novac::CGPSData gps, double WindDirection, double alpha_center_of_mass, double phi_center_of_mass);

    /** Calculates the plume-height for a scan, assuming that the plume originates
                at the postition given in 'source' and that the centre of the plume is
                at the scan angle 'plumeCentre' (in degrees). The direction of the wind
                is given by 'windDirection' (in degrees from north).

                The properties of the scanner are given by the 'compass' - direction (degrees from north)
                and the 'coneAngle' (degrees)
                @return the plume height if the calculations are successful
                @return a not set Nullable if something is wrong. */
    static novac::Nullable<double> GetPlumeHeight(const novac::CGPSData source, double windDirection, const novac::CGPSData scannerPos, double compass, double plumeCentre, double coneAngle, double tilt);

    /** Calculate the plume-height using the two scans found in the
            given evaluation-files.
            @param result - will on successful return be filled with information on the result
                the resulting plume height is the altitude of the plume in meters above sea level...
            @return true on success */
    bool CalculateGeometry(const novac::CPlumeInScanProperty& plume1, const novac::CDateTime& startTime1, const novac::CPlumeInScanProperty& plume2, const novac::CDateTime& startTime2, const Configuration::CInstrumentLocation locations[2], Geometry::CGeometryResult& result);

    /** Calculate the plume-height using the scan found in the given evaluation-file.
            @param windDirection - the assumed wind-direction at the time the measurement was made
            @param result - will on successful return be filled with information on the result
                the resulting plume height is the altitude of the plume in meters above sea level...
                the estimated error is based on the error in wind-direction and the error in estimating
                    the plume centre position.
            @return true on success */
    bool CalculatePlumeHeight(const novac::CString& evalLog, size_t scanIndex, Meteorology::WindField& windField, Configuration::CInstrumentLocation location, CGeometryResult& result);

    /** Calculate the wind direction using the scan found in the given evaluation-file.
            @param absolutePlumeHeight - the assumed plume height (in meters above sea level)
                at the time the measurement was made
            @param result - will on successful return be filled with information on the result
                only the wind-direction (and its error) will be filled in
            @return true on success */
    bool CalculateWindDirection(const novac::CPlumeInScanProperty& plume, const novac::CDateTime& startTime, const PlumeHeight& absolutePlumeHeight, const Configuration::CInstrumentLocation& location, CGeometryResult& result);

private:

    const Configuration::CUserConfiguration& m_userSettings;

    novac::ILogger& m_log;

    /** Calculates the height of the plume given data from two scans
            @param gps - the gps-positions for the two scanning instruments
                    that collected the data
            @param compass - the compass-directions for the two scanning instruments
                    that collected the data. In degrees from north
            @param plumeCentre - the centre of the plume, as seen from each of
                    the two scanning instruments. Scan angle, in degrees
            @param plumeHeight - will on return be filled with the calculated
                    height of the plume above the lower of the two scanners
            @return true if a plume height could be calculated. */
    static bool GetPlumeHeight_Exact(const novac::CGPSData gps[2], const double compass[2], const double plumeCentre[2], const double coneAngle[2], const double tilt[2], double& plumeHeight);
    static bool GetPlumeHeight_Exact(const Configuration::CInstrumentLocation locations[2], const double plumeCentre[2], double& plumeHeight);

    /** Calculates the height of the plume given data from two scans
            @param gps - the gps-positions for the two scanning instruments
                    that collected the data
            @param compass - the compass-directions for the two scanning instruments
                    that collected the data. In degrees from north
            @param plumeCentre - the centre of the plume, as seen from each of
                    the two scanning instruments. Scan angle, in degrees
            @param plumeHeight - will on return be filled with the calculated
                    height of the plume above the lower of the two scanners
            @param windDirection - will on successful return be filled with the
                    calculated wind directions. In degrees from north, positive clockwise.
            @return true if a plume height could be calculated. */
    static bool GetPlumeHeight_Fuzzy(const novac::CGPSData source, const novac::CGPSData gps[2], const double compass[2], const double plumeCentre[2], const double coneAngle[2], const double tilt[2], double& plumeHeight, double& windDirection);
    static bool GetPlumeHeight_Fuzzy(const novac::CGPSData source, const Configuration::CInstrumentLocation locations[2], const double plumeCentre[2], double& plumeHeight, double& windDirection);

    /** Calculates the wind-direction for a scan, assuming that the plume originates
                at the postition given in 'source' and that the centre of the plume is
                at the scan angle 'plumeCentre' (in degrees). The height of the plume above
                the scanning instrument is given by 'plumeHeight' (in meters).
                The properties of the scanner are given by the 'compass' - direction (degrees from north)
                and the 'coneAngle' (degrees)
                @return the wind-direction if the calculations are successful
                @return NOT_A_NUMBER if something is wrong. s*/
    static double GetWindDirection(const novac::CGPSData source, double plumeHeight, const novac::CGPSData scannerPos, double compass, double plumeCentre, double coneAngle, double tilt);
    static double GetWindDirection(const novac::CGPSData source, double plumeHeight, const Configuration::CInstrumentLocation scannerLocation, double plumeCentre);

    /** Calculates the wind-direction for a scan, assuming that the plume originates
                at the postition given in 'source' and that the centre of the plume is
                at the scan angle 'plumeCentre' (in degrees). The height of the plume above
                the scanning instrument is given by 'plumeHeight' (in meters).
                This function is intended for use with V-II Heidelberg instruments
                @return the wind-direction if the calculations are successful
                @return NOT_A_NUMBER if something is wrong. */
    static double GetWindDirection(const novac::CGPSData source, const novac::CGPSData scannerPos, double plumeHeight, double alpha_center_of_mass, double phi_center_of_mass);

    /** Rotates the given vector the given angle [degrees] around the given axis
            @param vec - the coordiates of the vector
            @param angle - the angle to rotate, in degrees
            @param axis - the axis to rotate around (1,2 or 3)
            If axis is not 1,2 or 3 then nothing will be done.*/
    static void Rotate(double vec[3], double angle, int axis);

    /** Calculates the parameters t1 and t2 so that the lines 'origin1 + t1*direction1'
            intersects the line 'origin2 + t2*direction2'. If the lines cannot intersect
            t1 and t2 define the points of closest approach.
            If the lines are parallel, t1 and t2 will be set to 0 and the function will return false.
            @origin1 - the origin of the first ray
            @direction1 - the direction of the first ray, should be normalized
            @origin2 - the origin of the second ray
            @direction2 - the direction of the second ray, should be normalized
            @t1 - will on return be the parameter t1, as defined above
            @t2 - will on return be the parameter t2, as defined above
            @return true if the rays do intersect
            @return false if the rays don't intersect */
    static bool Intersection(const double o1[3], const double d1[3], const double o2[3], const double d2[3], double& t1, double& t2);

    /** Calculates the coordinates of the point (origin + t*direction) */
    static void PointOnRay(const double origin[3], const double direction[3], double t, double point[3]);

    /** Calculates the direction of a ray from a cone-scanner with the given angles.
            Direction defined as direction from scanner, in a coordinate system with
                the x-axis in the direction of the scanner, the z-axis in the vertical direction
                and the y-axis defined as to get a right-handed coordinate system */
    static void GetDirection(double direction[3], double scanAngle, double coneAngle, double tilt);

    /** Calculates the cross product of the supplied vectors */
    static void Cross(const double u[3], const double v[3], double result[3]);

    /** Calculates the squared norm of the supplied vector */
    static double Norm2(const double v[3]);

    /** Normalizes the supplied vector */
    static void Normalize(double v[3]);

    /** Calculates the determinant of a matrix whose columns are defined
            by the three supplied vectors */
    static double Det(const double c1[3], const double c2[3], const double c3[3]);

};
}