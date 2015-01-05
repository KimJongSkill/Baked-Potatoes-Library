#pragma once

#include <exception>
#include <algorithm>

namespace bpl
{
	namespace memory
	{
		template <class Object>
		class Pool
		{
		public:
			using value_type = Object;
			using size_type = std::size_t;
			using difference_type = std::ptrdiff_t;
			using pointer = Object*;
			using const_pointer = Object const*;
			using reference = Object&;
			using const_reference = Object const&;

			explicit Pool(size_type Size) : RawMemory(::operator new[](Size * sizeof(value_type))), Memory(static_cast<value_type*>(RawMemory)),
				Capacity(Size), Index(0), Log(new bool[Size] { false }) { }

			~Pool()
			{
				delete[] Log;
				::operator delete[](RawMemory);
			}

			Pool(Pool<Object>&& Other) : RawMemory(Other.RawMemory), Memory(Other.Memory), Capacity(Other.Capacity), Index(Other.Index),
				Log(Other.Log)
			{
				Other.RawMemory = nullptr;
				Other.Memory = nullptr;
				Other.Log = nullptr;
			}

			Pool& operator=(Pool<Object>&& Other)
			{
				if (&Other != this)
				{
					clear();
					operator delete[](RawMemory);

					RawMemory = Other.RawMemory;
					Other.RawMemory = nullptr;

					Memory = Other.Memory;
					Other.Memory = nullptr;

					Capacity = Other.Capacity;
					Other.Capacity = 0;

					Index = Other.Index;
					Other.Index = 0;

					Log = Other.Log;
					Other.Log = nullptr;
				}

				return *this;
			}

			Pool(const Pool<Object>&) = delete;
			Pool& operator=(const Pool<Object>&) = delete;

			template <class... Args>
			pointer emplace(Args&&... Arguments)
			{
				size_type i = 0;

				if (Index < Capacity && !Log[Index])
				{
					i = Index++;
				}
				else
				{
					while (i < Capacity && Log[i])
						++i;

					if (i == Capacity)
						throw std::bad_alloc();

					Index = i + 1;
				}

				Log[i] = true;

				return new(Memory + i) value_type(Arguments...);
			}

			void erase(value_type const* Where)
			{
				Log[Where - Memory] = false;
				Where->~value_type();
			}

			const size_type size() const
			{
				return std::count(Log, Log + Capacity, true);
			}

			const size_type capacity() const
			{
				return Capacity;
			}

			const bool empty() const
			{
				return std::none_of(Log, Log + Capacity, [](bool Value) { return Value; });
			}

			void clear()
			{
				if (!empty())
				{
					for (size_type i = 0; i < Capacity; ++i)
					{
						if (Log[i])
						{
							(Memory + i)->~value_type();
							Log[i] = false;
						}
					}
				}

				Index = 0;
			}

		private:
			void* RawMemory;
			value_type* Memory;
			size_type Capacity;
			size_type Index;
			bool* Log;
		};
	}
}