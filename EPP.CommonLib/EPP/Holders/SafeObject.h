#pragma once

#pragma managed(push, off)
#include <memory>
#include <assert.h>
#include <EPP\Preprocessor\PP_DebugOnly.h>
#include <EPP\Preprocessor\PP_CONSTRUCT.h>
#include <EPP\Templates\t_safeobject_resolver.h>
#include <EPP\Locks\ExclusiveSpinLockRef.h>

namespace EPP::Holders
{

	template <typename T>
	struct SOCreator : public T
	{
	public:
		template <typename ...TParams>
		inline SOCreator(TParams&&... params) : T(std::forward<TParams>(params)...) {}
	private:
		virtual void This_Class_Must_Be_Created_Using_SafeObject_Template() override {}
#pragma warning(suppress: 4250) // "inherits via dominance"
	};


	struct ISafeObject
	{
	protected:
		inline ISafeObject() : m_safeobject_state(INC_VALUE) {}
		inline ISafeObject(const ISafeObject &) {}
		inline ISafeObject(ISafeObject &&) {}
	public:
		virtual ~ISafeObject() {}
	protected:
		const ISafeObject & operator= (const ISafeObject & /*s*/) { return *this; }
		const ISafeObject & operator= (const ISafeObject && /*s*/) { return *this; }
	private:
		virtual void This_Class_Must_Be_Created_Using_SafeObject_Template() = 0;
	public:
		PP_DEBUG_ONLY(const char * m_typeName);

		static constexpr long INC_VALUE = 0x2;
		static constexpr long DESTROYED_VALUE = 0x80000001;
		mutable volatile long m_safeobject_state; //ref count and lock
	};

	namespace Internal
	{
		struct ConstructT
		{
			inline ConstructT(int) {}
		};
	}
	using ::EPP::Locks::ExclusiveSpinLockRef;
	template<typename T>
	struct SafeObject
	{
		inline SafeObject()
		{
			static_assert(std::is_base_of_v<ISafeObject, T>, "The class is not derived from ISafeObject");
			ptr = NULL;
		}
		inline ~SafeObject()
		{
			ReleaseInstance();
		}
		inline SafeObject(std::nullptr_t)
		{
			static_assert(std::is_base_of_v<ISafeObject, T>, "The class is not derived from ISafeObject");
			ptr = NULL;
		}
		typedef typename std::remove_const<T>::type type;
		typedef typename std::add_const<type>::type const_type;
		const static bool isConstT = std::is_const<T>::value;
		template<typename O>
		inline SafeObject(O && val, decltype(((SafeObject<T>*)0)->OperatorEqual(std::forward<O>(val)))* = 0)
		{
			static_assert(std::is_base_of_v<ISafeObject, T>, "The class is not derived from ISafeObject");
			ptr = NULL;
			OperatorEqual(std::forward<O>(val));
		}
		template<typename ...TParams>
		inline SafeObject(Internal::ConstructT, TParams&&... params)
		{
			static_assert(std::is_base_of_v<ISafeObject, T>, "The class is not derived from ISafeObject");
			ptr = new SOCreator<T>(std::forward<TParams>(params)...);
		}
		template <typename NewInstanceType = T, typename ...TParams>
		inline NewInstanceType & Construct(TParams&&... params)
		{
			static_assert(std::is_base_of<T, NewInstanceType>::value, "The type is not same or base of T");

			T * pNew = new SOCreator<T>(std::forward<TParams>(params)...);
			ptr = pNew;
			return new_inst;
		}
		inline void ReleaseInstance()
		{
			if (ptr)
			{
				auto & state = ptr->m_safeobject_state;
				ExclusiveSpinLockRef::ExclusiveLock(state);
				long newstate = state;
				newstate -= ISafeObject::INC_VALUE;
				if (newstate < ISafeObject::INC_VALUE)
				{
					newstate = ISafeObject::DESTROYED_VALUE;
				}
				state = newstate;
				ExclusiveSpinLockRef::ExclusiveRelease(state);
				if (newstate < 0)
				{
					delete ptr;
				}
				ptr = NULL;
			}
		}
		inline unsigned int GetInstanceType() const
		{
			typedef std::remove_const<T>::type  TNonConst;
			return TNonConst::GetType<TNonConst>(*ptr);
		}
		inline T * operator->() { assert(ptr); return ptr; }
		inline T & operator*() { assert(ptr); return *ptr; }
		inline T * operator->() const { assert(ptr); return ptr; }
		inline T & operator*() const { assert(ptr); return *ptr; }
		inline explicit operator bool() const { return ptr != NULL; }
		template <typename O>
		inline auto operator=(O && val) -> std::enable_if_t<std::is_same_v<decltype(OperatorEqual(std::forward<O>(val))), void>, SafeObject<T>> &
		{
			OperatorEqual(static_cast<O&&>(val));
			return *this;
		}
		inline SafeObject<T> & operator=(std::nullptr_t)
		{
			ReleaseInstance();
			return *this;
		}
		inline bool operator==(const SafeObject<T> & val) const
		{
			return ptr == val.ptr;
		}
		inline bool operator !=(const std::nullptr_t & val) const
		{
			return ptr != nullptr;
		}

