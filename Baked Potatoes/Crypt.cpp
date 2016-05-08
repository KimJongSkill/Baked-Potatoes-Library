#include "Crypt.hpp"

#include <cstring>
#include <algorithm>
#include "Utility.hpp"

// Required wrappers so we can take their address
int RDRAND16(uint16_t* Value)
{
	return _rdrand16_step(Value);
}

int RDRAND32(uint32_t* Value)
{
	return _rdrand32_step(Value);
}

#ifdef ENVIROMENT64
int RDRAND64(uint64_t* Value)
{
	return _rdrand64_step(Value);
}
#endif

namespace bpl
{
	namespace crypt
	{

		UnsupportedInstruction::UnsupportedInstruction(const char* MissingInstruction) : Data(MissingInstruction) {}

		const char* UnsupportedInstruction::what() const
		{
			return "Required Instruction is unsupported by the Processor";
		}

		const char* UnsupportedInstruction::Instruction() const
		{
			return Data.c_str();
		}

		IvyRNG::IvyRNG()
		{
			if (!SupportsRDRAND())
				throw UnsupportedInstruction("RDRAND");
		}

		bool IvyRNG::operator()(uint16_t& Argument)
		{
			return Generate<uint16_t>(Argument, &RDRAND16);
		}

		bool IvyRNG::operator()(uint32_t& Argument)
		{
			return Generate<uint32_t>(Argument, &RDRAND32);
		}

		bool IvyRNG::operator()(uint64_t& Argument)
		{
#ifdef ENVIROMENT64
			return Generate<uint64_t>(Argument, &RDRAND64);
#else
			bool Result = 1;
			uint32_t Value1, Value2;

			Result &= Generate<unsigned int>(Value1, &RDRAND32);
			Result &= Generate<unsigned int>(Value2, &RDRAND32);
			Argument = Value1 | static_cast<uint64_t>(Value2) << 32;

			return Result;
#endif
		}

		bool IvyRNG::SupportsRDRAND()
		{
			static State Result = State::Undefined;

			if (Result == State::Supported)
				return true;
			else if (Result == State::Unsupported)
				return false;
			else
			{
				// EAX, EBX, ECX, EDX Registers
				int32_t Info[4] = { 0 };
				// EAX = 1 for feature bits
				__cpuid(Info, 0x1);

				// RDRAND Support resides on the 30th bit in ECX
				bool Support = (Info[2] & (1 << 30)) != 0;

				if (Support)
				{
					Result = State::Supported;
					return true;
				}
				else
				{
					Result = State::Unsupported;
					return false;
				}
			}
		}

		template <class Type>
		bool IvyRNG::Generate(Type& Argument, std::function<int(Type*)> RDRAND)
		{
			/*
			*	In case RDRAND fails to return a random number,
			*	it is recommended to retry 10 times.
			*/
			for (std::size_t i = 0; i < 10; ++i)
			{
				int Result = RDRAND(&Argument);
				
				if (Result != 0)
					return true;
			}

			return false;
		}

		bool ConstantTimeEquals(void* Bytes1, void* Bytes2, std::size_t Length)
		{
			unsigned int Result = 0;

			for (std::size_t i = 0; i < Length; ++i)
				Result |= reinterpret_cast<char*>(Bytes1)[i] ^ reinterpret_cast<char*>(Bytes2)[i];

			return Result == 0;
		}

		template <std::size_t KeySize>
		inline std::uint8_t AES<KeySize>::xtime(std::uint8_t Value) const
		{
			Value <<= 1;
			return Value & 0x80 ? Value ^ 0x1b : Value;
		}

		template <std::size_t KeySize>
		void AES<KeySize>::AddRoundKey(std::size_t Round)
		{
			auto StatePointer = reinterpret_cast<std::uint32_t*>(State.data());
			for (std::size_t i = 0; i < 4; ++i)
				StatePointer[i] ^= _byteswap_ulong(ExpandedKey[BlockSize * Round + i]);
		}

