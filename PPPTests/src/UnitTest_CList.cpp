#include <PPPLib/CList.h>
#include "catch.hpp"

namespace novac
{
	TEST_CASE("POSITION")
	{
		SECTION("Assignment from nullptr")
		{
			POSITION<int> p2 = nullptr;

			REQUIRE_FALSE(p2.HasNext());
		}
	}

	TEST_CASE("REVERSE_POSITION")
	{
		SECTION("Assignment from nullptr")
		{
			REVERSE_POSITION<int> p2 = nullptr;

			REQUIRE_FALSE(p2.HasPrevious());
		}
	}


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

			auto p = sut.GetTailPosition();
			REQUIRE(2 == sut.GetAt(p));
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

			auto p = sut.GetTailPosition();
			REQUIRE(4 == sut.GetAt(p));
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

		SECTION("Forward in 5 item list")
		{
			CList<int> sut;
			sut.AddTail(1);
			sut.AddTail(2);
			sut.AddTail(3);
			sut.AddTail(4);
			sut.AddTail(5);

			int nItems = 0;

			auto p = sut.GetHeadPosition();
			while (p.HasNext())
			{
				++nItems;

				sut.GetNext(p);
			}

			REQUIRE(5 == nItems);
		}

		SECTION("Forward in 5 item list - Comparison with nullptr")
		{
			CList<int> sut;
			sut.AddTail(1);
			sut.AddTail(2);
			sut.AddTail(3);
			sut.AddTail(4);
			sut.AddTail(5);

			int nItems = 0;

			auto p = sut.GetHeadPosition();
			while (p != nullptr)
			{
				++nItems;

				sut.GetNext(p);
			}

			REQUIRE(5 == nItems);
		}

		SECTION("Backwards in 5 item list")
		{
			CList<int> sut;
			sut.AddTail(1);
			sut.AddTail(2);
			sut.AddTail(3);
			sut.AddTail(4);
			sut.AddTail(5);

			int nItems = 0;

			auto p = sut.GetTailPosition();
			while (p.HasPrevious())
			{
				++nItems;

				sut.GetPrev(p);
			}

			REQUIRE(5 == nItems);
		}

		SECTION("Backwards in 5 item list - Comparison with nullptr")
		{
			CList<int> sut;
			sut.AddTail(1);
			sut.AddTail(2);
			sut.AddTail(3);
			sut.AddTail(4);
			sut.AddTail(5);

			int nItems = 0;

			auto p = sut.GetTailPosition();
			while (p != nullptr)
			{
				++nItems;

				sut.GetPrev(p);
			}

			REQUIRE(5 == nItems);
		}
	}
}