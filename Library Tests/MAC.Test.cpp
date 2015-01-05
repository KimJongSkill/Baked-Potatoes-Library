#include "targetver.h"
#include "CppUnitTest.h"
#include "MAC.hpp"
#include "Utility.hpp"
#include <sstream>
#include <iomanip>
#include <array>
#include <intrin.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace LibraryTests
{
	TEST_CLASS(MACTest)
	{
		/*
		All Test Vectors courtesy of FIPS-198a
		http://csrc.nist.gov/publications/fips/fips198/fips-198a.pdf
		unless otherwise mentioned
		*/

		bpl::crypt::MAC::HMAC_SHA1 HMAC;

		TEST_METHOD(Key64)
		{
			std::string Message = "Sample #1";
			std::array<uint32_t, 16> KeyArray =
			{
				0x00010203, 0x04050607, 0x08090a0b, 0x0c0d0e0f,
				0x10111213, 0x14151617, 0x18191a1b, 0x1c1d1e1f,
				0x20212223, 0x24252627, 0x28292a2b, 0x2c2d2e2f,
				0x30313233, 0x34353637, 0x38393a3b, 0x3c3d3e3f
			};
			std::array <uint32_t, 16> BigEndianKeyArray;
			std::transform(KeyArray.begin(), KeyArray.end(), BigEndianKeyArray.begin(), &_byteswap_ulong);
			std::string Key(reinterpret_cast<const char*>(&BigEndianKeyArray), 64);
			std::string Expected = "4f4ca3d5d68ba7cc0a1208c9c61e9c5da0403c0a";

			auto Result = HMAC(Key, Message);

			Assert::AreEqual(Expected.c_str(), bpl::utility::ByteToHexString(Result).c_str());
		}

		TEST_METHOD(StringKey)
		{
			std::string Message = "Hello World";
			std::string Key = "My Very Secret Password";
			std::string Expected = "ebdc867835d067237b2f384f5eded735a220246b";

			Assert::AreEqual(Expected.c_str(), bpl::utility::ByteToHexString(HMAC(Key, Message)).c_str());
		}

		TEST_METHOD(RFC2202)
		{
			// https://tools.ietf.org/html/rfc2202

			std::string Message1 = "Hi There";
			std::string Key1(20, 0x0b);
			
			Assert::AreEqual("b617318655057264e28bc0b6fb378c8ef146be00", bpl::utility::ByteToHexString(HMAC(Key1, Message1)).c_str());

			std::string Message2 = "what do ya want for nothing?";
			std::string Key2 = "Jefe";

			Assert::AreEqual("effcdf6ae5eb2fa2d27416d5f184df9c259a7c79", bpl::utility::ByteToHexString(HMAC(Key2, Message2)).c_str());

			std::string Message3(50, 0xdd);
			std::string Key3(16, 0xaa);

			Assert::AreEqual("125d7342b9ac11cd91a39af48aa17b4f63f175d3", bpl::utility::ByteToHexString(HMAC(Key3, Message3)).c_str());

			std::string Message4 = "0102030405060708090a0b0c0d0e0f10111213141516171819";
			std::string Key4(50, 0xcd);

			Assert::AreEqual("4c9007f4026250c6bc8414f9bf50c86c2d7235da", bpl::utility::ByteToHexString(HMAC(Key4, Message4)).c_str());

			std::string Message5 = "Test With Truncation";
			std::string Key5(20, 0x0c);

			Assert::AreEqual("4c9007f4026250c6bc8414f9bf50c86c2d7235da", bpl::utility::ByteToHexString(HMAC(Key5, Message5)).c_str());

		}
	};
}