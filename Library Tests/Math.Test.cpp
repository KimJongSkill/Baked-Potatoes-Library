#include "targetver.h"
#include "CppUnitTest.h"
#include "Math.hpp"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace LibraryTests
{
	TEST_CLASS(MathTest)
	{
		TEST_METHOD(HarmonicNumbers)
		{
			Assert::AreEqual(static_cast<bpl::math::fmath_t>(0.00000), bpl::math::Harmonic<0>::value, 0.0001);
			Assert::AreEqual(static_cast<bpl::math::fmath_t>(1.00000), bpl::math::Harmonic<1>::value, 0.0001);
			Assert::AreEqual(static_cast<bpl::math::fmath_t>(1.50000), bpl::math::Harmonic<2>::value, 0.0001);
			Assert::AreEqual(static_cast<bpl::math::fmath_t>(1.83333), bpl::math::Harmonic<3>::value, 0.0001);
			Assert::AreEqual(static_cast<bpl::math::fmath_t>(2.08333), bpl::math::Harmonic<4>::value, 0.0001);
			Assert::AreEqual(static_cast<bpl::math::fmath_t>(2.28333), bpl::math::Harmonic<5>::value, 0.0001);
			Assert::AreEqual(static_cast<bpl::math::fmath_t>(2.44999), bpl::math::Harmonic<6>::value, 0.0001);
			Assert::AreEqual(static_cast<bpl::math::fmath_t>(2.59285), bpl::math::Harmonic<7>::value, 0.0001);
			Assert::AreEqual(static_cast<bpl::math::fmath_t>(2.71785), bpl::math::Harmonic<8>::value, 0.0001);
			Assert::AreEqual(static_cast<bpl::math::fmath_t>(2.82896), bpl::math::Harmonic<9>::value, 0.0001);
			Assert::AreEqual(static_cast<bpl::math::fmath_t>(2.92896), bpl::math::Harmonic<10>::value, 0.0001);
		}
	};
}