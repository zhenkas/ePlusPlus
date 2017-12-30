#pragma once

namespace EPP::Templates
{
	template<typename...T>
	using make_void = void;

	template<typename TResolver, int priority>
	struct type_resolver_base
	{
		using get_next_resolver = type_resolver_base<TResolver, priority - 1>;
	};
	template<typename TResolver>
	struct type_resolver_base<TResolver, -1>;

	template<typename TResolver, typename T, typename TCheck>
	struct type_resolver;

	template<typename TResolver, typename T, typename TCheck>
	struct type_resolver<type_resolver_base<TResolver, -1>, T, TCheck> {};

	template<typename TResolver, typename T, typename TCheck>
	struct search_type_resolver : search_type_resolver<typename TResolver::get_next_resolver, T, TCheck> {};

	template<typename TResolver, typename T>
	struct search_type_resolver<TResolver, T, void> : type_resolver<TResolver, T, void> {};

	template<typename TResolver, typename T>
	using resolve_type = search_type_resolver<TResolver, T, void>;
}