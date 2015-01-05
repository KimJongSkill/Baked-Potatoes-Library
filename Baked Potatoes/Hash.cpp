#include "Hash.hpp"
#include <cassert>
#include <vector>
#include <utility>
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
					Context.reset(Other.Context.release());;
				}

				return *this;
			}

			void SHA1Context::Update(const std::string& Message)
			{
				uint64_t MessageLength = Message.length();

				if (!MessageLength)
					return;

				Context->TotalLength += MessageLength * 8; // Total Length is in bits
				auto MessageWords = ToWords(Message);

				if (MessageLength == 64)
				{
					assert(MessageWords.second.second == 0);

					HashBlock(MessageWords.first);
				}
				else
				{
					if (Context->SmallBuffer.second == 0)
					{
						Context->BigBuffer.splice(Context->BigBuffer.end(), MessageWords.first);
						Context->SmallBuffer = std::move(MessageWords.second);
					}
					else
					{
						std::size_t Offset = 4 - Context->SmallBuffer.second;
						Context->BigBuffer.push_back(Context->SmallBuffer.first | MessageWords.first.front() >> Offset * 8);

						for (auto Iterator = std::next(MessageWords.first.begin()); Iterator != MessageWords.first.end(); ++Iterator)
						{
							Context->BigBuffer.push_back((*std::prev(Iterator) << Offset * 8) | (*Iterator >> Offset * 8));
						}
					}
					HashFromBuffer();
				}
			}

			std::array<uint8_t, 20> SHA1Context::Hash()
			{
				assert(Context->BigBuffer.size() < 16);

				if (Context->SmallBuffer.second != 0)
				{
					Context->SmallBuffer.first |= 0x80 << (3 - Context->SmallBuffer.second++);
					Context->BigBuffer.push_back(Context->SmallBuffer.first);
				}
				else
					Context->BigBuffer.emplace_back(0x80000000);

				if (Context->BigBuffer.size() > 14)
				{
					while (Context->BigBuffer.size() % 16 != 0)
					{
						Context->BigBuffer.emplace_back(0);
					}

					HashFromBuffer();
				}

				while (Context->BigBuffer.size() % 16 != 14)
					Context->BigBuffer.emplace_back(0);

				Context->BigBuffer.push_back(Context->TotalLength >> 32);
				Context->BigBuffer.push_back(Context->TotalLength & 0xffffffff);

				assert(Context->BigBuffer.size() == 16);

				HashBlock(Context->BigBuffer);

				std::array<uint8_t, 20> Digest;
				for (std::size_t i = 0; i < 20; ++i)
				{
					Digest[i] = Context->Result[i / 4] >> ((3 - (i % 4)) * 8) & 0xff;
				}

				Reset();

				return Digest;
			}

			std::array<uint8_t, 20> SHA1Context::Hash(const std::string& Message)
			{
				Update(Message);

				return Hash();
			}

			void SHA1Context::HashFromBuffer()
			{
				while (Context->BigBuffer.size() >= 16)
				{
					std::list<uint32_t> Block;
					
					if (Context->BigBuffer.size() == 16)
						Block.splice(Block.begin(), Context->BigBuffer);
					else
						Block.splice(Block.begin(), Context->BigBuffer, Context->BigBuffer.begin(), std::next(Context->BigBuffer.begin(), 16));

					HashBlock(Block);
				}
			}

			void SHA1Context::HashBlock(const std::list<uint32_t>& Block)
			{
				std::array<uint32_t, 80> Words;
				Words.fill(0);
				std::copy(Block.begin(), Block.end(), Words.begin());

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

			inline void SHA1Context::Reset()
			{
				Context.reset(new Data);
			}

			auto SHA1Context::ToWords(const std::string& Block) -> Buffer_t
			{
				BigBuffer_t Words;
				Words.push_back(0);
				SmallBuffer_t Residue;

				auto WordsIterator = Words.begin();
				Counter<std::size_t> Index(0, 3);
				Index.SetCallback([&]() { Words.push_back(0); ++WordsIterator; });

				for (auto BlockIterator = Block.begin(); BlockIterator != Block.end(); ++BlockIterator, ++Index)
				{
					*WordsIterator |= static_cast<uint8_t>(*BlockIterator) << ((3 - Index) * 8);
				}
			
				Residue.second = Block.length() % 4;
				if (Residue.second != 0)
					Residue.first = Words.back();
				
				Words.pop_back();

				return std::make_pair(Words, Residue);
			}
		}
	}
}