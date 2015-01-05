#pragma once
#include <array>
#include <cstdint>
#include <list>
#include <memory>
#include <tuple>

#ifdef UNIT_TESTING // Used for Unit Testing
namespace LibraryTests
{
	class SHA1Test;
}
#endif

namespace bpl
{
	namespace crypt
	{
		namespace hash
		{
			class SHA1Context
			{
#ifdef UNIT_TESTING // Give the Test Class access
				friend class ::LibraryTests::SHA1Test;
#endif
				using SmallBuffer_t = std::pair<std::uint32_t, std::size_t>;
				using BigBuffer_t = std::list<uint32_t>;
				using Buffer_t = std::pair<BigBuffer_t, SmallBuffer_t>;

				struct Data
				{
					std::array<uint32_t, 5> Result;
					uint64_t TotalLength = 0;
					BigBuffer_t BigBuffer;
					SmallBuffer_t SmallBuffer;

					Data() : Result({ { 0x67452301, 0xEFCDAB89, 0x98BADCFE, 0x10325476, 0xC3D2E1F0 } }) {};
				};

				std::unique_ptr<Data> Context;

				void HashBlock(const BigBuffer_t& Block);
				Buffer_t ToWords(const std::string& Block);
				void HashFromBuffer();

			public:
				SHA1Context();

				SHA1Context(const SHA1Context&) = delete;
				SHA1Context operator=(const SHA1Context&) = delete;

				SHA1Context(SHA1Context&&);
				SHA1Context& operator=(SHA1Context&&);

				void Reset();

				void Update(const std::string& Message);

				std::array<uint8_t, 20> Hash();
				std::array<uint8_t, 20> Hash(const std::string& Message);

				// In Bytes
				static const std::size_t BlockSize = 64;

				// In Bits
				static const std::size_t DigestSize = 160;
			};
		}
	}
}