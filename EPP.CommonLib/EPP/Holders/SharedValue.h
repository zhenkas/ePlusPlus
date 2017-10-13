#pragma once
#include <EPP\Holders\SafeObject.h>


namespace EPP::Holders
{
	template<typename T>
	struct TSharedValue : ISafeObject
	{
		T value;
	};

	template<typename T>
	using SharedValue = std::conditional_t<std::is_base_of<ISafeObject, T>::value, SafeObject<T>, SafeObject<TSharedValue<T>>>;

}