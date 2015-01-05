//#include "Pixel.hpp"
//
//#include <limits>
//
//namespace bpl
//{
//	namespace image
//	{
//		Pixel24::Pixel24() : Pixel24(0, 0, 0) {};
//
//		Pixel24::Pixel24(component_type R, component_type G, component_type B) : Red(R), Green(G), Blue(B) {};
//
//		Pixel32::Pixel32() : Pixel32(0, 0, 0, std::numeric_limits<component_type>().max()) {};
//
//		Pixel32::Pixel32(component_type R, component_type G, component_type B, component_type A) : Pixel24(R, G, B), Alpha(A) {};
//
//		Pixel24::operator Pixel32() const
//		{
//			return Pixel32(Red, Green, Blue, std::numeric_limits<component_type>().max());
//		}
//
//		Pixel32::operator Pixel24()
//		{
//			return Pixel24(Red, Green, Blue);
//		}
//
//		auto Pixel24::R() -> component_type&
//		{
//			return Red;
//		}
//
//		auto Pixel24::R() const -> component_type const
//		{
//			return Red;
//		}
//
//		auto Pixel24::G() -> component_type&
//		{
//			return Green;
//		}
//
//		auto Pixel24::G() const -> component_type const
//		{
//			return Green;
//		}
//
//		auto Pixel24::B() -> component_type&
//		{
//			return Blue;
//		}
//
//		auto Pixel24::B() const -> component_type const
//		{
//			return Blue;
//		}
//
//		auto Pixel32::A() -> component_type&
//		{
//			return Alpha;
//		}
//
//		auto Pixel32::A() const -> component_type const
//		{
//			return Alpha;
//		}
//	}
//}