		template <std::size_t KeySize>
		void AES<KeySize>::MixColumns()
		{
			/*
			*	This is sort of like a matrix multiplication, but with XORs.
			*	- when we have a 0x1 in the matrix, we simply XOR the byte
			*	- when we have a 0x2 we perform the xtime operation and then XOR
			*	- when we have a 0x3 we do both, since 0x1 & 0x2 = 0x3
			*/

			std::array<std::uint8_t, 4> NewColumn;

			for (std::size_t i = 0; i < 16; i += 4)
			{
				//NewColumn[0] = xtime(State[i]) ^ (xtime(State[i + 1]) ^ State[i + 1]) ^ State[i + 2] ^ State[i + 3]; // Works
				//NewColumn[1] = State[i] ^ xtime(State[i + 1]) ^ (xtime(State[i + 2]) ^ State[i + 2]) ^ State[i + 3]; // Doesn't work
				//NewColumn[2] = State[i] ^ State[i + 1] ^ xtime(State[i + 2]) ^ (xtime(State[i + 3]) ^ State[i + 3]); // Also doesn't work
				//NewColumn[3] = (xtime(State[i]) ^ State[i]) ^ State[i + 1] ^ State[i + 2] ^ xtime(State[i + 3]); // But this one works?
				
				unsigned char a[4];
				unsigned char b[4];
				unsigned char c;
				unsigned char h;
				/* 
				*	The array 'a' is simply a copy of the input array 'r'.
				*	The array 'b' is each element of the array 'a' multiplied by 2 in Rijndael's Galois field.
				*	a[n] ^ b[n] is element n multiplied by 3 in Rijndael's Galois field.
				*/
				for (c = 0; c < 4; c++) {
					a[c] = State[i + c];
					// h is 0xff if the high bit of r[c] is set, 0 otherwise
					h = (unsigned char)((signed char)a[c] >> 7); // arithmetic right shift, thus shifting in either zeros or ones
					b[c] = a[c] << 1; // implicitly removes high bit because b[c] is an 8-bit char, so we xor by 0x1b and not 0x11b in the next line
					b[c] ^= 0x1B & h; // Rijndael's Galois field
				}
				NewColumn[0] = b[0] ^ a[3] ^ a[2] ^ b[1] ^ a[1]; // 2 * a0 + a3 + a2 + 3 * a1
				NewColumn[1] = b[1] ^ a[0] ^ a[3] ^ b[2] ^ a[2]; // 2 * a1 + a0 + a3 + 3 * a2
				NewColumn[2] = b[2] ^ a[1] ^ a[0] ^ b[3] ^ a[3]; // 2 * a2 + a1 + a0 + 3 * a3
				NewColumn[3] = b[3] ^ a[2] ^ a[1] ^ b[0] ^ a[0]; // 2 * a3 + a2 + a1 + 3 * a0

				std::copy(NewColumn.cbegin(), NewColumn.cend(), State.begin() + i);
			} 
		}

		template <std::size_t KeySize>
		inline std::uint32_t AES<KeySize>::RotWord(std::uint32_t Word) const
		{
			return bpl::utility::LeftRotate(Word, 8);
		}

		template <std::size_t KeySize>
		void AES<KeySize>::ShiftRows()
		{
			// Row 1
			auto Temp = State[1];
			State[1] = State[5];
			State[5] = State[9];
			State[9] = State[13];
			State[13] = Temp;
			// Row 2
			std::swap(State[2], State[10]);
			std::swap(State[6], State[14]);
			// Row 3
			Temp = State[3];
			State[3] = State[15];
			State[15] = State[11];
			State[11] = State[7];
			State[7] = Temp;
		}

		template <std::size_t KeySize>
		void AES<KeySize>::SubBytes()
		{
			for (auto& Byte : State)
				Byte = SBox[Byte];
		}

		template <std::size_t KeySize>
		std::uint32_t AES<KeySize>::SubWord(std::uint32_t Word) const
		{
			auto Pointer = reinterpret_cast<uint8_t*>(&Word);

			for (std::size_t i = 0; i < 4; ++i)
			{
				//std::size_t Row = *Pointer >> 4;
				//std::size_t Column = *Pointer & 0x0f;

				//*Pointer++ = SBox[Row * 0xf + Column];
				*Pointer = SBox[*Pointer];
				++Pointer;
			}

			return Word;
		}

		template <std::size_t KeySize>
		void AES<KeySize>::KeyExpansion(const std::array<uint8_t, KeySize / 8>& Key)
		{
			//std::copy(Key.cbegin(), Key.cend(), reinterpret_cast<std::uint8_t*>(ExpandedKey.data()));
			for (std::size_t i = 0; i < Key.size(); i += 4)
			{
				ExpandedKey[i / 4] = (Key[i] << 24) ^ (Key[i + 1] << 16) ^ (Key[i + 2] << 8) ^ Key[i + 3];
			}

			std::uint32_t Temp;
			for (std::size_t i = KeyLength; i < BlockSize * (Rounds + 1); ++i)
			{
				Temp = ExpandedKey[i - 1];
				
				if (i % KeyLength == 0)
					Temp = SubWord(RotWord(Temp)) ^ (Rcon[i / KeyLength] << 24);
#pragma warning(suppress: 4127)
				else if (KeyLength == 8 && i % KeyLength == 4)
					Temp = SubWord(Temp);

				ExpandedKey[i] = ExpandedKey[i - KeyLength] ^ Temp;
			}
		}

