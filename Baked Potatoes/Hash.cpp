#include "Hash.hpp"
#include <cassert>
#include <vector>
#include <utility>
#include <intrin.h>
#include "Utility.hpp"
#include "Counter.hpp"

namespace bpl
{
	namespace crypt
	{
		namespace hash
		{
			SHA1Context::SHA1Context() : Context(new Data) {};

			SHA1Context::SHA1Context(SHA1Context&& Other) : Context(Other.Context.release()) {};

			SHA1Context& SHA1Context::operator=(SHA1Context&& Other)
			{
				if (this != &Other)
				{
					Context.reset(Other.Context.release());
				}

				return *this;
			}

			void SHA1Context::Update(const void* Message, std::size_t Length)
			{
				if (!Length)
					return;

				Context->TotalLength += Length * 8; // Total Length is in bits

				if (Length == BlockSize() && !Context->Buffer.second)
					HashBlock(Message);
				else if (Length + Context->Buffer.second > BlockSize())
				{
					std::size_t Copied = BlockSize() - Context->Buffer.second;

					std::memcpy(&(Context->Buffer.first.data()[Context->Buffer.second]), Message, Copied);
					HashBuffer();

					const uint8_t* Pointer = static_cast<const uint8_t*>(Message) + Copied;
					while (Length - Copied >= 64)
					{
						HashBlock(Pointer);

						Copied += 64;
						Pointer += 64;
					}

					assert(Length - Copied >= 0);

					if (Length - Copied)
					{
						std::memcpy(Context->Buffer.first.data(), Pointer, Length - Copied);
						Context->Buffer.second = Length - Copied;
					}
				}
				else
				{
					std::memcpy(&(Context->Buffer.first.data()[Context->Buffer.second]), Message, Length);
					Context->Buffer.second += Length;
				}
			}

			std::string SHA1Context::Hash(bool RawOutput)
			{
				Context->Buffer.first[Context->Buffer.second++] = 0x80;
				
				// Total length won't fit; place it in the next block
				if (Context->Buffer.second > 56)
					HashBuffer();

				for (int i = 7; i >= 0; --i)
				{
					Context->Buffer.first[BlockSize() - i - 1] = (Context->TotalLength >> (i * 8)) & 0xff;
				}

				HashBuffer();

				std::string Digest(DigestSize() / 8, 0);
				for (std::size_t i = 0; i < 20; ++i)
				{
					Digest[i] = Context->Result[i / 4] >> ((3 - (i % 4)) * 8) & 0xff;
				}

				Reset();

				return RawOutput ? Digest : bpl::utility::ToHex(Digest);
			}

			std::string SHA1Context::Hash(const void* Message, std::size_t Length, bool RawOutput)
			{
				Update(Message, Length);

				return Hash(RawOutput);
			}

			void SHA1Context::HashBuffer()
			{
				HashBlock(Context->Buffer.first.data());
				Context->Buffer.first.fill(0);
				Context->Buffer.second = 0;
			}

			void SHA1Context::HashBlock(const void* Block)
			{
				std::array<uint32_t, 80> Words;
				Words.fill(0);
				std::memcpy(Words.data(), Block, BlockSize());
				std::transform(static_cast<const uint32_t*>(Block), static_cast<const uint32_t*>(Block) +16, Words.begin(), bpl::utility::BigEndian32);

				for (std::size_t i = 16; i < 80; ++i)
				{
					Words[i] = bpl::utility::LeftRotate(Words[i - 3] ^ Words[i - 8] ^ Words[i - 14] ^ Words[i - 16], 1);
				}

				std::uint32_t a = Context->Result[0];
				std::uint32_t b = Context->Result[1];
				std::uint32_t c = Context->Result[2];
				std::uint32_t d = Context->Result[3];
				std::uint32_t e = Context->Result[4];

				for (std::size_t i = 0; i < 80; ++i)
				{
					std::uint32_t f, k;

					if (i < 20)
					{
						f = (b & c) | (~b & d);
						k = 0x5A827999;
					}
					else if (i < 40)
					{
						f = b ^ c ^ d;
						k = 0x6ED9EBA1;
					}
					else if (i < 60)
					{
						f = (b & c) | (b & d) | (c & d);
						k = 0x8F1BBCDC;
					}
					else
					{
						f = b ^ c ^ d;
						k = 0xCA62C1D6;
					}

					std::uint32_t t = bpl::utility::LeftRotate(a, 5) + f + e + k + Words[i];
					e = d;
					d = c;
					c = bpl::utility::LeftRotate(b, 30);
					b = a;
					a = t;
				}

				Context->Result[0] += a;
				Context->Result[1] += b;
				Context->Result[2] += c;
				Context->Result[3] += d;
				Context->Result[4] += e;
			}

			void SHA1Context::Reset()
			{
				Context.reset(new Data);
			}

			void SHA1Context::Update(const std::string& Message)
			{
				Update(Message.c_str(), Message.length());
			}

			std::string SHA1Context::Hash(const std::string& Message, bool RawOutput)
			{
				return Hash(Message.c_str(), Message.length(), RawOutput);
			}

			const std::size_t SHA1Context::BlockSize() const
			{
				return 64;
			}

			const std::size_t SHA1Context::DigestSize() const
			{
				return 160;
			}
		}
	}
}