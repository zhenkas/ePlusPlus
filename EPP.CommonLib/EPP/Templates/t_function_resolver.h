#pragma once
#include "t_type_resolver.h"

namespace EPP::Templates
{

	template<int priority>
	struct FunctionResolver;

	template<typename T>
	using resolve_function = type_resolver<FunctionResolver<1>, T>;

	template<typename TRet, typename ...TParams>
	struct function_params {};

	template<typename TClass, typename TRet, typename...TParams>
	struct type_resolver<FunctionResolver<1>, TRet(__thiscall TClass::*)(TParams...), void>
	{
		static const bool is_thiscall = true;
		static const bool is_static = false;
		static const bool is_const = false;
		using t_params = function_params<TRet, TParams...>;
	};
	template<typename TClass, typename TRet, typename...TParams>
	struct type_resolver<FunctionResolver<1>, TRet(__thiscall TClass::*)(TParams...) const, void>
	{
		static const bool is_thiscall = true;
		static const bool is_static = false;
		static const bool is_const = true;
		using t_params = function_params<TRet, TParams...>;
	};
	template<typename TRet, typename...TParams>
	struct type_resolver<FunctionResolver<1>, TRet(TParams...), void>
	{
		static const bool is_thiscall = false;
		static const bool is_static = true;
		static const bool is_const = false;
		using t_params = function_params<TRet, TParams...>;
	};
}
