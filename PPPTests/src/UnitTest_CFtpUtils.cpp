#include "catch.hpp"
#include <PPPLib/CFtpUtils.h>

namespace novac
{
	TEST_CASE("SplitPathIntoServerAndDirectory behaves as expected", "[CFtpUtils]")
	{
		SECTION("Splits server and directory")
		{
			CString path{"ftp://127.0.0.1/some/directory/path/"};

			std::string server, directory;
			CVolcanoInfo info;
			CFtpUtils utils{info, 0};
			utils.SplitPathIntoServerAndDirectory(path, server, directory);

			REQUIRE(server == "127.0.0.1");
			REQUIRE(directory == "some/directory/path/");
		}
	}
}