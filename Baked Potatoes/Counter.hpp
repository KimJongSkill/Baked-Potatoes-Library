#pragma once

#include <type_traits>
#include <functional>

template <class Type = int>
class Counter
{
	Type Min, Max;
	Type Storage;
	std::function<void()> Callback;

public:
	/*Counter(Type Value, std::function<void(void)> Function) : Storage(Value), Callback(Function)
	{
	static_assert(std::is_arithmetic<Type>::value, "Counter requires an arithmetic type");

	if (Value > Max || Value < Min)
	throw std::out_of_range("Counter initialized with out-of-bounds Value");
	};*/

	Counter(Type Floor, Type Ceil, Type Value) : Min(Floor), Max(Ceil), Storage(Value)
	{
		static_assert(std::is_arithmetic<Type>::value, "Counter requires an arithmetic type");

		if (Value > Max || Value < Min)
			throw std::out_of_range("Counter initialized with out-of-bounds Value");
	}

	Counter(Type Floor, Type Ceil) : Counter(Floor, Ceil, Floor) {};

	void SetCallback(const std::function<void()>& Function)
	{
		Callback = Function;
	}

	operator Type()
	{
		return Storage;
	}

	Counter operator++()
	{
		if (Storage < Max)
			++Storage;
		else
		{
			Storage = Min;
			Callback();
		}

		return *this;
	}

	Counter operator++(int)
	{
		Counter<Type> Temp;

		operator++();

		return Temp;
	}

	Counter operator--()
	{
		if (Storage > Min)
			--Storage;
		else
		{
			Storage = Min;
			Callback();
		}

		return *this;
	}

	Counter operator--(int)
	{
		Counter<Type> Temp;

		operator--();

		return Temp;
	}

	bool operator<(Counter<Type>& rhs) const
	{
		return Storage < rhs.Storage;
	}

	bool operator>(Counter<Type>& rhs) const
	{
		return Storage > rhs.Storage;
	}

	bool operator==(Counter<Type>& rhs) const
	{
		return Storage == rhs.Storage;
	}

	bool operator!=(Counter<Type>& rhs) const
	{
		return Storage != rhs.Storage;
	}

	bool operator<(Type& rhs) const
	{
		return Storage < rhs;
	}

	bool operator>(Type& rhs) const
	{
		return Storage > rhs;
	}

	bool operator==(Type& rhs) const
	{
		return Storage == rhs;
	}

	bool operator!=(Type& rhs) const
	{
		return Storage != rhs;
	}
};