#include "catch.hpp"
#include <PPPLib/MFC/CFileUtils.h>

namespace novac
{
    TEST_CASE("GetInfoFromFileName behaves as expected", "[CFileUtils]")
    {
        CDateTime start;
        CString serial;
        int channel;
        MEASUREMENT_MODE mode;

        SECTION("Returns false if unknown file name")
        {
            bool returnCode = CFileUtils::GetInfoFromFileName("Upload.pak", start, serial, channel, mode);
            REQUIRE(false == returnCode);

            returnCode = CFileUtils::GetInfoFromFileName("U001.pak", start, serial, channel, mode);
            REQUIRE(false == returnCode);
        }

        SECTION("Finds correct serial")
        {
            bool returnCode = CFileUtils::GetInfoFromFileName("D2J2134_170129_0317_1.pak", start, serial, channel, mode);
            REQUIRE(returnCode);
            REQUIRE(serial.std_str() == "D2J2134");

            returnCode = CFileUtils::GetInfoFromFileName("I2J98765_170129_0317_1.pak", start, serial, channel, mode);
            REQUIRE(returnCode);
            REQUIRE(serial.std_str() == "I2J98765");
        }

        SECTION("Finds correct date")
        {
            bool returnCode = CFileUtils::GetInfoFromFileName("D2J2134_170129_0317_1.pak", start, serial, channel, mode);

            REQUIRE(returnCode);
            REQUIRE(start.year == 2017);
            REQUIRE(start.month == 1);
            REQUIRE(start.day == 29);
            REQUIRE(start.hour == 3);
            REQUIRE(start.minute == 17);
            REQUIRE(start.second == 0);
        }

        SECTION("Finds correct channel")
        {
            bool returnCode = CFileUtils::GetInfoFromFileName("D2J2134_170129_0317_1.pak", start, serial, channel, mode);
            REQUIRE(returnCode);
            REQUIRE(channel == 1);

            returnCode = CFileUtils::GetInfoFromFileName("D2J2134_170129_0317_0.pak", start, serial, channel, mode);
            REQUIRE(returnCode);
            REQUIRE(channel == 0);
        }

        SECTION("Finds correct mode")
        {
            bool returnCode = CFileUtils::GetInfoFromFileName("D2J2134_170129_0317_1.pak", start, serial, channel, mode);
            REQUIRE(returnCode);
            REQUIRE(mode == MODE_FLUX); // default

            returnCode = CFileUtils::GetInfoFromFileName("D2J2134_170129_0317_1_wind.pak", start, serial, channel, mode);
            REQUIRE(returnCode);
            REQUIRE(mode == MODE_WINDSPEED);
        }
    }
}