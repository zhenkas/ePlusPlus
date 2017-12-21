#pragma once
#include "t_function_resolver.h"

namespace EPP::Templates
{
	struct LambdaResolverID {};

	template<int priority>
	using LambdaResolver = type_resolver_base<LambdaResolverID, priority>;

	template<typename T>
	using resolve_lambda = resolve_type<LambdaResolver<1>, std::remove_const_t<std::remove_reference_t<T>>>;

	template<typename TLambda>
	struct type_resolver<LambdaResolver<1>, TLambda, make_void<decltype(&TLambda::operator())>>
	{
		using t_function = resolve_function<decltype(&TLambda::operator())>;
		using t_params = typename t_function::t_params;
		static const bool is_lambda = true;
	};

	template<typename T>
	struct type_resolver<LambdaResolver<0>, T, void>
	{
		static const bool is_lambda = false;
	};


}
