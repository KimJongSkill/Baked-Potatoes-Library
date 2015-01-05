#pragma once

#include <cstdint>
#include <exception>
#include <string>
#include <functional>
#include <vector>

namespace bpl
{
	namespace crypt
	{
		class UnsupportedInstruction : public std::exception
		{
			std::string Data;

		public:
			UnsupportedInstruction(const char* MissingInstruction);
			UnsupportedInstruction(const UnsupportedInstruction&) = default;
			UnsupportedInstruction& operator= (const UnsupportedInstruction&) = default;
			virtual ~UnsupportedInstruction() = default;

			virtual const char* what() const override;
			virtual const char* Instruction() const;
		};

		class IvyRNG
		{
		public:
			static bool SupportsRDRAND();

			IvyRNG();

			bool operator()(uint16_t&);
			bool operator()(uint32_t&);
			bool operator()(uint64_t&);
			template <class Iterator>
			bool operator()(Iterator Begin, Iterator End)
			{
				std::ptrdiff_t Distance = std::distance(Begin, End) * sizeof(*Begin);
				std::vector<uint64_t> Buffer;
				Buffer.resize((Distance + 7) / 8);

				for (uint64_t& i : Buffer)
				{
					if (!_rdrand64_step(&i))
						return false;
				}

				std::memcpy(&*Begin, &Buffer.front(), Distance);
				return true;
			}

		private:
			template <class Type>
			bool Generate(Type&, std::function<int(Type*)>);

			enum class State { Unsupported, Supported, Undefined };
		};

		bool ConstantTimeEquals(void* Bytes1, void* Bytes2, std::size_t Length);
	}
}