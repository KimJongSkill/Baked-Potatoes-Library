#pragma once
#include <cstddef>
#include <cstdint>
#include <string>
#include <sstream>
#include <iomanip>
#include <array>
#include <type_traits>

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

		template <class Type, std::size_t Length>
		std::string ByteToByteString(const std::array<Type, Length>& Input)
		{
			static_assert(std::is_integral<Type>::value, "Type must be integral");

			std::array<uint8_t, Length * sizeof(Type)> Bytes;

			for (std::size_t i = 0; i < Length; ++i)
			{
				for (std::size_t j = 0; j < sizeof(Type); ++j)
					Bytes[i * sizeof(Type) + j] = static_cast<uint8_t>(static_cast<std::make_unsigned<Type>::type>(Input[i]) >> (sizeof(Type) - 1 - j) * 8);
			}

			std::stringstream Stream;

			for (uint8_t i : Bytes)
				Stream << std::setfill('0') << std::setw(2) << std::hex << i;

			return Stream.str();
		}

		template <std::size_t Length>
		std::string ByteToByteString(const std::array<uint8_t, Length>& Input)
		{
			std::stringstream Stream;

			for (uint8_t i : Input)
				Stream << std::setfill('0') << std::setw(1) << std::hex << i;

			std::string Output = Stream.str();
			return Output;
		}

		template <class Type, std::size_t Length>
		std::string ByteToHexString(const std::array<Type, Length>& Input)
		{
			static_assert(std::is_integral<Type>::value, "Type must be integral");

			std::stringstream Stream;
			Stream << std::setfill('0') << std::setw(2 * sizeof(Type));

			for (Type i : Input)
				Stream << std::hex << std::setfill('0') << std::setw(2 * sizeof(Type)) << i;

			return Stream.str();
		}

		template <std::size_t Length>
		std::string ByteToHexString(const std::array<uint8_t, Length>& Input)
		{
			std::stringstream Stream;

			for (auto i : Input)
				Stream << std::setfill('0') << std::setw(2) << std::hex << static_cast<unsigned>(i);

			return Stream.str();
		}
	}
}