#pragma once

#include <vector>
#include <iterator>
#include <iostream>
#include <cstdint>
#include "Image.hpp"

namespace bpl
{
	namespace image
	{
		class BMP
		{
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

		public:
			template <class Pixel>
			void DumpInfo(const Image<Pixel>& Object, std::ostream& Output = std::cout) const;

			template <class Pixel>
			void Read(const std::string& Source, Image<Pixel>& Target);

			template <class Pixel>
			Image<Pixel> Read(const std::string& Source);
			
			void Write();

		private:
			void SetFile(const std::string& Source);

			void SetFile(std::fstream&& Source);

			template <class Type>
			void ReadFromFile(Type& Variable) const;

			template <class Type>
			void WriteToFile(Type& Variable) const;

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