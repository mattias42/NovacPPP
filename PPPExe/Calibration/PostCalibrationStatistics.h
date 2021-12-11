#pragma once

#include <map>

#include <SpectralEvaluation/DateTime.h>

namespace novac
{
    class CPostCalibrationStatistics
    {
    public:


    private:
        struct InstrumentCalibration
        {
            // The time stamps of the calibrated scans. This is the time the scan started (utc).
            // This list shall be sorted in increasing time such that the last scan is last in the list.
            std::vector<novac::CDateTime> calibrationTimeStamps;
        };

        // Map of instrument serial numbers and calibration properties
        std::map<std::string, InstrumentCalibration> m_calibrations;

    };
}