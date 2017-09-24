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

	TEST_CASE("CList append and remove behaves as expected", "[CList]")
	{
		SECTION("AddTail - increases size")
		{
			CList<int> sut;
			REQUIRE(0 == sut.GetSize());
			sut.AddTail(2);
			REQUIRE(1 == sut.GetSize());
		}

		SECTION("AddTail - appends new element")
		{
			CList<int> sut;
			sut.AddTail(2);

			REQUIRE(2 == sut.GetAt(sut.GetTailPosition()));
		}

		SECTION("RemoveTail - removes last element")
		{
			CList<int> sut;
			sut.AddTail(2);
			sut.AddTail(4);
			sut.AddTail(8);
			REQUIRE(3 == sut.GetSize());

			sut.RemoveTail();

			REQUIRE(2 == sut.GetSize());

			REQUIRE(4 == sut.GetAt(sut.GetTailPosition()));
		}
	}

	TEST_CASE("CList iteration behaves as expected", "[CList]")
	{
		SECTION("Empty list")
		{
			CList<int> sut;
			int nItems = 0;

			auto p = sut.GetHeadPosition();
			while (p.HasNext())
			{
				++nItems;

				sut.GetNext(p);
			}

			REQUIRE(0 == nItems);
		}

		SECTION("5 item list")
		{

		}

	}
}