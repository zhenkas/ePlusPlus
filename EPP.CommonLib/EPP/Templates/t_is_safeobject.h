#pragma once
#include <type_traits>

namespace EPP::Holders
{
	template<typename T>
	struct SafeObject;
}

namespace EPP::Templates
{
	template<typename T>
	struct test_is_safeobject :std::false_type {};

	template<typename T>
	struct test_is_safeobject<EPP::Holders::SafeObject<T>> : std::true_type
	{
		using type = T;
	};


	template<typename T>
	using is_safeobject = test_is_safeobject<std::remove_const_t<std::remove_reference_t<T>>>;

	template<typename T>
	constexpr bool is_safeobject_v = test_is_safeobject<std::remove_const_t<std::remove_reference_t<T>>>::value;

	template<typename T>
	using get_safeobject_t = typename test_is_safeobject<std::remove_const_t<std::remove_reference_t<T>>>::type;

}