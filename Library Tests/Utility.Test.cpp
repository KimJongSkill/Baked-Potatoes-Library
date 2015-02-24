#include "targetver.h"
#include "CppUnitTest.h"
#include "Utility.hpp"
#include <cstdint>
#include <numeric>

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

		TEST_METHOD(BigEndianConversions)
		{
			std::array<uint8_t, 8> Byte;
			std::iota(Byte.begin(), Byte.end(), 0);
			std::array<uint8_t, 8> Uint64ExpectedByte;
			std::iota(Uint64ExpectedByte.rbegin(), Uint64ExpectedByte.rend(), 0);
			std::array<uint8_t, 4> Uint32ExpectedByte;
			std::iota(Uint32ExpectedByte.rbegin(), Uint32ExpectedByte.rend(), 0);
			std::array<uint8_t, 2> Uint16ExpectedByte;
			std::iota(Uint16ExpectedByte.rbegin(), Uint16ExpectedByte.rend(), 0);

			std::uint64_t* Uint64 = reinterpret_cast<std::uint64_t*>(Byte.data());
			std::uint64_t* Uint64Expected = reinterpret_cast<std::uint64_t*>(Uint64ExpectedByte.data());
			std::uint32_t* Uint32 = reinterpret_cast<std::uint32_t*>(Byte.data());
			std::uint32_t* Uint32Expected = reinterpret_cast<std::uint32_t*>(Uint32ExpectedByte.data());
			std::uint16_t* Uint16 = reinterpret_cast<std::uint16_t*>(Byte.data());
			std::uint16_t* Uint16Expected = reinterpret_cast<std::uint16_t*>(Uint16ExpectedByte.data());

			Assert::AreEqual(*Uint64Expected, bpl::utility::BigEndian64(*Uint64));
			Assert::AreEqual(*Uint32Expected, bpl::utility::BigEndian32(*Uint32));
			Assert::AreEqual(*Uint16Expected, bpl::utility::BigEndian16(*Uint16));
		}

		TEST_METHOD(LittleEndianConversions)
		{
			std::array<uint8_t, 8> Byte;
			std::iota(Byte.begin(), Byte.end(), 0);
			std::array<uint8_t, 8> Uint64ExpectedByte;
			std::iota(Uint64ExpectedByte.begin(), Uint64ExpectedByte.end(), 0);
			std::array<uint8_t, 4> Uint32ExpectedByte;
			std::iota(Uint32ExpectedByte.begin(), Uint32ExpectedByte.end(), 0);
			std::array<uint8_t, 2> Uint16ExpectedByte;
			std::iota(Uint16ExpectedByte.begin(), Uint16ExpectedByte.end(), 0);

			std::uint64_t* Uint64 = reinterpret_cast<std::uint64_t*>(Byte.data());
			std::uint64_t* Uint64Expected = reinterpret_cast<std::uint64_t*>(Uint64ExpectedByte.data());
			std::uint32_t* Uint32 = reinterpret_cast<std::uint32_t*>(Byte.data());
			std::uint32_t* Uint32Expected = reinterpret_cast<std::uint32_t*>(Uint32ExpectedByte.data());
			std::uint16_t* Uint16 = reinterpret_cast<std::uint16_t*>(Byte.data());
			std::uint16_t* Uint16Expected = reinterpret_cast<std::uint16_t*>(Uint16ExpectedByte.data());

			Assert::AreEqual(*Uint64Expected, bpl::utility::LittleEndian64(*Uint64));
			Assert::AreEqual(*Uint32Expected, bpl::utility::LittleEndian32(*Uint32));
			Assert::AreEqual(*Uint16Expected, bpl::utility::LittleEndian16(*Uint16));
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