namespace EPP::Memory
{
	template<typename T>
	class DefaultMemoryAllocator
	{
	public:
		static inline void *Allocate(size_t size)
		{
			return ::operator new(size, ::std::nothrow);
		}
		static inline void Deallocate(void *pointer, size_t size)
		{
			::operator delete(pointer);
		}
	};

	template<size_t _itemSizeP2, class TMemoryAllocator = DefaultMemoryAllocator>
	class ObjectPool
	{
		static const size_t _itemSize = 1 << _itemSizeP2;
	private:
		struct SubPool
		{
			void *_memory;
			size_t _capacity;
			SubPool *_nextSubPool;

			SubPool(size_t capacity)
			{
				if (capacity < 1)
					throw std::invalid_argument("capacity must be at least 1.");

				_memory = TMemoryAllocator::Allocate(capacity << _itemSizeP2);
				if (_memory == NULL)
					throw std::bad_alloc();

				_capacity = capacity;
				_nextNode = NULL;
			}
			~SubPool()
			{
				TMemoryAllocator::Deallocate(_memory, _capacity << _itemSizeP2);
			}
		};

		void *_nodeMemory;
		T *_firstDeleted;
		size_t _countInNode;
		size_t _nodeCapacity;
		_Node _firstNode;
		_Node *_lastNode;
		size_t _maxBlockLength;

		void _AllocateNewNode()
		{
			size_t size = _countInNode;
			if (size >= _maxBlockLength)
				size = _maxBlockLength;
			else
			{
				size *= 2;

				if (size < _countInNode)
					throw std::overflow_error("size became too big.");

				if (size >= _maxBlockLength)
					size = _maxBlockLength;
			}

			_Node *newNode = new _Node(size);
			_lastNode->_nextNode = newNode;
			_lastNode = newNode;
			_nodeMemory = newNode->_memory;
			_countInNode = 0;
			_nodeCapacity = size;
		}

	public:
		explicit ObjectPool(size_t initialCapacity = 32, size_t maxBlockLength = 1000000) :
			_firstDeleted(NULL),
			_countInNode(0),
			_nodeCapacity(initialCapacity),
			_firstNode(initialCapacity),
			_maxBlockLength(maxBlockLength)
		{
			if (maxBlockLength < 1)
				throw std::invalid_argument("maxBlockLength must be at least 1.");

			_nodeMemory = _firstNode._memory;
			_lastNode = &_firstNode;
		}
		~ObjectPool()
		{
			_Node *node = _firstNode._nextNode;
			while (node)
			{
				_Node *nextNode = node->_nextNode;
				delete node;
				node = nextNode;
			}
		}

		void *New()
		{
			if (_firstDeleted)
			{
				void *result = _firstDeleted;
				_firstDeleted = *((void **)_firstDeleted);
				new(result) T();
				return result;
			}

			if (_countInNode >= _nodeCapacity)
				_AllocateNewNode();

			char *address = (char *)_nodeMemory;
			address += _countInNode * _itemSize;
			_countInNode++;
			return result;
		}

		// This method is useful if you want to call a non-default constructor.
		// It should be used like this:
		// new (pool.GetNextWithoutInitializing()) ObjectType(... parameters ...);
		T *GetNextWithoutInitializing()
		{
			if (_firstDeleted)
			{
				T *result = (T *)_firstDeleted;
				_firstDeleted = *((T **)_firstDeleted);
				return result;
			}

			if (_countInNode >= _nodeCapacity)
				_AllocateNewNode();

			char *address = (char *)_nodeMemory;
			address += _countInNode * _itemSize;
			_countInNode++;
			return (T *)address;
		}
		void Delete(T *content)
		{
			content->~T();

			*((T **)content) = _firstDeleted;
			_firstDeleted = content;
		}
		void DeleteWithoutDestroying(T *content)
		{
			*((T **)content) = _firstDeleted;
			_firstDeleted = content;
		}
	};
}
