#pragma once
#include <cstdint>
#include <tuple>

namespace bpl
{
	namespace math
	{
		using math_t = int64_t;
		using umath_t = uint64_t;
		using fmath_t = double;

		bool IsPrime(math_t Number);

		bool IsPalindrome(math_t Number);

		bool IsPythagoreanTriple(math_t a, math_t b, math_t c);

		auto FindPythagoreanTriple(math_t Sum) -> std::tuple<math_t, math_t, math_t>;

		math_t LargestPrimeFactor(math_t Number);

		template <umath_t m, umath_t n>
		struct Ackermann
		{
			static const umath_t value = Ackermann<m - 1, Ackermann<m, n - 1>::value>::value;
		};

		template <umath_t n>
		struct Ackermann < 0, n >
		{
			static const umath_t value = n + 1;
		};

		template <umath_t m>
		struct Ackermann < m, 0 >
		{
			static const umath_t value = Ackermann<m - 1, 1>::value;
		};

		template <umath_t n>
		struct Factorial
		{
			static const umath_t value = n * Factorial<n - 1>::value;
		};

		template <>
		struct Factorial < 0 >
		{
			static const umath_t value = 1;
		};

		template <umath_t n>
		struct Harmonic
		{
			fmath_t static const value;
		};

		template <>
		struct Harmonic<0>
		{
			fmath_t static const value;
		};

		template <>
		struct Harmonic<1>
		{
			fmath_t static const value;
		};
		
		template <umath_t n>
		fmath_t const Harmonic<n>::value = Harmonic<n - 1>::value + 1.0 / n;
	}
}