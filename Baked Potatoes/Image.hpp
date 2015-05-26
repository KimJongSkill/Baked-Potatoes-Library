#pragma once

#include <vector>
#include <functional>
#include <iostream>
#include <algorithm>
#include "Defines.hpp"

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
			using array_type = std::vector<value_type>;
			using data_type = std::vector<array_type>;

			/* 
			* To be expanded as support for more formats is hopefully added
			* Allows the client code to query the file format of the image
			* Virtual means the image was not loaded from a file
			*/
			enum class Format { Virtual, BMP };

		private:
			data_type PixelArray;

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

				if (!Extension.compare("bmp")) // Returns 0 when equal 
					; // BMP load image
			}

			virtual Format GetFormat() const
			{
				return Format::Virtual;
			}

			Image(std::size_t x, std::size_t y)
			{
				resize(x, y);
			}

			Image(const Image<value_type>& Other) = default;

			Image(Image<value_type>&& Other) : PixelArray(std::move(Other.PixelArray)) {};

			Image<value_type>& operator=(const Image<value_type>& Other) = default;

			Image<value_type>& operator=(Image<value_type>&& Other)
			{
				PixelArray = std::move(Other.PixelArray);
			}

			reference operator()(std::size_t x, std::size_t y)
			{
				return const_cast<value_type&>(const_cast<const Image<value_type>*>(this)->operator()(x, y));
			}

			const_reference operator()(std::size_t x, std::size_t y) const
			{
				return PixelArray[y][x];
			}

			reference at(std::size_t x, std::size_t y)
			{
				return const_cast<value_type&>(const_cast<const Image<value_type>*>(this)->at(x, y));
			}

			const_reference at(std::size_t x, std::size_t y) const
			{
				if (x > width() - 1)
					throw std::out_of_range("Width is out of bounds");
				if (y > height() - 1)
					throw std::out_of_range("Height is out of bounds");

				return operator()(x, y);
			}

			std::size_t width() const
			{
				if (PixelArray.empty())
					return 0;
				return PixelArray.front().size();
			}

			std::size_t height() const
			{
				return PixelArray.size();
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

			data_type* data()
			{
				return &PixelArray;
			}

			const data_type* data() const
			{
				return &PixelArray;
			}

			void resize(std::size_t x, std::size_t y)
			{
				PixelArray.resize(y);
				std::for_each(PixelArray.begin(), PixelArray.end(), [&](array_type& i) { i.resize(x); });
			}

			void apply(std::function<value_type(const_reference)> Function)
			{
				for (auto& i : PixelArray)
					std::transform(i.begin(), i.end(), i.begin(), Function);
			}

			void fill(const_reference Pixel)
			{
				for (auto& i : PixelArray)
					for (auto& j : i)
						j = Pixel;
			}

			void mirror(bpl::Axis Axis)
			{
				//Calls to std::vector<T>::reserve guarantee that our iterators are not invalidated

				if (Axis == bpl::Axis::x)
				{
					/*resize(width(), height() * 2);

					for (std::size_t i = 0; i < height() / 2; ++i)
					{
						PixelArray[height() - i] = PixelArray[i];
					}*/

					PixelArray.reserve(height() * 2);
					/*for (auto i = PixelArray.crbegin(); i != PixelArray.crend(); ++i)
					{
					PixelArray.push_back(*i);
					}*/
					if (height() > 1)
						std::for_each(PixelArray.crbegin(), PixelArray.crend(), [&](const array_type& i) { PixelArray.push_back(i); });
					else
						PixelArray.push_back(PixelArray.front());
				}
				else if (Axis == bpl::Axis::y)
				{
					//resize(width() * 2, height());

					for (auto& i : PixelArray)
					{
						i.reserve(width() * 2);
						for (std::ptrdiff_t j = i.size() - 1; j >= 0; --j)
						{
							i.push_back(i[j]);
						}
					}
					/*std::for_each(PixelArray.begin(), PixelArray.end(), std::bind(&array_type::reserve, std::placeholders::_1, width() * 2));
					std::for_each(PixelArray.begin(), PixelArray.end(), [&](array_type& i) { std::for_each(i.crbegin(), i.crend(), [&](const value_type& j) { i.push_back(j); }); });*/
				}
				else
				{
					throw std::out_of_range("2D image does not have a z direction");
				}
			}

			void flip(bpl::Axis Axis)
			{
				using std::swap;

				if (Axis == bpl::Axis::x)
				{
					for (std::size_t y = 0; y < height(); ++y)
					{
						swap(PixelArray[y], PixelArray[height() - y - 1]); // Height returns a one-based index
					}
				}
				else if (Axis == bpl::Axis::y)
				{
					for (std::size_t y = 0; y < height(); ++y)
					{
						for (std::size_t x = 0; x < width(); ++x)
						{
							swap(this->operator()(x, y), this->operator()(width() - x - 1, y)); // Width returns a one-based index
						}
					}
				}
				else
				{
					throw std::out_of_range("2D image does not have a z direction");
				}
			}

			void rotate(bpl::Rotation Degrees)
			{
				using std::swap;

				std::function<std::size_t(std::size_t, std::size_t)> xFunction;
				std::function<std::size_t(std::size_t, std::size_t)> yFunction;
				std::size_t NewWidth;
				std::size_t NewHeight;

				switch (Degrees)
				{
					case bpl::Rotation::_90:
						xFunction = [this](std::size_t x, std::size_t y) { return height() - 1 - y; };
						yFunction = [this](std::size_t x, std::size_t y) { return x; };
						NewWidth = height();
						NewHeight = width();
						break;
					case bpl::Rotation::_180:
						xFunction = [this](std::size_t x, std::size_t y) { return width() - 1 - x; };
						yFunction = [this](std::size_t x, std::size_t y) { return height() - 1 - y; };
						NewWidth = width();
						NewHeight = height();
						break;
					case bpl::Rotation::_270:
						xFunction = [this](std::size_t x, std::size_t y) { return y; };
						yFunction = [this](std::size_t x, std::size_t y) { return width() - 1 - x; };
						NewWidth = height();
						NewHeight = width();
						break;
				}

				Image<value_type> Temp(NewWidth, NewHeight);

				for (std::size_t y = 0; y < height(); ++y)
				{
					for (std::size_t x = 0; x < width(); ++x)
					{
						Temp(xFunction(x, y), yFunction(x, y)) = this->operator()(x, y);
					}
				}

				swap(Temp, *this);
			}

			bool operator==(const Image<value_type>& Other) const
			{
				if (this == &Other)
					return true;

				if (width() != Other.width())
					return false;
				if (height() != Other.height())
					return false;

				for (std::size_t y = 0; y < height(); ++y)
					for (std::size_t x = 0; x < width(); ++x)
						if (this->operator()(x, y) != Other(x, y))
							return false;

				return true;
			}

			bool operator!=(const Image<value_type>& Other) const
			{
				return !this->operator==(Other);
			}

			void swap(Image<value_type>& Other)
			{
				PixelArray.swap(Other.PixelArray);
			}
		};

		template <class Pixel>
		void swap(Image<Pixel>& Left, Image<Pixel>& Right)
		{
			Left.swap(Right);
		}
	}
}