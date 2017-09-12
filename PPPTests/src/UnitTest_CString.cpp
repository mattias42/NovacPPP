#include <PPPLib/CString.h>
#include "catch.hpp"

namespace novac
{
	TEST_CASE("CString Construction behaves as expected", "[CString]")
	{
		SECTION("Default constructor")
		{
			CString defaultContructedCString;
			REQUIRE(defaultContructedCString.ToStdString() == "");
			REQUIRE(defaultContructedCString.GetLength() == 0);
		}

		SECTION("Std::string constructor")
		{
			std::string original = "Mary had a little lamb";

			CString copiedString{ original };
			
			REQUIRE(copiedString.ToStdString() == original);
		}

		SECTION("const char* constructor")
		{
			char original[] = "Mary had a little lamb";

			CString copiedString{ original };

			const char* contents = copiedString;

			REQUIRE(strcmp(contents, original) == 0);
		}
	}

	TEST_CASE("Properties behaves as expected", "[CString]")
	{
		std::string original = "Mary had a little lamb";
		CString sut{ original };

		SECTION("GetLength")
		{
			REQUIRE(original.size() == sut.GetLength());
		}
	}

	TEST_CASE("Substrings behaves as expected", "[CString]")
	{
		std::string original = "Mary had a little lamb";
		CString sut{ original };

		SECTION("Left - 5")
		{
			CString fiveLeftMostCharacters = sut.Left(5);

			REQUIRE(5 == fiveLeftMostCharacters.GetLength());
			REQUIRE(fiveLeftMostCharacters.ToStdString() == "Mary ");
		}

		SECTION("Left - 0")
		{
			CString zeroLeftMostCharacters = sut.Left(0);

			REQUIRE(0 == zeroLeftMostCharacters.GetLength());
			REQUIRE(zeroLeftMostCharacters.ToStdString() == "");
		}

		SECTION("Left - size + 1")
		{
			CString allLeftMostCharacters = sut.Left(sut.GetLength() + 1);

			REQUIRE(sut.GetLength() == allLeftMostCharacters.GetLength());
			REQUIRE(allLeftMostCharacters.ToStdString() == sut.ToStdString());
		}

		SECTION("Right - 5")
		{
			CString fiveRightMostCharacters = sut.Right(5);

			REQUIRE(5 == fiveRightMostCharacters.GetLength());
			REQUIRE(fiveRightMostCharacters.ToStdString() == " lamb");
		}

		SECTION("Right - 0")
		{
			CString zeroRightMostCharacters = sut.Right(0);

			REQUIRE(0 == zeroRightMostCharacters.GetLength());
			REQUIRE(zeroRightMostCharacters.ToStdString() == "");
		}

		SECTION("Right - size + 1")
		{
			CString allRightMostCharacters = sut.Right(sut.GetLength() + 1);

			REQUIRE(sut.GetLength() == allRightMostCharacters.GetLength());
			REQUIRE(allRightMostCharacters.ToStdString() == sut.ToStdString());
		}
	}

	TEST_CASE("Format behaves as expected", "[CString]")
	{
		const std::string original = "Twinkle, twinkle little star";

		SECTION("String - char array")
		{
			CString sut;
			sut.Format("%s", original.data());

			REQUIRE(sut.ToStdString() == original);
		}

		SECTION("String - CString")
		{
			CString copy{original}; // this will have the same contents as the original

			CString sut;
			sut.Format("%s", (const char*)copy);

			REQUIRE(sut.ToStdString() == original);
		}

	}
}