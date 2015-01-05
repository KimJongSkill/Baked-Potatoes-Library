#pragma once

#include <iterator>
#include <vector>
#include <exception>
#include <atomic>
#include <thread>
#include <iostream>

namespace bpl
{
	namespace sort
	{
		static std::atomic<std::size_t> ThreadCount(1);
		static std::size_t MaximumThreadCount(std::thread::hardware_concurrency());

		template <class Order, class Iterator>
		void Mergesort(Iterator const Begin, Iterator const End)
		{
			using Type = Iterator::value_type;
			using Storage = std::vector<Type>;
			using std::swap;

			std::ptrdiff_t const Distance = std::distance(Begin, End);

			if (Distance == 2)
			{
				if (!Order::Compare(Begin, End - 1))
				{
					swap(*Begin, *(End - 1));
				}
			}

			else if (Distance > 2)
			{
				Storage Temp(Begin, End);
				Storage::iterator Middle = std::next(Temp.begin(), Distance / 2);

				if (ThreadCount.load(std::memory_order_acquire) < MaximumThreadCount && Distance > 16)
				{
					std::thread Thread(Mergesort<Order, Iterator>, Temp.begin(), Middle);

					ThreadCount.fetch_add(1, std::memory_order_release);

					Mergesort<Order, Iterator>(Middle, Temp.end());

					Thread.join();
				}
				else
				{
					if (std::distance(Temp.begin(), Middle) > 1)
						Mergesort<Order, Iterator>(Temp.begin(), Middle);

					if (std::distance(Middle, Temp.end()) > 1)
						Mergesort<Order, Iterator>(Middle, Temp.end());
				}

				/*if (Order::Compare(Middle - 1, Middle))
				{
					std::move(Temp.begin(), Temp.end(), Begin);
				}
				else if (Order::Compare(Begin, End - 1))
				{
					Rotate(Temp.begin(), Middle, Temp.end());
					std::move(Temp.begin(), Temp.end(), Begin);
				}
				else*/
				{
					Storage::iterator StorageIter1 = Temp.begin();
					Storage::iterator StorageIter2 = Middle;

					for (Iterator i = Begin; i != End; ++i)
					{
						if (StorageIter1 == Middle)
						{
							*i = std::move(*StorageIter2);
							++StorageIter2;
						}
						else if (StorageIter2 == Temp.end())
						{
							*i = std::move(*StorageIter1);
							++StorageIter1;
						}
						else
						{
							if (Order::Compare(StorageIter1, StorageIter2))
							{
								*i = std::move(*StorageIter1);
								++StorageIter1;
							}
							else
							{
								*i = std::move(*StorageIter2);
								++StorageIter2;
							}
						}
					}
				}
			}

			else if (Distance < 0)
			{
				throw std::invalid_argument("Invalid Mergesort distance (" + std::to_string(Distance) + ")");
			}
		}

		template <class Iterator>
		void Reverse(Iterator Begin, Iterator End)
		{
			for (--End; Begin < End; Begin++, End--)
				std::swap(*Begin, *End);
		}

		template <class Iterator>
		void Rotate(Iterator Begin, Iterator Middle, Iterator End)
		{
			if (Begin != Middle && Middle != End)
			{
				Reverse(Begin, Middle);
				Reverse(Middle, End);
				Reverse(Begin, End);
			}
		}

		struct Ascending
		{
			template <class Iterator>
			static bool Compare(const Iterator& a, const Iterator& b)
			{
				return *a <= *b;
			}
			/*template <class Iterator>
			static bool IsSorted(const Iterator& Begin, const Iterator& Middle, const Iterator& End)
			{
			return *(Middle - 1) <= *Middle;
			}
			template <class Iterator>
			static bool IsInverted(const Iterator& Begin, const Iterator& Middle, const Iterator& End)
			{
			return *(End - 1) <= *Begin;
			}*/
		};

		struct Descending
		{
			template <class Iterator>
			static bool Compare(const Iterator& a, const Iterator& b)
			{
				return *a >= *b;
			}
			/*template <class Iterator>
			static bool IsSorted(const Iterator& Begin, const Iterator& Middle, const Iterator& End)
			{
			return *(Middle - 1) >= *Middle;
			}
			template <class Iterator>
			static bool IsInverted(const Iterator& Begin, const Iterator& Middle, const Iterator& End)
			{
			return *(End - 1) >= *Begin;
			}*/
		};
	}
}