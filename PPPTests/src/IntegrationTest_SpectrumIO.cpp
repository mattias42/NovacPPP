#include <PPPLib/Spectra/SpectrumIO.h>
#include "catch.hpp"

namespace novac
{
extern novac::CString GetFileName();

	TEST_CASE("SpectrumIO CountSpectra", "[SpectrumIO][IntegrationTests]")
	{
		SpectrumIO::CSpectrumIO sut;
		int number = sut.CountSpectra(GetFileName());

		REQUIRE(53 == number);
	}

	TEST_CASE("SpectrumIO ReadSpectrum can read first spectrum from file", "[SpectrumIO][IntegrationTests]")
	{
		SpectrumIO::CSpectrumIO sut;
		CSpectrum spectrum;
		int returnValue = sut.ReadSpectrum(GetFileName(), 0, spectrum);

		REQUIRE(SUCCESS == returnValue);

		REQUIRE(spectrum.m_length == 2048);
		REQUIRE(spectrum.m_info.m_name == "sky");
		REQUIRE(spectrum.m_info.m_device == "I2J8549");

		REQUIRE(spectrum.m_info.m_startTime.year == 2017);
		REQUIRE(spectrum.m_info.m_startTime.month == 2);
		REQUIRE(spectrum.m_info.m_startTime.day == 16);
		REQUIRE(spectrum.m_info.m_startTime.hour == 12);
		REQUIRE(spectrum.m_info.m_startTime.minute == 30);

		REQUIRE(spectrum.m_info.m_scanIndex == 0);
	}
}