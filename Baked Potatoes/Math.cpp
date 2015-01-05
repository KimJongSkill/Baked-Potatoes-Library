#include "Math.hpp"
#include <cmath>
#include <iostream>

namespace bpl
{
	namespace math
	{
		fmath_t const Harmonic<0>::value = 0;
		fmath_t const Harmonic<1>::value = 1;

		bool IsPrime(math_t Number)
		{
			if (Number == 1)
				return false;
			else if (Number < 4) // 2, 3
				return true;
			else if (Number % 2 == 0) // Even
				return false;
			else if (Number % 3 == 0)
				return false;
			else if (Number < 9) // 5, 7
				return true;
			else
			{
				for (int i = 5; i <= sqrt(Number); i += 6) // All Primes > 3 can be represented as: 6k +- 1
				{
					if (((Number % i) == 0) || ((Number % (i + 2)) == 0))
						return false;
				}
			}

			return true;
		}

		bool IsPalindrome(math_t Number)
		{
			if (Number < 0)
				return false;
			if (Number < 10)
				return true;

			int Divisor = 1;

			while (Number / Divisor >= 10)
			{
				Divisor *= 10;
			}

			while (Number != 0)
			{
				int First = static_cast<int>(Number / Divisor);
				int Last = Number % 10;

				if (First != Last)
					return false;

				Number %= Divisor;
				Number /= 10;
				Divisor /= 100;
			}

			return true;
		}

		bool IsPythagoreanTriple(math_t a, math_t b, math_t c)
		{
			return (a * a) + (b * b) == (c * c);
		}

		auto FindPythagoreanTriple(math_t Sum) -> std::tuple < math_t, math_t, math_t >
		{
			// (2mn) + (m^2 - n^2) = (m^2 + n^2)
			math_t m = 0;

			for (math_t n = 0; n < Sum; ++n)
			{
				math_t Delta = n * n + 2 * Sum;
				double m1 = (-n + sqrt(Delta)) / 2;
				double m2 = (-n - sqrt(Delta)) / 2;

				if (fmod(m1, 1.0) == 0 && m1 > n)
				{
					m = static_cast<math_t>(m1);

					return std::make_tuple(2 * m * n, m * m - n * n, m * m + n * n);
				}
				else if (fmod(m2, 1.0) == 0 && m2 > n)
				{
					m = static_cast<math_t>(m2);

					return std::make_tuple(2 * m * n, m * m - n * n, m * m + n * n);
				}
			}

			return std::make_tuple(0, 0, 0);
		}

		math_t LargestPrimeFactor(math_t Number)
		{
			math_t Factor, LastFactor;
			double MaxFactor;

			if (Number % 2 == 0)
			{
				LastFactor = 2;

				do
				{
					Number /= 2;
				} while (Number % 2 == 0);
			}
			else
				LastFactor = 1;

			Factor = 3;
			MaxFactor = sqrt(Number);

			while (Number > 1 && Factor <= MaxFactor)
			{
				if (Number % Factor == 0)
				{
					do
					{
						Number /= Factor;
					} while (Number % Factor == 0);

					LastFactor = Factor;
					MaxFactor = sqrt(Number);
				}

				Factor += 2;
			}

			if (Number == 1)
				return LastFactor;
			else
				return Number;
		}
	}
}