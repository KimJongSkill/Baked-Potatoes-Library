#pragma once

#include <cstdint>
#include <array>
#include <string>
#include <memory>
#include "Hash.hpp"
#include "Utility.hpp"

namespace bpl
{
	namespace crypt
	{
		namespace MAC
		{
			template <class Hash>
			class HMAC
			{
				std::unique_ptr<bpl::crypt::hash::HashInterface> Function;

			public:
				HMAC() : Function(new Hash) {};

				std::string operator()(std::string Key, const  std::string& Message)
				{
					if (Key.length() > Function->BlockSize())
						Key = Function->Hash(Key, false);
					if (Key.length() < Function->BlockSize())
						Key.resize(Function->BlockSize(), 0x00);

					std::string InnerKeyPad(64, 0x00);
					std::string OutterKeyPad(64, 0x00);

					std::transform(Key.begin(), Key.end(), InnerKeyPad.begin(), [](std::string::value_type Byte) { return Byte ^ 0x36; });
					std::transform(Key.begin(), Key.end(), OutterKeyPad.begin(), [](std::string::value_type Byte) { return Byte ^ 0x5c; });

					return Function->Hash(OutterKeyPad + Function->Hash(InnerKeyPad + Message, false));
				}
			};

			typedef HMAC<bpl::crypt::hash::SHA1Context> HMAC_SHA1;
		}
	}
}