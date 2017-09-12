#include <PPPLib/CList.h>
#include "catch.hpp"

namespace novac
{
	TEST_CASE("CList Construction behaves as expected", "[CList]")
	{
		SECTION("Default constructor")
		{
			CList<int> defaultContructedCList;
			REQUIRE(defaultContructedCList.GetSize() == 0);
			REQUIRE(defaultContructedCList.GetCount() == 0);
		}

	}
}