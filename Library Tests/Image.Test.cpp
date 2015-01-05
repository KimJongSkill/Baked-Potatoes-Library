#include "targetver.h"
#include "CppUnitTest.h"
#include "Pixel.hpp"
//
//using namespace Microsoft::VisualStudio::CppUnitTestFramework;
//
//namespace LibraryTests
//{
//	TEST_CLASS(PixelTest)
//	{
//		TEST_METHOD(Behaviour)
//		{
//			bpl::image::Pixel32 Pixel1;
//
//			Assert::IsFalse(Pixel1.R());
//			Assert::IsFalse(Pixel1.G());
//			Assert::IsFalse(Pixel1.B());
//			Assert::IsFalse(Pixel1.A());
//
//			Pixel1.B() = 42;
//			Assert::AreEqual(static_cast<bpl::image::Pixel32::component_type>(42), Pixel1.B());
//
//			bpl::image::Pixel32 Pixel2(69, 0, 0, 256);
//			Pixel2 = Pixel1;
//			Assert::AreEqual(static_cast<bpl::image::Pixel32::component_type>(0), Pixel2.R());
//			Assert::AreEqual(static_cast<bpl::image::Pixel32::component_type>(42), Pixel2.B());
//		}
//	};
//}