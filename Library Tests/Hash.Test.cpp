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

			Assert::AreEqual("2fd4e1c67a2d28fced849ee1bb76e7391b93eb12", ToHex(SHA1.Hash("The quick brown fox jumps over the lazy dog")).c_str());
			Assert::AreEqual("a7631795f6d59cd6d14ebd0058a6394a4b93d868", ToHex(SHA1.Hash("Test vector from febooti.com")).c_str());
			Assert::AreEqual("da39a3ee5e6b4b0d3255bfef95601890afd80709", ToHex(SHA1.Hash("")).c_str());
		}

		TEST_METHOD(ContextResetsOnHash)
		{
			auto First = SHA1.Hash("Hello World");
			auto Second = SHA1.Hash("Hello World");

			Assert::IsTrue(std::equal(First.begin(), First.end(), Second.begin()));
		}

		TEST_METHOD(WordGeneration)
		{
			std::string Input;
			Input.reserve(64);

			for (std::size_t i = 0; i < 64; ++i)
			{
				Input.push_back('\f'); // 0x0c
			}

			auto Words = SHA1.ToWords(Input);

			for (auto&& i : Words.first)
			{
				Assert::AreEqual(static_cast<uint32_t>(0x0c0c0c0c), i);
			}
			Assert::AreEqual((std::uint32_t) 0, Words.second.first);
			Assert::AreEqual((std::size_t) 0, Words.second.second);
		}

		TEST_METHOD(WordGenerationComplex)
		{
			std::string Input;
			Input.reserve(67);

			for (std::size_t i = 0; i < 67; ++i)
			{
				Input.push_back('\f');
			}

			auto Words = SHA1.ToWords(Input);

			for (auto&& i : Words.first)
			{
				Assert::AreEqual(static_cast<uint32_t>(0x0c0c0c0c), i);
			}
			Assert::AreEqual((std::uint32_t) 0x0c0c0c00, Words.second.first);
			Assert::AreEqual((std::size_t) 3, Words.second.second);
		}

		TEST_METHOD(BlockHashing)
		{
			bpl::crypt::hash::SHA1Context::BigBuffer_t Words(16, 0);

			SHA1.HashBlock(std::move(Words));

			Assert::AreEqual((unsigned) 0x92b404e5, SHA1.Context->Result[0]);
			Assert::AreEqual((unsigned) 0x56588ced, SHA1.Context->Result[1]);
			Assert::AreEqual((unsigned) 0x6c1acd4e, SHA1.Context->Result[2]);
			Assert::AreEqual((unsigned) 0xbf053f68, SHA1.Context->Result[3]);
			Assert::AreEqual((unsigned) 0x09f73a93, SHA1.Context->Result[4]);
		}

		TEST_METHOD(Performance)
		{
			for (std::size_t i = 0; i < 10000; ++i)
				SHA1.Hash("Hello, World!");
		}

		TEST_METHOD(NSRL)
		{
			// http://www.nsrl.nist.gov/testdata/

			Assert::AreEqual("a9993e364706816aba3e25717850c26c9cd0d89d", ToHex(SHA1.Hash("abc")).c_str());
			Assert::AreEqual("84983e441c3bd26ebaae4aa1f95129e5e54670f1", ToHex(SHA1.Hash("abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq")).c_str());
			Assert::AreEqual("34aa973cd4c4daa4f61eeb2bdbad27316534016f", ToHex(SHA1.Hash(std::string(1000000, 'a'))).c_str());
		}
	};
}