		template <std::size_t KeySize>
		inline void AES<KeySize>::InitializeState(const std::array<uint8_t, 16>& Input)
		{
			/*for (std::size_t r = 0; r < 4; ++r)
				for (std::size_t c = 0; c < 4; ++c)
				State[r][c] = Input[r + 4 * c];*/
			std::copy(Input.cbegin(), Input.cend(), State.begin());
		}

		template <std::size_t KeySize>
		inline std::array<uint8_t, 16> AES<KeySize>::StateToOutput() const
		{
			std::array<uint8_t, 16> Output;
			/*for (std::size_t r = 0; r < 4; ++r)
				for (std::size_t c = 0; c < 4; ++c)
				Output[r + 4 * c] = State[r][c];*/
			std::copy(State.cbegin(), State.cend(), Output.begin());
			return Output;
		}

		template <std::size_t KeySize>
		std::array<uint8_t, 16> AES<KeySize>::EncryptBlock(const std::array<uint8_t, 16>& Plaintext, const std::array<uint8_t, KeySize / 8>& Key)
		{
			InitializeState(Plaintext);

			KeyExpansion(Key);

			AddRoundKey(0);

			for (std::size_t i = 1; i < Rounds; ++i)
			{
				SubBytes();
				ShiftRows();
				MixColumns();
				AddRoundKey(i);
			}

			SubBytes();
			ShiftRows();
			AddRoundKey(Rounds);

			return StateToOutput();
		}
		
		template <std::size_t KeySize>
		const std::uint8_t AES<KeySize>::SBox[256] =
		{
			0x63, 0x7C, 0x77, 0x7B, 0xF2, 0x6B, 0x6F, 0xC5, 0x30, 0x01, 0x67, 0x2B, 0xFE, 0xD7, 0xAB, 0x76,
			0xCA, 0x82, 0xC9, 0x7D, 0xFA, 0x59, 0x47, 0xF0, 0xAD, 0xD4, 0xA2, 0xAF, 0x9C, 0xA4, 0x72, 0xC0,
			0xB7, 0xFD, 0x93, 0x26, 0x36, 0x3F, 0xF7, 0xCC, 0x34, 0xA5, 0xE5, 0xF1, 0x71, 0xD8, 0x31, 0x15,
			0x04, 0xC7, 0x23, 0xC3, 0x18, 0x96, 0x05, 0x9A, 0x07, 0x12, 0x80, 0xE2, 0xEB, 0x27, 0xB2, 0x75,
			0x09, 0x83, 0x2C, 0x1A, 0x1B, 0x6E, 0x5A, 0xA0, 0x52, 0x3B, 0xD6, 0xB3, 0x29, 0xE3, 0x2F, 0x84,
			0x53, 0xD1, 0x00, 0xED, 0x20, 0xFC, 0xB1, 0x5B, 0x6A, 0xCB, 0xBE, 0x39, 0x4A, 0x4C, 0x58, 0xCF,
			0xD0, 0xEF, 0xAA, 0xFB, 0x43, 0x4D, 0x33, 0x85, 0x45, 0xF9, 0x02, 0x7F, 0x50, 0x3C, 0x9F, 0xA8,
			0x51, 0xA3, 0x40, 0x8F, 0x92, 0x9D, 0x38, 0xF5, 0xBC, 0xB6, 0xDA, 0x21, 0x10, 0xFF, 0xF3, 0xD2,
			0xCD, 0x0C, 0x13, 0xEC, 0x5F, 0x97, 0x44, 0x17, 0xC4, 0xA7, 0x7E, 0x3D, 0x64, 0x5D, 0x19, 0x73,
			0x60, 0x81, 0x4F, 0xDC, 0x22, 0x2A, 0x90, 0x88, 0x46, 0xEE, 0xB8, 0x14, 0xDE, 0x5E, 0x0B, 0xDB,
			0xE0, 0x32, 0x3A, 0x0A, 0x49, 0x06, 0x24, 0x5C, 0xC2, 0xD3, 0xAC, 0x62, 0x91, 0x95, 0xE4, 0x79,
			0xE7, 0xC8, 0x37, 0x6D, 0x8D, 0xD5, 0x4E, 0xA9, 0x6C, 0x56, 0xF4, 0xEA, 0x65, 0x7A, 0xAE, 0x08,
			0xBA, 0x78, 0x25, 0x2E, 0x1C, 0xA6, 0xB4, 0xC6, 0xE8, 0xDD, 0x74, 0x1F, 0x4B, 0xBD, 0x8B, 0x8A,
			0x70, 0x3E, 0xB5, 0x66, 0x48, 0x03, 0xF6, 0x0E, 0x61, 0x35, 0x57, 0xB9, 0x86, 0xC1, 0x1D, 0x9E,
			0xE1, 0xF8, 0x98, 0x11, 0x69, 0xD9, 0x8E, 0x94, 0x9B, 0x1E, 0x87, 0xE9, 0xCE, 0x55, 0x28, 0xDF,
			0x8C, 0xA1, 0x89, 0x0D, 0xBF, 0xE6, 0x42, 0x68, 0x41, 0x99, 0x2D, 0x0F, 0xB0, 0x54, 0xBB, 0x16
		};

