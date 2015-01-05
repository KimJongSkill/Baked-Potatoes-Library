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
				
				struct Data
				{
					std::array<uint32_t, 5> Result;
					std::pair<std::array<uint8_t, 64>, std::size_t> Buffer;
					uint64_t TotalLength = 0;

					Data() : Result({ { 0x67452301, 0xEFCDAB89, 0x98BADCFE, 0x10325476, 0xC3D2E1F0 } }) 
					{
						Buffer.first.fill(0);
						Buffer.second = 0;
					};
				};

				std::unique_ptr<Data> Context;

				void HashBlock(const void* Block);
				void HashBuffer();

			public:
				SHA1Context();

				SHA1Context(const SHA1Context&) = delete;
				SHA1Context operator=(const SHA1Context&) = delete;

				SHA1Context(SHA1Context&&);
				SHA1Context& operator=(SHA1Context&&);

				void Reset();

				void Update(const void* Message, std::size_t Length);

				std::array<uint8_t, 20> Hash();
				std::array<uint8_t, 20> Hash(const void* Message, std::size_t Length);

				// In Bytes
				static const std::size_t BlockSize = 64;

				// In Bits
				static const std::size_t DigestSize = 160;
			};
		}
	}
}