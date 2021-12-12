#pragma once

#include <map>
#include <string>
#include <vector>
#include <SpectralEvaluation/DateTime.h>
#include <SpectralEvaluation/Calibration/StandardCrossSectionSetup.h>

namespace novac
{
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

        struct BasicScanInfo
        {
            novac::CDateTime startTime;
            std::string serial;
            int channel = 0;
            std::string fullPath;
        };

        struct SpectrometerId
        {
            SpectrometerId()
                : serial(""), channel(0) { }

            SpectrometerId(const std::string& serialNumber, int channelNumber)
                : serial(serialNumber), channel(channelNumber) { }

            std::string serial;
            int channel = 0;

            bool operator<(const SpectrometerId& other) const
            {
                int serialCompare = serial.compare(other.serial);
                if (serialCompare == 0)
                {
                    return channel < other.channel;
                }
                else
                {
                    return serialCompare < 0;
                }
            }
        };

        /** Arranges the provided list of scan files by the instrument which performed the measurement */
        static std::map<SpectrometerId, std::vector<BasicScanInfo>> SortScanFilesByInstrument(const std::vector<std::string>& scanFileList);

    };
}