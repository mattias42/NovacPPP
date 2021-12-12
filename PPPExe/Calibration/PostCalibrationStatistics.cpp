#include "PostCalibrationStatistics.h"
#include <algorithm>

using namespace novac;

void CPostCalibrationStatistics::RememberCalibrationPerformed(
    const std::string& instrumentSerial,
    const novac::CDateTime& gpsTimeOfScan,
    const std::vector<novac::CReferenceFile>& referencesCreated)
{
    InstrumentCalibration newCalibration;
    newCalibration.calibrationTimeStamp = gpsTimeOfScan;
    newCalibration.referenceFiles = referencesCreated;

    auto pos = m_calibrations.find(instrumentSerial);

    if (pos == m_calibrations.end())
    {
        InstrumentCalibrationCollection newCollection;
        newCollection.calibrationsPerformed.push_back(newCalibration);
        m_calibrations[instrumentSerial] = newCollection;
    }
    else
    {
        InstrumentCalibrationCollection& collection = pos->second;
        collection.calibrationsPerformed.push_back(newCalibration);

        // keep the collection sorted
        std::sort(
            begin(collection.calibrationsPerformed),
            end(collection.calibrationsPerformed),
            [](const InstrumentCalibration& c1, const InstrumentCalibration& c2)
                {
                    return c1.calibrationTimeStamp < c2.calibrationTimeStamp;
                });
    }
}
