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
			if (Endianness())
				return _byteswap_uint64(Value);
		}

		uint32_t BigEndian32(uint32_t Value)
		{
			if (Endianness())
				return _byteswap_ulong(Value);
		}

		uint16_t BigEndian16(uint16_t Value)
		{
			if (Endianness())
				return _byteswap_ushort(Value);
		}

		uint64_t LittleEndian64(uint64_t Value)
		{
			if (!Endianness())
				return _byteswap_uint64(Value);
		}

		uint32_t LittleEndian32(uint32_t Value)
		{
			if (!Endianness())
				return _byteswap_ulong(Value);
		}

		uint16_t LittleEndian16(uint16_t Value)
		{
			if (!Endianness())
				return _byteswap_ushort(Value);
		}
	}
}