//#pragma once
//
//#include <cstdint>
//
//namespace bpl
//{
//	namespace image
//	{
//		class Pixel24
//		{
//		public:
//			using component_type = std::uint8_t;
//
//			Pixel24();
//
//			Pixel24(component_type R, component_type G, component_type B);
//
//			operator Pixel32() const;
//
//			component_type& R();
//
//			const component_type R() const;
//
//			component_type& G();
//
//			component_type const G() const;
//
//			component_type& B();
//
//			const component_type B() const;
//
//		protected:
//			component_type Red;
//			component_type Green;
//			component_type Blue;
//		};
//
//		class Pixel32 : public Pixel24
//		{
//		public:
//			using component_type = Pixel24::component_type;
//
//			Pixel32();
//
//			Pixel32(component_type R, component_type G, component_type B, component_type A);
//
//			operator Pixel24();
//
//			component_type& A();
//
//			component_type const A() const;
//
//		private:
//			component_type Alpha;
//		};
//	}
//}