#pragma once

#include "Utility.hpp"

namespace bpl
{
	namespace analog_literals
	{
		using namespace bpl::utility;
		using literal_t = unsigned int;
		enum LineSymbols { I, o };
		enum DiagonalSymbol{ L };

		namespace symbols
		{
			using analog_literals::I;
			using analog_literals::o;
			using analog_literals::L;
		}

		template <literal_t x> struct Line;
		template <literal_t x, literal_t y> struct Rectangle;
		template <literal_t x, literal_t y, literal_t z> struct Cuboid;

		template <literal_t x>
		struct Line
		{
			literal_t static const Length = x;

			operator literal_t() const { return Length; }
		};

		template <literal_t x, literal_t y>
		struct Rectangle
		{
			literal_t static const Width = x;
			literal_t static const Height = y / 2;
			literal_t static const Area = Width * Height;
		};

		template <literal_t x, literal_t y, literal_t z>
		struct Cuboid
		{
			literal_t static const Width = x / 2;
			literal_t static const Height = y / 3;
			literal_t static const Depth = z / 2;
			literal_t static const Area = 2 * Width * Height + 2 * Width * Depth + 2 * Height * Depth;
			literal_t static const Volume = Width * Height * Depth;
		};

		template <literal_t x, literal_t y>
		struct Dashes
		{
			literal_t static const Length = x;
			literal_t static const Height = y;

			Dashes<Length + 1, Height> operator--(int) const { return Any(); }
		};

		Dashes<1, 0> operator--(LineSymbols, int) { return Any(); }

		Line<0> operator-(LineSymbols, LineSymbols) { return Any(); }

		template <literal_t x>
		Line<x> operator-(Dashes<x, 0> Dashes, LineSymbols) { return Any(); }

		template <literal_t x, literal_t y>
		Rectangle<x, y + 1> operator|(Line<x>, Rectangle<x, y>) { return Any(); }

		template <literal_t x, literal_t y>
		Rectangle<x, y> operator-(Dashes<x, y> Dashes, LineSymbols) { return Any(); }

		template <literal_t x, literal_t y>
		Dashes<x, y + 1> operator!(Dashes<x, y>) { return Any(); }

		template <literal_t x, literal_t y>
		Cuboid<x, y, 1> operator|(DiagonalSymbol, Rectangle<x, y>) { return Any(); }

		Cuboid<0, 1, 1> operator!(DiagonalSymbol) { return Any(); }

		Cuboid <0, 1, 0> operator!(LineSymbols) { return Any(); }

		template <literal_t y1, literal_t z1, literal_t y2, literal_t z2>
		Cuboid<0, y1 + y2, z1 + z2> operator|(Cuboid<0, y1, z1>, Cuboid<0, y2, z2>) { return Any(); }

		template <literal_t y>
		Cuboid<0, y + 1, 0> operator!(Cuboid<0, y, 0>) { return Any(); }

		template <literal_t x1, literal_t x2, literal_t y2, literal_t z2>
		Cuboid<x1 + x2, y2, z2> operator|(Line<x1>, Cuboid<x2, y2, z2>) { return Any(); }

		template <literal_t x>
		Cuboid<x, 1, 1> operator|(Line<x>, DiagonalSymbol) { return Any(); }

		template <literal_t x, literal_t y, literal_t z>
		Cuboid<x, y + 1, z + 1> operator|(Cuboid<x, y, z>, DiagonalSymbol) { return Any(); }

		template <literal_t x, literal_t y, literal_t z>
		Cuboid<x, y + 1, z> operator|(Cuboid<x, y, z>, Line<x>) { return Any(); }

		template <literal_t x1, literal_t y1, literal_t z1, literal_t x2, literal_t y2, literal_t z2>
		Cuboid<x1 + x2, y1 + y2 + 1, z1 + z2> operator|(Cuboid<x1, y1, z1>, Cuboid<x2, y2, z2>) { return Any(); }

		template <literal_t x1, literal_t y1, literal_t z1, literal_t x2, literal_t y2>
		Cuboid<x1 + x2, y1 + y2 + 1, z1> operator|(Cuboid<x1, y1, z1>, Rectangle<x2, y2>) { return Any(); }

		template <literal_t x, literal_t y, literal_t z>
		Rectangle<x / 2, z> Top(Cuboid<x, y, z>) { return Any(); }

		template <literal_t x, literal_t y, literal_t z>
		Rectangle<x / 2, y / 3 * 2> Front(Cuboid<x, y, z>) { return Any(); }
		
		template <literal_t x, literal_t y, literal_t z>
		Rectangle<z / 2, y / 3 * 2> Side(Cuboid<x, y, z>) { return Any(); }
	}
}