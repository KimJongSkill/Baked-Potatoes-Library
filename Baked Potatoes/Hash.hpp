#pragma once
#include <array>
#include <cstdint>
#include <list>
#include <memory>
#include <tuple>
#include "Utility.hpp"

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
			class HashInterface : public bpl::utility::Uncopyable
			{
			public:
				virtual ~HashInterface() = default;

				virtual void Reset() = 0;

				virtual void Update(const void* Message, std::size_t Length) = 0;
				virtual void Update(const std::string& Message) = 0;

				virtual std::string Hash() = 0;
				virtual std::string Hash(const void* Message, std::size_t Length) = 0;
				virtual std::string Hash(const std::string& Message) = 0;

				virtual const std::size_t BlockSize() const = 0; // In Bytes
				virtual const std::size_t DigestSize() const = 0; // In Bits
			};

			class SHA1Context : public HashInterface
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

				SHA1Context(SHA1Context&&);
				SHA1Context& operator=(SHA1Context&&);

				void Reset() override;

				void Update(const void* Message, std::size_t Length) override;
				void Update(const std::string& Message) override;

				std::string Hash() override;
				std::string Hash(const void* Message, std::size_t Length) override;
				std::string Hash(const std::string& Message) override;

				const std::size_t BlockSize() const override; // In Bytes
				const std::size_t DigestSize() const override; // In Bits
			};
		}
	}
}