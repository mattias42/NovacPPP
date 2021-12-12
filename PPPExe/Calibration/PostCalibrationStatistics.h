#pragma once

#include <map>
#include <string>
#include <vector>

#include <SpectralEvaluation/DateTime.h>
#include <SpectralEvaluation/Evaluation/ReferenceFile.h>

namespace novac
{
    class CPostCalibrationStatistics
    {
    public:

        void RememberCalibrationPerformed(
            const std::string& instrumentSerial,
            const novac::CDateTime& gpsTimeOfScan,
            const std::vector<novac::CReferenceFile>& referencesCreated);


        /** Returns the last time a calibration was performed for the given spectrometers.
            @returns the Gps timestamp of the calibrated spectrum OR default (year=0, month=0,...) if none has been performed. */
        novac::CDateTime LastCalibrationPerformed(const std::string& instrumentSerial) const;

        // TODO: Now that all the references have been successfully created and remembered, figure out a way such that can build up the evaluation setting.

    private:
        struct InstrumentCalibration
        {
            // The time stamp of the calibrated scans. This is the time the scan started (utc).
            novac::CDateTime calibrationTimeStamp;

            std::vector<novac::CReferenceFile> referenceFiles;
        };

        struct InstrumentCalibrationCollection
        {
            // The list of calibrations performed for this particular instrument.
            // This list shall be sorted in increasing time such that the last scan is last in the list.
            std::vector<InstrumentCalibration> calibrationsPerformed;
        };

        // Map of instrument serial numbers and calibration properties
        std::map<std::string, InstrumentCalibrationCollection> m_calibrations;

    };
}