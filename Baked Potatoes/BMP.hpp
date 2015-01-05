#pragma once

#include <vector>
#include <fstream>
#include <iterator>
#include <iostream>
#include <cstdint>
#include "Pixel.hpp"

namespace bpl
{
	namespace image
	{
		class BMP
		{
		public:
			using pixel_type = Pixel24;
			
		private:
			struct BitmapFileHeader
			{
				int8_t	 ID[2];
				uint32_t BMPSize;
				uint16_t Reserved1;
				uint16_t Reserved2;
				uint32_t PixelArrayOffset;
			} BMPHeader;

			struct BITMAPINFOHEADER
			{
				uint32_t	HeaderSize;
				int32_t		BitmapWidth;
				int32_t		BitmapHeight;
				uint16_t	ColourPlanes;
				uint16_t	BPP;
				uint32_t	Compression;
				uint32_t	PixelArraySize;
				int32_t		PrintResolutionH;
				int32_t		PrintResolutionV;
				uint32_t	PaletteColours;
				uint32_t	ImportantColours;
			} DIBHeader;

			std::vector<std::vector<Pixel24>> PixelArray;
			mutable std::fstream File;

		public:
			BMP(const std::string& Source);

			BMP(std::fstream&& Source);

			BMP(const BMP& Other);

			BMP(BMP&& Other);

			BMP& operator=(const BMP& Other);

			BMP& operator=(BMP&& Other);

			std::size_t Size() const;

			std::size_t PixelSize() const;

			std::size_t Width() const;
			
			std::size_t Height() const;

			void Resize(std::size_t x, std::size_t y);

			void DumpInfo(std::ostream& Output = std::cout) const;

			Pixel24& operator()(std::size_t x, std::size_t y);

			const Pixel24& operator()(std::size_t x, std::size_t y) const;

			void Read();

			void Write();

		private:
			void SetFile(const std::string& Source);

			void SetFile(std::fstream&& Source);

			template <class Type>
			void ReadFromFile(Type& Variable) const;

			template <class Type>
			void WriteToFile(Type& Variable) const;

			template <>
			void WriteToFile(Pixel24& Variable) const;

			void CheckFile() const;

			void ReadBMPHeader();

			void ReadDIBHeader();

			void ReadPixelArray();

			void WriteBMPHeader();

			void WriteDIBHeader();

			void WritePixelArray();
		};
	}
}