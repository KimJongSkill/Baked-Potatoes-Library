#include "BMP.hpp"

#include <fstream>
#include <string>
#include <cassert>
#include <exception>
#include "Pixel.hpp"
#include "Counter.hpp"
/*
namespace bpl
{
	namespace image
	{
		BMP::BMP(const std::string& Source) : File(Source, std::ios::in | std::ios::out | std::ios::binary)
		{
			CheckFile();

			Read();
		}

		BMP::BMP(std::fstream&& Source) : File(std::move(Source))
		{
			CheckFile();

			Read();
		}

		BMP::BMP(const BMP& Other) : BMPHeader(Other.BMPHeader), DIBHeader(Other.DIBHeader), PixelArray(Other.PixelArray) {};

		void BMP::ReadBMPHeader()
		{
			CheckFile();

			File.seekg(0, std::ios::beg);

			ReadFromFile(BMPHeader.ID[0]);
			ReadFromFile(BMPHeader.ID[1]);

			ReadFromFile(BMPHeader.BMPSize);

			ReadFromFile(BMPHeader.Reserved1);
			ReadFromFile(BMPHeader.Reserved2);

			ReadFromFile(BMPHeader.PixelArrayOffset);
		}

		void BMP::ReadDIBHeader()
		{
			CheckFile();

			File.seekg(14, std::ios::beg);

			ReadFromFile(DIBHeader.HeaderSize);

			if (DIBHeader.HeaderSize != 40)
				throw std::runtime_error("Unsupported DIB Header(" + std::to_string(DIBHeader.HeaderSize) + " bytes)");

			ReadFromFile(DIBHeader.BitmapWidth);
			ReadFromFile(DIBHeader.BitmapHeight);

			ReadFromFile(DIBHeader.ColourPlanes);

			ReadFromFile(DIBHeader.BPP);

			ReadFromFile(DIBHeader.Compression);

			ReadFromFile(DIBHeader.PixelArraySize);

			ReadFromFile(DIBHeader.PrintResolutionH);
			ReadFromFile(DIBHeader.PrintResolutionV);

			ReadFromFile(DIBHeader.PaletteColours);
			ReadFromFile(DIBHeader.ImportantColours);
		}

		void BMP::ReadPixelArray()
		{
			CheckFile();

			File.seekg(BMPHeader.PixelArrayOffset, std::ios::beg);

			PixelArray.clear();
			PixelArray.resize(DIBHeader.BitmapHeight);
			for (auto& i : PixelArray)
				i.resize(DIBHeader.BitmapWidth);

			for (auto& i : PixelArray)
			{
				for (auto& j : i)
				{
					pixel_type::component_type Blue = File.get();
					pixel_type::component_type Green = File.get();
					pixel_type::component_type Red = File.get();
					
					j = pixel_type { Red, Green, Blue };
				}
			}
		}

		void BMP::Read()
		{
			CheckFile();

			ReadBMPHeader();
			ReadDIBHeader();
			ReadPixelArray();
		}

		void BMP::WriteBMPHeader()
		{
			CheckFile();

			File.seekp(0, std::ios::beg);

			File.put(BMPHeader.ID[0]);
			File.put(BMPHeader.ID[1]);

			WriteToFile(BMPHeader.BMPSize);

			WriteToFile(BMPHeader.Reserved1);
			WriteToFile(BMPHeader.Reserved2);

			WriteToFile(BMPHeader.PixelArrayOffset);
		}

		void BMP::WriteDIBHeader()
		{
			CheckFile();

			File.seekp(14, std::ios::beg);

			WriteToFile(DIBHeader.HeaderSize);

			WriteToFile(DIBHeader.BitmapWidth);
			WriteToFile(DIBHeader.BitmapHeight);

			WriteToFile(DIBHeader.ColourPlanes);

			WriteToFile(DIBHeader.BPP);

			WriteToFile(DIBHeader.Compression);

			WriteToFile(DIBHeader.PixelArraySize);

			WriteToFile(DIBHeader.PrintResolutionH);
			WriteToFile(DIBHeader.PrintResolutionV);

			WriteToFile(DIBHeader.PaletteColours);
			WriteToFile(DIBHeader.ImportantColours);
		}

		void BMP::WritePixelArray()
		{
			CheckFile();

			File.seekp(BMPHeader.PixelArrayOffset, std::ios::beg);

			Counter<> Padding(1, DIBHeader.BitmapWidth);
			Padding.SetCallback([&](){
				for (int p = 0; p < DIBHeader.BitmapWidth % 4; ++p)
				{
					File.put('\0');
				}
			});

			for (auto i : PixelArray)
			{
				WriteToFile(i);
				++Padding;
			}
		}

		void BMP::Write()
		{
			CheckFile();

			WriteBMPHeader();
			WriteDIBHeader();
			WritePixelArray();
		}

		auto BMP::operator()(std::size_t x, std::ptrdiff_t y) -> pixel_reference
		{
			return const_cast<pixel_type&>(const_cast<const BMP*>(this)->operator()(x, y));
		}

		auto BMP::operator()(std::size_t x, std::ptrdiff_t y) const -> pixel_const_reference
		{
			return PixelArray[x][std::abs(y)];
		}

		auto BMP::at(std::size_t x, std::ptrdiff_t y) -> pixel_reference
		{
			return const_cast<pixel_reference>(const_cast<const BMP*>(this)->at(x, y));
		}

		auto BMP::at(std::size_t x, std::ptrdiff_t y) const -> pixel_const_reference
		{
			if (x > DIBHeader.BitmapWidth)
				throw std::out_of_range("Width is out of bounds");
			if ((y > 0 && y > DIBHeader.BitmapHeight) || (y < 0 && y < DIBHeader.BitmapHeight))
				throw std::out_of_range("Height is out of bounds");

			return operator()(x, y);
		}

		template <class Type>
		void BMP::ReadFromFile(Type& Variable) const
		{
			File.read(reinterpret_cast<char*>(&Variable), sizeof(Variable));
		}

		template <>
		void BMP::ReadFromFile(pixel_reference Variable) const
		{
			ReadFromFile(Variable.Blue);
			ReadFromFile(Variable.Green);
			ReadFromFile(Variable.Red);
		}

		template <class Type>
		void BMP::WriteToFile(Type& Variable) const
		{
			File.write(reinterpret_cast<char*>(&Variable), sizeof(Variable));
		}

		template <>
		void BMP::WriteToFile(pixel_reference Variable) const
		{
			File.put(static_cast<char>(Variable.Blue));
			File.put(static_cast<char>(Variable.Green));
			File.put(static_cast<char>(Variable.Red));
		}

		void BMP::CheckFile() const
		{
			if (!File)
				throw std::ios_base::failure("Can't open BMP File");
		}

		void BMP::SetFile(const std::string& Source)
		{
			std::fstream TempFile;

			TempFile.open(Source, std::ios::in | std::ios::out | std::ios::binary); // Try an existing file
			if (!File.is_open()) // Else create a new file
				TempFile.open(Source, std::ios::in | std::ios::out | std::ios::binary | std::ios::trunc);
		
			SetFile(std::move(TempFile));
		}

		void BMP::SetFile(std::fstream&& Source)
		{
			if (File.is_open())
				File.close();

			File = std::move(Source);

			CheckFile();
		}

		std::size_t BMP::Size() const
		{
			return BMPHeader.BMPSize;
		}

		std::size_t BMP::PixelSize() const
		{
			return BMPHeader.BMPSize - BMPHeader.PixelArrayOffset;
		}

		std::size_t BMP::Width() const
		{
			return DIBHeader.BitmapWidth;
		}

		std::size_t BMP::Height() const
		{
			return DIBHeader.BitmapHeight;
		}

		void BMP::DumpInfo(std::ostream& Output) const
		{
			Output << "BMP Header\n" << "----------\n";

			Output << "ID Field: " << BMPHeader.ID[0] << BMPHeader.ID[1] << '\n';

			Output << "BMP Size: " << BMPHeader.BMPSize << '\n';

			Output << "Unused 1: " << BMPHeader.Reserved1 << "\nUnused 2: " << BMPHeader.Reserved2 << '\n';

			Output << "Pixel Array Offset: " << BMPHeader.PixelArrayOffset << "\n\n";

			Output << "DIB Header\n" << "----------\n";

			Output << "Header Size: " << DIBHeader.HeaderSize << '\n';

			Output << "Width: " << DIBHeader.BitmapWidth << "\nHeight: " << DIBHeader.BitmapHeight << '\n';

			Output << "Colour Planes: " << DIBHeader.ColourPlanes << '\n';

			Output << "Bits Per Pixel: " << DIBHeader.BPP << '\n';

			Output << "Compression: " << DIBHeader.Compression << '\n';

			Output << "Pixel Array Size: " << DIBHeader.PixelArraySize << '\n';

			Output << "Print Resoultion Horizontal: " << DIBHeader.PrintResolutionH <<
				"\nPrint Resolution Vertical: " << DIBHeader.PrintResolutionV << '\n';

			Output << "Colours in the Palette: " << DIBHeader.PaletteColours << '\n';

			Output << "Important Colours: " << DIBHeader.ImportantColours << "\n\n";
		}
	}
}*/