#include <PPPLib/File/EvaluationConfigurationParser.h>
#include "catch.hpp"
#include "StdOutLogger.h"

namespace novac
{
    static std::string GetTestDataDirectory()
    {
#ifdef _MSC_VER
        return std::string("../testData/");
#else
        return std::string("testData/");
#endif // _MSC_VER 
    }

    static std::string GetEvaluationConfigurationFile()
    {
        return GetTestDataDirectory() + std::string("I2J8552.exml");
    }

    TEST_CASE("ReadConfigurationFile gives expected configuration", "[EvaluationConfigurationParser][File]")
    {
        StdOutLogger logger;
        Configuration::CEvaluationConfiguration resultingEvaluationSettings;
        Configuration::CDarkCorrectionConfiguration resultingDarkSettings;
        Configuration::CInstrumentCalibrationConfiguration resultingCalibrationSettings;
        FileHandler::CEvaluationConfigurationParser sut{ logger };

        int returnCode = sut.ReadConfigurationFile(
            GetEvaluationConfigurationFile(),
            resultingEvaluationSettings,
            resultingDarkSettings,
            resultingCalibrationSettings);

        REQUIRE(returnCode == 0);

        // Expected evaluation settings
        {
            REQUIRE("I2J8552" == resultingEvaluationSettings.m_serial);
            REQUIRE(3 == resultingEvaluationSettings.GetFitWindowNum());

        }

        // Expected first evaluation fit window
        {
            novac::CFitWindow window;
            novac::CDateTime validFrom;
            novac::CDateTime validTo;

            int result = resultingEvaluationSettings.GetFitWindow(0, window, validFrom, validTo);

            REQUIRE(result == 0);
            REQUIRE(validFrom == novac::CDateTime(0, 0, 0, 0, 0, 0));
            REQUIRE(validTo == novac::CDateTime(2017, 2, 20, 5, 49, 1));

            REQUIRE(window.nRef == 3);
            REQUIRE(window.ref[0].m_path == "D:\\NovacPostProcessingProgram\\TestRun_2021_12\\OutputFeb2017UTC\\2017.02.20/I2J8552/I2J8552_SO2_Bogumil_293K_170220_0348.txt");
            REQUIRE(window.fraunhoferRef.m_path == "D:\\NovacPostProcessingProgram\\TestRun_2021_12\\OutputFeb2017UTC\\2017.02.20/I2J8552/I2J8552_Fraunhofer_170220_0348.txt");
        }

        // Expected second evaluation fit window
        {
            novac::CFitWindow window;
            novac::CDateTime validFrom;
            novac::CDateTime validTo;

            int result = resultingEvaluationSettings.GetFitWindow(1, window, validFrom, validTo);

            REQUIRE(result == 0);
            REQUIRE(validFrom == novac::CDateTime(2017, 2, 20, 5, 49, 1));
            REQUIRE(validTo == novac::CDateTime(2017, 2, 20, 9, 53, 24));

            REQUIRE(window.nRef == 3);
            REQUIRE(window.ref[0].m_path == "D:\\NovacPostProcessingProgram\\TestRun_2021_12\\OutputFeb2017UTC\\2017.02.20/I2J8552/I2J8552_SO2_Bogumil_293K_170220_0749.txt");
            REQUIRE(window.fraunhoferRef.m_path == "D:\\NovacPostProcessingProgram\\TestRun_2021_12\\OutputFeb2017UTC\\2017.02.20/I2J8552/I2J8552_Fraunhofer_170220_0749.txt");
        }

        // Expected third evaluation fit window
        {
            novac::CFitWindow window;
            novac::CDateTime validFrom;
            novac::CDateTime validTo;

            int result = resultingEvaluationSettings.GetFitWindow(2, window, validFrom, validTo);

            REQUIRE(result == 0);
            REQUIRE(validFrom == novac::CDateTime(2017, 2, 20, 9, 53, 24));
            REQUIRE(validTo == novac::CDateTime(9999, 12, 31, 23, 59, 59));

            REQUIRE(window.nRef == 3);
            REQUIRE(window.ref[0].m_path == "D:\\NovacPostProcessingProgram\\TestRun_2021_12\\OutputFeb2017UTC\\2017.02.20/I2J8552/I2J8552_SO2_Bogumil_293K_170220_1157.txt");
            REQUIRE(window.fraunhoferRef.m_path == "D:\\NovacPostProcessingProgram\\TestRun_2021_12\\OutputFeb2017UTC\\2017.02.20/I2J8552/I2J8552_Fraunhofer_170220_1157.txt");
        }

        // Expected dark settings
        {
            REQUIRE(1 == resultingDarkSettings.GetSettingsNum());
            novac::CDateTime requestTime{ 2017, 2, 20, 10, 0, 0 };
            Configuration::CDarkSettings darkSettings;

            int result = resultingDarkSettings.GetDarkSettings(darkSettings, requestTime);

            REQUIRE(result == 0);
            REQUIRE(darkSettings.m_darkSpecOption == Configuration::DARK_SPEC_OPTION::MEASURED_IN_SCAN);
        }

        // Expected calibration settings
        {
            REQUIRE("C:\\NOVAC\\novacP3\\Cross sections\\I2J8552_SolarSpec.xs" == resultingCalibrationSettings.m_initialCalibrationFile);
        }
    }
}