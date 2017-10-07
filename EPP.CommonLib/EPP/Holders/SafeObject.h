#pragma once

#pragma managed(push, off)
#include <memory>
#include <assert.h>
#include <EPP\Preprocessor\PP_DebugOnly.h>

namespace EPP
{
	struct ISafeObject;
	typedef std::shared_ptr<ISafeObject> TSafePtr;
	typedef std::weak_ptr<ISafeObject> TWeakPtr;

	struct ISafeObject
	{
	protected:
		inline ISafeObject() PP_DEBUG_ONLY(:m_typeName("Not created using ISafeObject::Create method")) {}
		inline ISafeObject(const ISafeObject &) {}
	public:
		virtual ~ISafeObject() {}

	protected:
		template <typename T>
		struct Creator : public T
		{
		public:
			template <typename ...TParams>
			inline Creator(TParams&&... params) : T(std::forward<TParams>(params)...) {}
		private:
			virtual void This_Class_Must_Be_Created_Using_SafeObject_Template() override {}
#pragma warning(suppress: 4250) // "inherits via dominance"
		};

	public:
		template <typename T, typename ...TParams>
		inline static T & Create(TSafePtr & out_ptr, TParams&&... params)
		{
			static_assert(std::is_base_of<ISafeObject, T>::value, "The class must derive from ISafeObject");
			T * pNew = new Creator<T>(std::forward<TParams>(params)...);
			if (pNew->m_weakPtr.use_count() == 0)
			{
				out_ptr = TSafePtr(pNew);
				pNew->m_weakPtr = out_ptr;
			}
			else
			{
				out_ptr = TSafePtr(pNew->m_weakPtr);
			}
			PP_DEBUG_ONLY(pNew->m_typeName = typeid(T).name();)
				return *pNew;
		}
		template <typename T>
		inline static T * CreateFromType(unsigned int /*type*/, TSafePtr & ptr)
		{
			return &Create<T>(ptr);
		}
	private:
		//This function declared to prevent create classes inherited from ISafeObject directly.
		//Such classes must be created by SafeObject<T> template!
		virtual void This_Class_Must_Be_Created_Using_SafeObject_Template() = 0;
	protected:
		const ISafeObject & operator= (const ISafeObject & /*s*/) {
			return *this;
		}
	public:
		PP_DEBUG_ONLY(const char * m_typeName);
		mutable TWeakPtr m_weakPtr;
	};

	struct ConstructT
	{
		inline ConstructT(int) {}
	};
#define CONSTRUCT ConstructT(0)

	template<typename T>
	struct SafeObject
	{
		struct TCheckIsSafeObject
		{
			virtual void F()
			{
				static_assert(std::is_base_of<ISafeObject, T>::value, "The class is not derived from ISafeObject");
			}
		};

		typedef decltype(((TCheckIsSafeObject *)(0))->F()) TCheck2;

		inline SafeObject()
		{
			ptr = NULL;
		}
		inline SafeObject(std::nullptr_t)
		{
			ptr = NULL;
		}
		typedef typename std::remove_const<T>::type type;
		typedef typename std::add_const<type>::type const_type;
		const static bool isConstT = std::is_const<T>::value;
		template<typename O>
		inline SafeObject(O && val, decltype(((SafeObject<T>*)0)->OperatorEqual(std::forward<O>(val)))* = 0)
		{
			OperatorEqual(std::forward<O>(val));
		}
		template<typename ...TParams>
		inline SafeObject(ConstructT, TParams&&... params)
		{
			CreateInstance<T>(static_cast<TParams&&>(params)...);
		}
		template <typename NewInstanceType = T, typename ...TParams>
		inline NewInstanceType & CreateInstance(TParams&&... params)
		{
			static_assert(std::is_base_of<T, NewInstanceType>::value, "The type is not same or base of T");
			NewInstanceType & new_inst = ISafeObject::Create<NewInstanceType>(safePtr, std::forward<TParams>(params)...);
			ptr = &new_inst;
			return new_inst;
		}
		inline T * CreateInstanceFromType(unsigned int type)
		{
			typedef std::remove_const<T>::type TNonConst;
			ptr = TNonConst::CreateFromType<TNonConst>(type, safePtr);
			return ptr;
		}
		inline void ReleaseInstance()
		{
			safePtr = TSafePtr();
			ptr = NULL;
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
		inline auto operator=(O && val) -> typename std::enable_if<std::is_same<decltype(OperatorEqual(std::forward<O>(val))), void>::value, SafeObject<T>>::type &
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
		inline auto OperatorEqual(O & val) -> typename std::enable_if<std::is_base_of<T, O>::value, void>::type
		{
			OperatorEqual(&val);
		}
		template <typename O>
		inline auto OperatorEqual(O * val)-> typename std::enable_if<std::is_base_of<T, O>::value, void>::type
		{
			static_assert(std::is_base_of<T, O>::value, "Type O is not equal or base of T");

			if (!val) {
				ReleaseInstance();
				return;
			}
			if (val->m_weakPtr.use_count() == 0)
			{
				//this case may be only if assigning from constructor of ISafeObject type to self, before safe ptr on this is created
				safePtr = TSafePtr((ISafeObject *)val);
				val->m_weakPtr = safePtr;
			}
			else {
				safePtr = TSafePtr(val->m_weakPtr);
			}
			ptr = val;
		}
		template <typename O>
		inline auto OperatorEqual(const SafeObject<O> & val) -> typename std::enable_if<std::is_base_of<T, O>::value, void>::type
		{
			OperatorEqual(val.ptr);
		}
		template <typename O>
		inline auto OperatorEqual(SafeObject<O> & val) -> typename std::enable_if<std::is_base_of<T, O>::value, void>::type
		{
			OperatorEqual(val.ptr);
		}
		template <typename O>
		inline auto OperatorEqual(SafeObject<O> && val) -> typename std::enable_if<std::is_base_of<T, O>::value, void>::type
		{
			ReleaseInstance();
			ptr = val.ptr;
			safePtr.swap(val.safePtr);
		}

		inline operator const T & () const
		{
			assert(ptr);
			return *ptr;
		}
		inline operator T & ()
		{
			assert(ptr);
			return *ptr;
		}
		template<typename O>
		inline operator O * ()
		{
			static_assert(std::is_base_of<O, T>::value, "Type T is not equal or base of type O");
			return ptr;
		}
		template<typename O>
		inline operator const O*() const
		{
			static_assert(std::is_base_of<O, T>::value, "Type T is not equal or base of type O");
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
			static_assert(std::is_base_of<T, O>::value || std::is_base_of<O, T>::value, "Type O is not base of type T and T is not base of type O");
			typedef decltype(static_cast<O &>(*ptr)) check_T;
			return static_cast<O &>(*ptr);
		}
		template<typename O>
		inline O & StaticCast()
		{
			static_assert(std::is_base_of<T, O>::value || std::is_base_of<O, T>::value, "Type O is not base of type T and T is not base of type O");
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
		TSafePtr safePtr;
		T * ptr;
	};
}

#pragma managed(pop)