		template<typename O>
		inline auto OperatorEqual(O & val) -> std::enable_if_t<std::is_base_of_v<T, O>, void>
		{
			OperatorEqual(&val);
		}
		template <typename O>
		inline auto OperatorEqual(O * val)-> std::enable_if_t<std::is_base_of_v<T, O>, void>
		{
			static_assert(std::is_base_of_v<T, O>, "Type O is not equal or base of T");

			if (!val)
			{

				ReleaseInstance();
				return;
			}
			auto & state = val->m_safeobject_state;
			if (state < 0)
			{
				ReleaseInstance();
				return;
			}
			if (val == ptr)
				return;

			ReleaseInstance();

			ExclusiveSpinLockRef::ExclusiveLock(state);
			if (state < 0)
			{
				ExclusiveSpinLockRef::ExclusiveRelease(state);
				return;
			}
			state += ISafeObject::INC_VALUE;
			ExclusiveSpinLockRef::ExclusiveRelease(state);
			ptr = val;
		}
		template <typename O>
		inline auto OperatorEqual(const SafeObject<O> & val) -> std::enable_if_t<std::is_base_of_v<T, O>, void>
		{
			OperatorEqual(val.ptr);
		}
		template <typename O>
		inline auto OperatorEqual(SafeObject<O> & val) -> std::enable_if_t<std::is_base_of_v<T, O>, void>
		{
			OperatorEqual(val.ptr);
		}
		template <typename O>
		inline auto OperatorEqual(SafeObject<O> && val) -> std::enable_if_t<std::is_base_of_v<T, O>, void>
		{
			auto tmp = val.ptr;
			val.ptr = (O*)ptr;
			ptr = tmp;
		}
		template<typename O, std::enable_if_t<std::is_base_of_v<O, T>, bool> = true>
		inline operator const O & () const
		{
			assert(ptr);
			return static_cast<const O &>(*ptr);
		}
		template<typename O, std::enable_if_t<std::is_base_of_v<O, T>, bool> = true>
		inline operator O & ()
		{
			assert(ptr);
			return static_cast<O &>(*ptr);
		}
		template<typename O, std::enable_if_t<(!std::is_base_of_v<ISafeObject, O> && !Templates::is_safeobject_v<O>) || (std::is_base_of_v<ISafeObject, O> && !std::is_base_of_v<O, T>), bool> = true>
		inline operator const O & () const
		{
			static_assert(false, "Invalid cast to const reference type from SafeObject<T>");
		}
		template<typename O, std::enable_if_t<!std::is_base_of_v<ISafeObject, O> && !Templates::is_safeobject_v<O> || (std::is_base_of_v<ISafeObject, O> && !std::is_base_of_v<O, T>), bool> = true>
		inline operator O & () const
		{
			static_assert(false, "Invalid cast to reference type from SafeObject<T>");
		}
		template<typename O, std::enable_if_t<Templates::is_safeobject_v<O> && !std::is_same_v<Templates::get_isafeobject_t<O>, T> && !std::is_const_v<O>, bool> = true>
		inline operator O & () const
		{
			static_assert(false, "Invalid cast to reference type from SafeObject<T>");
		}

		template<typename O>
		inline operator O * ()
		{
			static_assert(std::is_base_of_v<O, T>, "Type T is not equal or base of type O");
			return ptr;
		}
		template<typename O>
		inline operator const O*() const
		{
			static_assert(std::is_base_of_v<O, T>, "Type T is not equal or base of type O");
			return ptr;
		}
		template<typename O>
		inline const O * DynamicCast() const
		{
			O * ptr2 = dynamic_cast<O *>(ptr);
			return ptr2;
		}
		template<typename O>
		inline O * DynamicCast()
		{
			typedef decltype(dynamic_cast<O *>(ptr)) check_T;
			O * ptr2 = dynamic_cast<O *>(ptr);
			return ptr2;
		}
		template<typename O>
		inline const O & StaticCast() const
		{
			static_assert(std::is_base_of_v<T, O> || std::is_base_of_v<O, T>, "Type O is not base of type T and T is not base of type O");
			typedef decltype(static_cast<O &>(*ptr)) check_T;
			return static_cast<O &>(*ptr);
		}
		template<typename O>
		inline O & StaticCast()
		{
			static_assert(std::is_base_of_v<T, O> || std::is_base_of_v<O, T>, "Type O is not base of type T and T is not base of type O");
			typedef decltype(static_cast<O &>(*ptr)) check_T;
			return static_cast<O &>(*ptr);
		}

		inline const T * GetPtr() const
		{
			return ptr;
		}
		inline T * GetPtr()
		{
			return ptr;
		}
		template<typename TIndex>
		inline auto operator [] (TIndex && i) const -> decltype(ptr->operator[](std::forward<TIndex>(i)))
		{
			return ptr->operator[](static_cast<TIndex&&>(i));
		}
		template<typename TIndex>
		inline auto operator [] (TIndex && i) -> decltype(ptr->operator[](std::forward<TIndex>(i)))
		{
			return ptr->operator[](static_cast<TIndex&&>(i));
		}
	public:
		T * ptr;
	};
}

#pragma managed(pop)