#pragma once
#include <cstddef>
#include <cstdint>
#include <string>
#include <sstream>
#include <iomanip>
#include <array>
#include <type_traits>
#include <cstdlib>

namespace bpl
{
	namespace utility
	{
		struct Any
		{
			template <class Type>
			operator Type() const
			{
				return Type();
			}
		};

		// Returns true if little, false if big-endian
		bool Endianness();

		template <class T>
		inline T LeftRotate(T Value, std::size_t Shift)
		{
			return (Value << Shift) | (Value >> (sizeof(Value) * 8 - Shift));
		}

		template <class T>
		inline T RightRotate(T Value, std::size_t Shift)
		{
			return (Value >> Shift) | (Value << (sizeof(Value) * 8 - Shift));
		}

		std::string ToHex(const std::string& Array);

		uint64_t BigEndian64(uint64_t Value);

		uint32_t BigEndian32(uint32_t Value);

		uint16_t BigEndian16(uint16_t Value);

		uint64_t LittleEndian64(uint64_t Value);

		uint32_t LittleEndian32(uint32_t Value);

		uint16_t LittleEndian16(uint16_t Value);

		struct Uncopyable
		{
		protected:
			Uncopyable() = default;
			~Uncopyable() = default;

			Uncopyable(Uncopyable&&);
			const Uncopyable& operator=(Uncopyable&&);

			Uncopyable(const Uncopyable&) = delete;
			const Uncopyable& operator=(const Uncopyable&) = delete;
		};
	}
}