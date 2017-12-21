#pragma once

#include <EPP\Holders\SafeObject.h>
#include <EPP\Templates\t_lambda_resolver.h>

namespace EPP::Holders
{
	using namespace EPP::Templates;


	// Usage sample: void Function( ISafeLambdaT<void(void)> & lambda) { //expecting not nullable lambda }
	template<typename TFunction>
	struct ISafeLambdaT;

	// Usage sample: ISafeLambda<void(void)> lambda = ConstructSafeLambda([](){});
	template<typename TFunc>
	using ISafeLambda = SafeObject<ISafeLambdaT<TFunc>>;

	// Usage - internal
	template<typename TLambda, typename TFunc>
	struct SafeLambdaT;

	// ISafeLambdaT - define for function definition only (no lambda type)
	template<typename TRet, typename ...TParams>
	struct ISafeLambdaT<TRet(TParams...)> : ISafeObject
	{
		virtual TRet CallFunction(TParams...) = 0;
	};

	// SafeLambdaT - define for function definition with lambda type (used internally)
	template<typename TLambda, typename TRet, typename ...TParams>
	struct SafeLambdaT<TLambda, TRet(TParams...)> : ISafeLambdaT<TRet(TParams...)>
	{
		SafeLambdaT(TLambda && lambda) : m_lamda(std::forward<TLambda>(lambda)) {}
		virtual TRet CallFunction(TParams... params) override
		{
			return m_lamda(std::forward<TParams>(params)...);
		}
		TLambda m_lamda;
	};


	// Automatically creates the ISafeLambda<TRet,(TParams)> from TLambda
	template<typename TLambda>
	inline auto CreateSafeLambda(TLambda && lambda) 
	{
		using t_lambda = resolve_lambda<TLambda>;
		static_assert(t_lambda::is_lambda, "CreateSafeLambda expecting Lambda class that have one operator()");
		using t_params = typename t_lambda::t_params;
		return InternalCreateSafeLambda(std::forward<TLambda>(lambda), t_params());
	}

	// Explicitly creates the ISafeLambda<TRet,(TParams)> from TLambda
	template<typename TLambda, typename TRet, typename...TParams>
	inline ISafeLambda<TRet(TParams...)> InternalCreateSafeLambda(TLambda && lambda, function_params<TRet, TParams...>)
	{
		using TSafeLambda = SafeObject<SafeLambdaT<TLambda, TRet(TParams...)>>;
		return TSafeLambda(CONSTRUCT, std::forward<TLambda>(lambda));
	}


	/// AutoSafeLambda - Should be use only as parameter to function, which eliminates the need in template functions to receive any lambda.
	///                  Take the m_safeLambda for storing&moving the lambda
	///

	template<typename TFunc>
	struct AutoSafeLambda;

	template<typename TRet, typename ...TParams>
	struct AutoSafeLambda<TRet(TParams...)>
	{		
		template<typename TLambda>
		static constexpr bool is_compatible = std::is_same<typename resolve_lambda<TLambda>::t_params, function_params<TRet, TParams...>>::value;
		template<typename TLambda, typename TSafeLambda = SafeObject<SafeLambdaT<TLambda, TRet(TParams...)>>>
		AutoSafeLambda(TLambda && lambda, std::enable_if_t<is_compatible<TLambda>, void> * = nullptr)
			: m_safeLambda(TSafeLambda(CONSTRUCT, std::forward<TLambda>(lambda)))
		{
		}
		using t_safelambda = ISafeLambda<TRet(TParams...)>;
		ISafeLambda<TRet(TParams...)> m_safeLambda;
	};
}