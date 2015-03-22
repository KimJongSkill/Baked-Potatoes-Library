#include "Pixel.hpp"

namespace bpl
{
	namespace image
	{
		Pixel24::operator Pixel32() const
		{
			return Pixel32(Red, Green, Blue, std::numeric_limits<Pixel32::component_type>::max());
		}

		bool Pixel24::operator==(const Pixel24& Other) const
		{
			return Red == Other.Red && Green == Other.Green && Blue == Other.Blue;
		}

		bool Pixel24::operator!=(const Pixel24& Other) const
		{
			return !this->operator==(Other);
		}

		Pixel32::operator Pixel24() const
		{
			return Pixel24(Red, Green, Blue);
		}

		bool Pixel32::operator==(const Pixel32& Other) const
		{
			return Pixel24::operator==(Other) && Alpha == Other.Alpha;
		}

		bool Pixel32::operator!=(const Pixel32& Other) const
		{
			return !this->operator==(Other);
		}
	}
}