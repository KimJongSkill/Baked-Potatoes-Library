#include "targetver.h"
#include "CppUnitTest.h"
#include "Utility.hpp"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace Microsoft
{
	namespace VisualStudio
	{
		namespace CppUnitTestFramework
		{
			template<> 
			static std::wstring ToString<unsigned short>(const unsigned short& t)
			{ 
				RETURN_WIDE_STRING(t); 
			}
		}
	}
}

namespace LibraryTests
{
	TEST_CLASS(UtilityTest)
	{
		TEST_METHOD(Any)
		{
			struct Foo {};
			struct Bar
			{
				bool operator==(Foo)
				{
					return true;
				}
			};

			Assert::IsTrue(Bar() == bpl::utility::Any());
		}

		TEST_METHOD(EndianTest)
		{
			// System dependent
			Assert::IsTrue(bpl::utility::Endianness());
		}

		TEST_METHOD(RolTest)
		{
			std::uint16_t Value = 0x8001;
			
			Assert::AreEqual(static_cast<uint16_t>(0x6), bpl::utility::LeftRotate(Value, 2));
		}

		TEST_METHOD(RorTest)
		{
			std::uint16_t Value = 0x8001;

			Assert::AreEqual(static_cast<uint16_t>(0xC000), bpl::utility::RightRotate(Value, 1));
		}
	};
}