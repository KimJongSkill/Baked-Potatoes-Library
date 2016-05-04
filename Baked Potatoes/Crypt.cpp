#include "Crypt.hpp"

#include <cstring>

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
			// In case RDRAND fails to return a random number,
			// it is recommended to retry 10 times
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
	}
}