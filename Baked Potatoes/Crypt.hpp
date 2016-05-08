#pragma once

#include <cstdint>
#include <exception>
#include <string>
#include <functional>
#include <vector>
#include <array>
#include "MAC.hpp"

namespace bpl
{
	namespace crypt
	{
		class UnsupportedInstruction : public std::exception
		{
			std::string Data;

		public:
			UnsupportedInstruction(const char* MissingInstruction);
			UnsupportedInstruction(const UnsupportedInstruction&) = default;
			UnsupportedInstruction& operator= (const UnsupportedInstruction&) = default;
			virtual ~UnsupportedInstruction() = default;

			virtual const char* what() const override;
			virtual const char* Instruction() const;
		};

		class IvyRNG
		{
		public:
			static bool SupportsRDRAND();

			IvyRNG();

			bool operator()(uint16_t&);
			bool operator()(uint32_t&);
			bool operator()(uint64_t&);
			template <class Iterator>
			bool operator()(Iterator Begin, Iterator End)
			{
				std::ptrdiff_t Distance = std::distance(Begin, End) * sizeof(*Begin);
#ifdef ENVIROMENT64
				std::vector<uint64_t> Buffer;
				Buffer.resize((Distance + 7) / sizeof(uint64_t));

				for (uint64_t& i : Buffer)
				{
					if (!_rdrand64_step(&i))
						return false;
				}
#else
				std::vector<uint32_t> Buffer;
				Buffer.resize((Distance + 7) / sizeof(uint32_t));

				for (uint32_t& i : Buffer)
				{
					if (!_rdrand32_step(&i))
						return false;
				}
#endif

				std::memcpy(&*Begin, &Buffer.front(), Distance);
				return true;
			}

		private:
			template <class Type>
			bool Generate(Type&, std::function<int(Type*)>);

			enum class State { Unsupported, Supported, Undefined };
		};

		bool ConstantTimeEquals(void* Bytes1, void* Bytes2, std::size_t Length);

		template <class Hash>
		std::string PBKDF2(std::string Password, std::string Salt, std::size_t Cost, std::size_t Length, bool RawOutput = false)
		{
			bpl::crypt::MAC::HMAC<Hash> Function;
			std::string DerivedKey;

			if (Length > (std::pow(2, 32) - 1) * Function.DigestSize())
				throw std::out_of_range("Derived key too long");

			for (std::uint32_t i = 1; DerivedKey.length() < Length; ++i)
			{
				union { std::uint32_t Int; std::uint8_t Array[4]; } BigEndian;
				BigEndian.Int = bpl::utility::BigEndian32(i);

				std::string Buffer = Salt;
				for (std::size_t j = 0; j < sizeof(BigEndian); ++j)
					Buffer.push_back(BigEndian.Array[j]);
				std::string CurrentKey = Function(Password, Buffer, true);
				Buffer = CurrentKey;

				for (std::size_t j = 1; j < Cost; ++j)
				{
					Buffer = Function(Password, Buffer, true);
					std::transform(CurrentKey.begin(), CurrentKey.end(), Buffer.begin(), CurrentKey.begin(), [](std::uint8_t x, std::uint8_t y) { return x ^ y; });
				}

				DerivedKey += CurrentKey;
			}

			DerivedKey.resize(Length);
			return RawOutput ? DerivedKey : bpl::utility::ToHex(DerivedKey);
		}

		/*
		*	AES Class
		*	KeySize is in bits. Valid values are 128, 192 and 256.
		*/
		template <std::size_t KeySize>
		class AES
		{
			static const std::size_t KeyLength = KeySize / 32;
			static const std::size_t BlockSize = 4;
			static const std::size_t Rounds = KeyLength + 6;

			static const uint8_t SBox[256];
			static const uint8_t SBoxInverse[256];
			static const uint8_t Rcon[11];
			// http://csrc.nist.gov/publications/fips/fips197/fips-197.pdf pg23 5.2 Key Expansion
		
		private:
			std::array<std::uint8_t, 16> State;
			std::array<std::uint32_t, BlockSize * (Rounds + 1)> ExpandedKey;
			
			std::uint8_t xtime(std::size_t Index) const;

			void AddRoundKey(std::size_t Round);
			/*void InvMixColumns();
			void InvShiftRows();	After we get encryption to compile
			void InvSubBytes();*/
			void MixColumns();
			std::uint32_t RotWord(std::uint32_t Word) const;
			void ShiftRows();
			void SubBytes();
			std::uint32_t SubWord(std::uint32_t Word) const;

			void KeyExpansion(const std::array<uint8_t, KeySize / 8>& Key);

		public:
			std::array<uint8_t, 16> EncryptBlock(const std::array<uint8_t, 16>& Plaintext, const std::array<uint8_t, KeySize / 8>& Key);
		};
	}
}