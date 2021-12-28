#pragma once

#include "XMLFileReader.h"
#include <PPPLib/Configuration/EvaluationConfiguration.h>
#include <PPPLib/Configuration/DarkCorrectionConfiguration.h>
#include <PPPLib/Configuration/InstrumentCalibrationConfiguration.h>

namespace FileHandler {
    class CEvaluationConfigurationParser : public CXMLFileReader
    {
    public:
        /** Reads in an evaluation-configuration file.
            In the format specified for the NovacPostProcessingProgram (NPPP)
            @return 0 on sucess */
        int ReadConfigurationFile(
            const novac::CString& fileName,
            Configuration::CEvaluationConfiguration& settings,
            Configuration::CDarkCorrectionConfiguration& darkSettings,
            Configuration::CInstrumentCalibrationConfiguration& calibrationSettings);

        /** Writes an evaluation configuration file in the NPPP-format
            @return 0 on success */
        int WriteConfigurationFile(
            const novac::CString& fileName,
            const Configuration::CEvaluationConfiguration& settings,
            const Configuration::CDarkCorrectionConfiguration& darkSettings,
            const Configuration::CInstrumentCalibrationConfiguration& calibrationSettings);

    private:

        /** Reads a 'fitWindow' section */
        int Parse_FitWindow(novac::CFitWindow& window, novac::CDateTime& validFrom, novac::CDateTime& validTo);

        /** Reads a 'Reference' section */
        int Parse_Reference(novac::CFitWindow& window);

        /** Reads a 'dark-correction' section */
        int Parse_DarkCorrection(Configuration::CDarkSettings& dSettings, novac::CDateTime& validFrom, novac::CDateTime& validTo);

        int Parse_CalibrationSettings(Configuration::CInstrumentCalibrationConfiguration& calibrationSettings);

    };
}
