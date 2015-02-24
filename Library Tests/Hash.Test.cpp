#include "targetver.h"
#include "CppUnitTest.h"
#include "Hash.hpp"
#include <sstream>
#include <iomanip>
#include <array>
#include <algorithm>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace LibraryTests
{
	TEST_CLASS(SHA1Test)
	{
		bpl::crypt::hash::SHA1Context SHA1;

		std::string ToHex(const std::array<uint8_t, 20>& Result)
		{
			std::stringstream Stream;

			for (auto i : Result)
				Stream << std::setfill('0') << std::setw(2) << std::hex << static_cast<unsigned>(i);
			
			return Stream.str();
		}

		TEST_METHOD(Febooti)
		{
			// http://www.febooti.com/products/filetweak/members/hash-and-crc/test-vectors/
			std::string Test1 = "The quick brown fox jumps over the lazy dog";
			std::string Test2 = "Test vector from febooti.com";
			std::string Test3 = "";

			Assert::AreEqual("2fd4e1c67a2d28fced849ee1bb76e7391b93eb12", SHA1.Hash(Test1.c_str(), Test1.length()).c_str());
			Assert::AreEqual("a7631795f6d59cd6d14ebd0058a6394a4b93d868", SHA1.Hash(Test2.c_str(), Test2.length()).c_str());
			Assert::AreEqual("da39a3ee5e6b4b0d3255bfef95601890afd80709", SHA1.Hash(Test3.c_str(), Test3.length()).c_str());
		}

		TEST_METHOD(ContextResetsOnHash)
		{
			auto First = SHA1.Hash("Hello World", 11);
			auto Second = SHA1.Hash("Hello World", 11);

			Assert::IsTrue(std::equal(First.begin(), First.end(), Second.begin()));
		}

		TEST_METHOD(BlockHashing)
		{
			std::array<uint8_t, 64> Words;
			Words.fill(0);

			SHA1.HashBlock(Words.data());

			Assert::AreEqual((unsigned) 0x92b404e5, SHA1.Context->Result[0]);
			Assert::AreEqual((unsigned) 0x56588ced, SHA1.Context->Result[1]);
			Assert::AreEqual((unsigned) 0x6c1acd4e, SHA1.Context->Result[2]);
			Assert::AreEqual((unsigned) 0xbf053f68, SHA1.Context->Result[3]);
			Assert::AreEqual((unsigned) 0x09f73a93, SHA1.Context->Result[4]);
		}

		TEST_METHOD(Performance)
		{
			for (std::size_t i = 0; i < 10000; ++i)
				SHA1.Hash("Hello, World!", 13);
		}

		TEST_METHOD(FIPS180)
		{
			// http://csrc.nist.gov/publications/fips/fips180-2/fips180-2withchangenotice.pdf
			std::string Test1 = "abc";
			std::string Test2 = "abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq";
			std::string Test3(1000000, 'a');

			Assert::AreEqual("a9993e364706816aba3e25717850c26c9cd0d89d", SHA1.Hash(Test1.c_str(), Test1.length()).c_str());
			Assert::AreEqual("84983e441c3bd26ebaae4aa1f95129e5e54670f1", SHA1.Hash(Test2.c_str(), Test2.length()).c_str());
			Assert::AreEqual("34aa973cd4c4daa4f61eeb2bdbad27316534016f", SHA1.Hash(Test3.c_str(), Test3.length()).c_str());
		}
	};
}