		template <std::size_t KeySize>
		const std::uint8_t AES<KeySize>::SBoxInverse[256] = 
		{
			0x52, 0x09, 0x6A, 0xD5, 0x30, 0x36, 0xA5, 0x38, 0xBF, 0x40, 0xA3, 0x9E, 0x81, 0xF3, 0xD7, 0xFB,
			0x7C, 0xE3, 0x39, 0x82, 0x9B, 0x2F, 0xFF, 0x87, 0x34, 0x8E, 0x43, 0x44, 0xC4, 0xDE, 0xE9, 0xCB,
			0x54, 0x7B, 0x94, 0x32, 0xA6, 0xC2, 0x23, 0x3D, 0xEE, 0x4C, 0x95, 0x0B, 0x42, 0xFA, 0xC3, 0x4E,
			0x08, 0x2E, 0xA1, 0x66, 0x28, 0xD9, 0x24, 0xB2, 0x76, 0x5B, 0xA2, 0x49, 0x6D, 0x8B, 0xD1, 0x25,
			0x72, 0xF8, 0xF6, 0x64, 0x86, 0x68, 0x98, 0x16, 0xD4, 0xA4, 0x5C, 0xCC, 0x5D, 0x65, 0xB6, 0x92,
			0x6C, 0x70, 0x48, 0x50, 0xFD, 0xED, 0xB9, 0xDA, 0x5E, 0x15, 0x46, 0x57, 0xA7, 0x8D, 0x9D, 0x84,
			0x90, 0xD8, 0xAB, 0x00, 0x8C, 0xBC, 0xD3, 0x0A, 0xF7, 0xE4, 0x58, 0x05, 0xB8, 0xB3, 0x45, 0x06,
			0xD0, 0x2C, 0x1E, 0x8F, 0xCA, 0x3F, 0x0F, 0x02, 0xC1, 0xAF, 0xBD, 0x03, 0x01, 0x13, 0x8A, 0x6B,
			0x3A, 0x91, 0x11, 0x41, 0x4F, 0x67, 0xDC, 0xEA, 0x97, 0xF2, 0xCF, 0xCE, 0xF0, 0xB4, 0xE6, 0x73,
			0x96, 0xAC, 0x74, 0x22, 0xE7, 0xAD, 0x35, 0x85, 0xE2, 0xF9, 0x37, 0xE8, 0x1C, 0x75, 0xDF, 0x6E,
			0x47, 0xF1, 0x1A, 0x71, 0x1D, 0x29, 0xC5, 0x89, 0x6F, 0xB7, 0x62, 0x0E, 0xAA, 0x18, 0xBE, 0x1B,
			0xFC, 0x56, 0x3E, 0x4B, 0xC6, 0xD2, 0x79, 0x20, 0x9A, 0xDB, 0xC0, 0xFE, 0x78, 0xCD, 0x5A, 0xF4,
			0x1F, 0xDD, 0xA8, 0x33, 0x88, 0x07, 0xC7, 0x31, 0xB1, 0x12, 0x10, 0x59, 0x27, 0x80, 0xEC, 0x5F,
			0x60, 0x51, 0x7F, 0xA9, 0x19, 0xB5, 0x4A, 0x0D, 0x2D, 0xE5, 0x7A, 0x9F, 0x93, 0xC9, 0x9C, 0xEF,
			0xA0, 0xE0, 0x3B, 0x4D, 0xAE, 0x2A, 0xF5, 0xB0, 0xC8, 0xEB, 0xBB, 0x3C, 0x83, 0x53, 0x99, 0x61,
			0x17, 0x2B, 0x04, 0x7E, 0xBA, 0x77, 0xD6, 0x26, 0xE1, 0x69, 0x14, 0x63, 0x55, 0x21, 0x0C, 0x7D
		};

		template <std::size_t KeySize>
		const std::uint8_t AES<KeySize>::Rcon[11] = { 0x8d, 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80, 0x1b, 0x36 };

		template AES<128>;
		template AES<192>;
		template AES<256>;
	}
}