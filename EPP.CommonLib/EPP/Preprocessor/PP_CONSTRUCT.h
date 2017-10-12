#pragma once

/*
	CONSTRUCT - used for easy construction of SafeObject

	Usage: 
		SafeObject<MyT> var = CONSTRUCT // will construct the new object of type MyT

	Note: 
		It will not work for abstract types. Use one of the statements below to construct into abstract type:

			1.	SafeObject<AbstractT> var;
				var.Construct<ImplT>(params...); //params forwarded to ImplT constructor

			2.	SafeObject<AbstractT> var = SafeObject<ImplT>(CONSTRUCT, params...); //params forwarded to ImplT constructor
		
*/

#define CONSTRUCT EPP::Holders::Internal::ConstructT(0)