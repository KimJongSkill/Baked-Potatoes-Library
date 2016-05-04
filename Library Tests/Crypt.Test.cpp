#include "targetver.h"
#include "CppUnitTest.h"
#include "Crypt.hpp"
#include "Hash.hpp"
#include <array>
#include <chrono>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace Microsoft
{
	namespace VisualStudio 
	{
		namespace CppUnitTestFramework
		{
			template<> 
			static std::wstring ToString<unsigned short>(const unsigned short& t)    
			{ return std::to_wstring(t); }
		}
	}
}

namespace LibraryTests
{
	TEST_CLASS(IvyRNG_Test)
	{
		bpl::crypt::IvyRNG Generator;

		TEST_METHOD(SupportForRDRAND)
		{
			// On Ivy Bridge and newer it should return true
			Assert::IsTrue(Generator.SupportsRDRAND());
		}

		TEST_METHOD(Numbers)
		{
			uint16_t Short;
			uint32_t Int;
			uint64_t Long;

			Generator(Short);
			Generator(Int);
			Generator(Long);
			
			Assert::AreNotEqual((uint16_t) 0, Short);
			Assert::AreNotEqual((uint32_t) 0, Int);
			Assert::AreNotEqual((uint64_t) 0, Long);
		}

		TEST_METHOD(Range)
		{
			std::array<uint16_t, 11> Data;
			Data.fill(0);

			Generator(Data.begin(), Data.end());

			for (uint16_t i : Data)
				Assert::AreNotEqual((uint16_t) 0, i);
		}
	};

	TEST_CLASS(ConstantTimeEqualsTest)
	{
		TEST_METHOD(Equals)
		{
			Assert::IsTrue(bpl::crypt::ConstantTimeEquals("Hello World", "Hello World", 12));
			Assert::IsFalse(bpl::crypt::ConstantTimeEquals("hello World", "Hello World", 12));
			Assert::IsFalse(bpl::crypt::ConstantTimeEquals("Oh no!", "Oh yes", 7));
		}

		TEST_METHOD(ConstantTime)
		{
			auto Start = std::chrono::steady_clock::now();
			for (int i = 0; i < 100000; ++i)
				bpl::crypt::ConstantTimeEquals("111111", "111111", 7);
			auto TrueAverage = std::chrono::steady_clock::now() - Start;

			Start = std::chrono::steady_clock::now();
			for (int i = 0; i < 100000; ++i)
				bpl::crypt::ConstantTimeEquals("111111", "000000", 7);
			auto FalseAverage = std::chrono::steady_clock::now() - Start;
			
			Assert::AreEqual(static_cast<double>(TrueAverage.count()), static_cast<double>(FalseAverage.count()), static_cast<double>(10)); // This may fail occasionally
		}
	};

	TEST_CLASS(PBKDF2Test)
	{
		TEST_METHOD(RFC6070)
		{
			// https://www.ietf.org/rfc/rfc6070.txt
			
			auto Function = bpl::crypt::PBKDF2<bpl::crypt::hash::SHA1Context>;

			Assert::AreEqual("0c60c80f961f0e71f3a9b524af6012062fe037a6", Function("password", "salt", 1, 20, false).c_str());
			Assert::AreEqual("ea6c014dc72d6f8ccd1ed92ace1d41f0d8de8957", Function("password", "salt", 2, 20, false).c_str());
			Assert::AreEqual("4b007901b765489abead49d926f721d065a429c1", Function("password", "salt", 4096, 20, false).c_str());
			Assert::AreEqual("eefe3d61cd4da4e4e9945b3d6ba2158c2634e984", Function("password", "salt", 16777216, 20, false).c_str());
			Assert::AreEqual("3d2eec4fe41c849b80c8d83662c0e44a8b291a964cf2f07038", Function("passwordPASSWORDpassword", "saltSALTsaltSALTsaltSALTsaltSALTsalt", 4096, 25, false).c_str());
			Assert::AreEqual("56fa6aa75548099dcc37d7f03425e0c3", Function(std::string("pass\0word", 9), std::string("sa\0lt", 5), 4096, 16, false).c_str());
		}
	};
}