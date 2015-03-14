#pragma once

#include <vector>
#include <functional>
#include <iostream>
#include <algorithm>

namespace bpl
{
	namespace image
	{
		template <class Pixel>
		class Image
		{
		public:
			using value_type = Pixel;
			using reference = Pixel&;
			using const_reference = const Pixel&;
			using pointer = Pixel*;
			using const_pointer = const Pixel*;

		private:
			std::vector<std::vector<value_type>> PixelArray;

		public:
			Image() = default;

			~Image() = default;

			Image(const std::string& Source)
			{
				std::size_t ExtensionPoint = Source.find_last_of('.');
				if (ExtensionPoint == std::string::npos)
					return std::invalid_argument("No file extension in " + Source);

				std::string Extension = Source.substr(ExtensionPoint);
				if (Extension.length() == 0)
					return std::invalid_argument("No file extension in " + Source);
				std::transform(Extension.begin(), Extension.end(), Extension.begin(), ::tolower);

				std::ifstream File;
				File.open(Source, std::ios::in || std::ios::binary);

				if (!File.is_open())
					throw std::invalid_argument("Could not open " + Source);

				if (!Extension.compare("bmp") // Returns 0 when equal 
					; // BMP load image
			}

			Image(const Image<value_type>& Other) = default;

			Image(Image<value_type>&& Other) = default;

			Image<value_type>& operator=(const Image<value_type>& Other) = default;

			Image<value_type>& operator=(Image<value_type>&& Other) = default;

			reference operator()(std::size_t x, std::ptrdiff_t y)
			{
				return const_cast<value_type&>(const_cast<const Image<value_type>*>(this)->operator()(x, y));
			}

			const_reference operator()(std::size_t x, std::ptrdiff_t y) const
			{
				return PixelArray[x][std::abs(y)];
			}

			reference at(std::size_t x, std::ptrdiff_t y)
			{
				return const_cast<value_type&>(const_cast<const Image<value_type>*>(this)->at(x, y));
			}

			const_reference at(std::size_t x, std::ptrdiff_t y) const
			{
				if (x > width())
					throw std::out_of_range("Width is out of bounds");
				if ((y > 0 && y > height()) || (y < 0 && y < height()))
					throw std::out_of_range("Height is out of bounds");

				return operator()(x, y);
			}

			std::size_t width() const
			{
				return PixelArray.size();
			}

			std::size_t height() const
			{
				return PixelArray.front().size();
			}

			std::size_t depth() const
			{
				return value_type::Depth;
			}

			std::size_t size() const
			{
				return width() * height();
			}

			bool empty() const
			{
				return PixelArray.empty();
			}

			void clear()
			{
				PixelArray.clear();
			}

			void resize(std::size_t x, std::ptrdiff_t y)
			{
				PixelArray.resize(x);
				std::for_each(PixelArray.begin(), PixelArray.end(), [](std::vector<value_type>& x) {x.resize(y); });
			}

			void apply(std::function<value_type(const_reference)> Function)
			{
				for (auto& i : PixelArray)
					std::for_each(i.begin(), i.end(), Function);
			}

			void apply(const_reference Pixel)
			{
				for (auto& i : PixelArray)
					for (auto& j : i)
						j = Pixel;
			}

			void swap(Image<value_type>& Other)
			{
				PixelArray.swap(Other.PixelArray);
			}

			void erase_row(std::size_t x)
			{
				PixelArray.erase(x);
			}

			void erase_column(std::ptrdiff_t y)
			{
				std::for_each(PixelArray.begin(), PixelArray.end(), std::bind(&std::vector<value_type>::erase, y);
			}

			void insert_row(std::size_t x, std::vector<value_type>&& Vector)
			{
				PixelArray.insert(x, std::forward(Vector));
			}

			void insert_row(std::size_t x, const_reference Value)
			{
				insert_row(x, std::vector<value_type>(width(), Value));
			}

			void insert_column(std::size_t y, const std::vector<value_type>& Vector)
			{
				for (std::size_t i = 0; i < height(); ++i)
					PixelArray[i].insert(y, Vector[i]);
			}

			void insert_column(std::size_t y, const_reference Value)
			{
				std::for_each(PixelArray.begin(), PixelArray.end(), std::bind(&std::vector<std::vector<value_type>>::insert, y));
			}
		};

		template <class Pixel>
		void swap(Image<Pixel>& Left, Image<Pixel>& Right)
		{
			Left.swap(Right);
		}
	}
}