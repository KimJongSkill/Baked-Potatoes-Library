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
	}
}