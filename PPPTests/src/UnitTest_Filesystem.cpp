#include <PPPLib/File/Filesystem.h>
#include "catch.hpp"

namespace novac
{
    TEST_CASE("AppendPathSeparator adds path separator if none exists already", "[Filesystem][AppendPathSeparator]")
    {
        CString originalPath = "C:\\SomeDirectory\\SubDirectory";

        CString result = Filesystem::AppendPathSeparator(originalPath);

#ifdef _MSC_VER
            REQUIRE(Equals(result, "C:\\SomeDirectory\\SubDirectory\\"));
#else
            REQUIRE(Equals(result, "C:\\SomeDirectory\\SubDirectory/"));
#endif // _MSC_VER 
    }

    TEST_CASE("AppendPathSeparator does not modify string if already exists with Windows path separator", "[Filesystem][AppendPathSeparator]")
    {
        CString originalPath = "C:\\SomeDirectory\\SubDirectory\\";

        CString result = Filesystem::AppendPathSeparator(originalPath);

        REQUIRE(Equals(result, "C:\\SomeDirectory\\SubDirectory\\"));
    }

    TEST_CASE("AppendPathSeparator does not modify string if already exists with Linux path separator", "[Filesystem][AppendPathSeparator]")
    {
        CString originalPath = "C:\\SomeDirectory\\SubDirectory/";

        CString result = Filesystem::AppendPathSeparator(originalPath);

        REQUIRE(Equals(result, "C:\\SomeDirectory\\SubDirectory/"));
    }
}