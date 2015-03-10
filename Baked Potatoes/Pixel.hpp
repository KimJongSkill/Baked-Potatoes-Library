#pragma once

#include <cstdint>

namespace bpl
{
	namespace image
	{
		struct Pixel24
		{
			using component_type = std::uint8_t;
			static const std::size_t Depth = 24;

			component_type Red;
			component_type Green;
			component_type Blue;
		};

		struct Pixel32 : public Pixel24
		{
			using component_type = Pixel24::component_type;
			static const std::size_t Depth = 32;

			component_type Alpha;
		};
	}
}