#pragma once

#include <cstdint>
#include <limits>

namespace bpl
{
	namespace image
	{
		struct Pixel32;

		struct Pixel24
		{
			using component_type = std::uint8_t;
			static const std::size_t Depth = 24;

			component_type Red;
			component_type Green;
			component_type Blue;

			Pixel24() = default;
			Pixel24(component_type r, component_type g, component_type b) : Red(r), Green(g), Blue(b) {};
			~Pixel24() = default;

			explicit operator Pixel32() const;
			bool operator==(const Pixel24& Other) const;
			bool operator!=(const Pixel24& Other) const;
		};

		struct Pixel32 : public Pixel24
		{
			using component_type = Pixel24::component_type;
			static const std::size_t Depth = 32;

			component_type Alpha;

			Pixel32() = default;
			Pixel32(component_type r, component_type g, component_type b, component_type a) : Pixel24(r, g, b), Alpha(a) {};
			~Pixel32() = default;

			explicit operator Pixel24() const;
			bool operator==(const Pixel32& Other) const;
			bool operator!=(const Pixel32& Other) const;
		};
	}
}