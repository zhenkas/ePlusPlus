#pragma once

namespace EPP::Templates
{
	template<typename...T>
	using make_void = void;

	template<typename TResolver, typename T, typename TCheck>
	struct type_resolver;

	template<template<int> typename TResolver, typename T, typename TCheck>
	struct type_resolver<TResolver<-1>, T, TCheck> {};

	template<template<int> typename TResolver, int priority, typename T>
	struct type_resolver<TResolver<priority>, T, void> : type_resolver<TResolver<priority-1>, T, void> {};

	template<typename TResolver, typename T>
	using resolve_type = type_resolver<TResolver, T, void>;
}