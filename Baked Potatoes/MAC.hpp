#pragma once

#include <cstdint>
#include <array>
#include <string>
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
				Hash Function;

			public:
				std::array<std::uint8_t, Hash::DigestSize / 8> operator()(std::string Key, const  std::string& Message)
				{
					using bpl::utility::ByteToByteString;

					if (Key.length() > Hash::BlockSize)
						Key = ByteToByteString(Function.Hash(Key));
					if (Key.length() < Hash::BlockSize)
						Key.resize(Hash::BlockSize, 0x00);

					std::string InnerKeyPad(64, 0x00);
					std::string OutterKeyPad(64, 0x00);

					std::transform(Key.begin(), Key.end(), InnerKeyPad.begin(), [](std::string::value_type Byte) { return Byte ^ 0x36; });
					std::transform(Key.begin(), Key.end(), OutterKeyPad.begin(), [](std::string::value_type Byte) { return Byte ^ 0x5c; });

					std::string InnerString = InnerKeyPad + Message;
					auto InnerHash = Function.Hash(InnerString);
					std::string InnerResult = ByteToByteString(InnerHash);
					return Function.Hash(OutterKeyPad + InnerResult);
				}
			};

			typedef HMAC<bpl::crypt::hash::SHA1Context> HMAC_SHA1;
		}
	}
}