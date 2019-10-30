#include "PlumeHeight.h"

namespace Geometry
{
    CPlumeHeight::CPlumeHeight()
    {
        this->m_plumeAltitude = 1000.0;
        this->m_plumeAltitudeError = 1000.0;
        this->m_plumeAltitudeSource = Meteorology::MET_DEFAULT;

        m_validFrom = CDateTime(0, 0, 0, 0, 0, 0);
        m_validTo = CDateTime(9999, 12, 31, 23, 59, 59);
    }

    CPlumeHeight &CPlumeHeight::operator=(const CPlumeHeight &ph2)
    {
        this->m_plumeAltitude = ph2.m_plumeAltitude;
        this->m_plumeAltitudeError = ph2.m_plumeAltitudeError;
        this->m_plumeAltitudeSource = ph2.m_plumeAltitudeSource;

        this->m_validFrom = ph2.m_validFrom;
        this->m_validTo = ph2.m_validTo;

        return *this;
    }
}
