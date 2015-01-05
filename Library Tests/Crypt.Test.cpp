#include "targetver.h"
#include "CppUnitTest.h"
#include "Crypt.hpp"
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
				bpl::crypt::ConstantTimeEquals("1", "1", 2);
			auto TrueAverage = std::chrono::steady_clock::now() - Start;

			Start = std::chrono::steady_clock::now();
			for (int i = 0; i < 100000; ++i)
				bpl::crypt::ConstantTimeEquals("1", "1", 2);
			auto FalseAverage = std::chrono::steady_clock::now() - Start;
			
			Assert::AreEqual(static_cast<double>(TrueAverage.count()), static_cast<double>(FalseAverage.count()), static_cast<double>(10)); // This may fail occasionally
		}
	};
}