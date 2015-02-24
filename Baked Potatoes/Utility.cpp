#include "Utility.hpp"
#include <cstdint>

namespace bpl
{
	namespace utility
	{
		bool Endianness()
		{
			return !((*reinterpret_cast<uint16_t*>("#S") >> 8) == '#');
		}

		uint64_t BigEndian64(uint64_t Value)
		{
			return Endianness() ? _byteswap_uint64(Value) : Value;
		}

		uint32_t BigEndian32(uint32_t Value)
		{
			return Endianness() ? _byteswap_ulong(Value) : Value;
		}

		uint16_t BigEndian16(uint16_t Value)
		{
			return Endianness() ? _byteswap_ushort(Value) : Value;
		}

		uint64_t LittleEndian64(uint64_t Value)
		{
			return !Endianness() ? _byteswap_uint64(Value) : Value;
		}

		uint32_t LittleEndian32(uint32_t Value)
		{
			return !Endianness() ? _byteswap_ulong(Value) : Value;
		}

		uint16_t LittleEndian16(uint16_t Value)
		{
			return !Endianness() ? _byteswap_ushort(Value) : Value;
		}

		std::string ToHex(const std::string& Array)
		{
			const std::string LookUpTable = "0123456789abcdef";
			std::string Result;
			Result.reserve(Array.length() * 2);

			for (auto Character : Array)
			{
				auto Byte = static_cast<std::uint8_t>(Character);
				Result.push_back(LookUpTable[Byte >> 4]);
				Result.push_back(LookUpTable[Byte & 0xf]);
			}

			return Result;
		}
	}
}