#pragma once
#include <PPPLib/SpectralEvaluation/Spectra/DateTime.h>
#include "../MeteorologySource.h"
#include <PPPLib/CString.h>

namespace Geometry{
	class CGeometryResult
	{
	public:
		/** Default constructor */
		CGeometryResult(void);

		/** Default destructor */
		~CGeometryResult(void);

		/** Assignement operator */
		CGeometryResult &operator=(const CGeometryResult &gr);
		
		/** How this calculation was made. Can be either one of
			MET_GEOMETRY_CALCULATION (default) or
			MET_GEOMETRY_CALCULATION_SINGLE_INSTR */
		Meteorology::MET_SOURCE m_calculationType;

		/** The average of the starting-time of the
				two scans that were combined to generate the result
				(seconds since midnight, UTC) */
		CDateTime m_averageStartTime;

		/** The difference in start-time between the two
			scans that were combined to make this measurement.
			In seconds. */
		int	m_startTimeDifference;

		/** The two instruments that were used to derive this 
			geometry result. */
		novac::CString m_instr1, m_instr2;

		/** The plume centre-angles for the two scans 
			that were combined */
		float	m_plumeCentre1, m_plumeCentre2;
		
		/** The estimated error in the plume-centre angles
			for each of the two scans that were combined */
		float	m_plumeCentreError1, m_plumeCentreError2;

		/** The calculated plume height (in meters above sea level) */
		double m_plumeAltitude;

		/** The estimated error in plume height (in meters) */
		double	m_plumeAltitudeError;

		/** The calculated wind-direction (degrees from north) */
		double	m_windDirection;

		/** The estimated error in the calculated wind-direction (degrees) */
		double	m_windDirectionError;
	};
}