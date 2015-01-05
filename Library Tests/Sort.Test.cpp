#include "targetver.h"
#include "CppUnitTest.h"
#include "Sort.hpp"
#include <algorithm>
#include <random>
#include <functional>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace LibraryTests
{		
	TEST_CLASS(MergesortTest)
	{
		std::vector<int> TestData;

	public:
		TEST_METHOD(Simple)
		{
			std::vector<int> Data { 5, 4, 3, 6, 1, };

			bpl::sort::Mergesort<bpl::sort::Ascending>(Data.begin(), Data.end());

			Assert::IsTrue(std::is_sorted(Data.begin(), Data.end()));
		}

		TEST_METHOD(Extended)
		{
			TestData = std::vector<int>(10000000);
			std::default_random_engine Generator;
			std::uniform_int_distribution<int> Distribution(-128, 127);
			std::generate(TestData.begin(), TestData.end(), std::bind(Distribution, Generator));

			bpl::sort::Mergesort<bpl::sort::Ascending>(TestData.begin(), TestData.end());

			Assert::IsTrue(std::is_sorted(TestData.begin(), TestData.end()));
		}

		TEST_METHOD(ValidArguments)
		{
			std::vector<int> TestData(2);

			bpl::sort::Mergesort<bpl::sort::Ascending>(TestData.begin(), TestData.end());
			bpl::sort::Mergesort<bpl::sort::Ascending>(TestData.begin(), TestData.begin());
		}

		TEST_METHOD(InvalidArguments)
		{
			std::vector<int> TestData(3);

			Assert::ExpectException<std::invalid_argument>(std::bind(bpl::sort::Mergesort<bpl::sort::Ascending, std::vector<int>::iterator>, TestData.end(), TestData.begin()));
		}

	};
}