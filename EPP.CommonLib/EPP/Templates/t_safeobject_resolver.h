#pragma once
#include <type_traits>
#include "t_type_resolver.h"

namespace EPP::Holders
{
	template<typename T>
	struct SafeObject;
}

namespace EPP::Templates
{
	using namespace EPP::Holders;


	template<int priority>
	struct SafeObjectResolver;

	template<typename T>
	using resolve_safeobject = resolve_type<SafeObjectResolver<1>, std::remove_const_t<std::remove_reference_t<T>>>;


	template<typename T>
	struct type_resolver<SafeObjectResolver<1>, SafeObject<T>, void>
	{
		static const bool is_safeobject_holder = true;
		using isafeobject_t = T;
	};

	template<typename T>
	struct type_resolver<SafeObjectResolver<1>, T, void>
	{
		static const bool is_safeobject_holder = false;
	};

	template<typename T>
	constexpr bool is_safeobject_v = resolve_safeobject<T>::is_safeobject_holder;

	template<typename T>
	using get_isafeobject_t = typename resolve_safeobject<T>::isafeobject_t;

}