#pragma once

#include <string>
#include <vector>
#include <SpectralEvaluation/Calibration/StandardCrossSectionSetup.h>

namespace novac
{
    class CDateTime;
    class CPostCalibrationStatistics;

    /** The CPostCalibration class is the helper class for performing instrument calibrations in the
        NovacPostProcessingProgram. This operates on measured .pak files and produces instrument calibrations
        which can be used later. */
    class CPostCalibration
    {
    public:

        CPostCalibration(const novac::StandardCrossSectionSetup& standardCrossSections)
            : m_standardCrossSections(standardCrossSections)
        {
        }

        /** Performs an automatic instrument calibration for the supplied spectrometer using the provided
            .pak file for measurement data.
            If the scan is good enough for performing the calibration, an instrument calibration will be created and returned
                as well as a set of
            @return The number of successful calibrations.*/
        int RunInstrumentCalibration(const std::vector<std::string>& scanFileList, CPostCalibrationStatistics& statistics);

    private:

        const novac::StandardCrossSectionSetup m_standardCrossSections;

        /** Performs an automatic instrument calibration for the supplied spectrometer using the provided
            .pak file for measurement data.
            If the scan is good enough for performing the calibration, an instrument calibration will be created and returned
                as well as a set of
            @return true if the calibration succeeded.*/
        bool RunInstrumentCalibration(const std::string& scanFile, CPostCalibrationStatistics& statistics);

    };
}