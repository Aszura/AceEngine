#pragma once

#include <assert.h>
#include <stdio.h>
#include <algorithm> //For std::swap
#include <functional>

namespace utility
{
	typedef unsigned int ElementId;

#define INVALID_VALUE -1

	// The ComponentEntityWorld only holds components, nothing more.
	// The ComponentEntityWorld implementation makes sure that after *each* operation its internal data structure has no holes,
	// And contains component instances which are stored contiguously in memory.
	template<typename T>
	class PackedArray
	{
	public:
		class Iterator
		{
		public:
			Iterator(PackedArray<T>* array, int index)
				: mIndex(index)
				, mArray(array)
			{ }

			bool operator!= (const Iterator& other) const
			{
				return mIndex != other.mIndex;
			}

			T& operator* ()
			{
				return mArray->getElementArray()[mIndex];
			}

			Iterator& operator++ ()
			{
				++mIndex;
				return *this;
			}

		private:
			int mIndex;
			PackedArray<T>* mArray;
		};

		class ConstIterator
		{
		public:
			ConstIterator(const PackedArray<T>* array, int index)
				: mIndex(index)
				, mArray(array)
			{ }

			bool operator!= (const ConstIterator& other) const
			{
				return mIndex != other.mIndex;
			}

			T& operator* () const
			{
				return mArray->getElementArray()[mIndex];
			}

			const ConstIterator& operator++ ()
			{
				++mIndex;
				return *this;
			}

		private:
			int mIndex;
			const PackedArray<T>* mArray;
		};


		PackedArray(size_t elementCount)
			: m_count(0)
			, m_elementCount(elementCount)
			, m_elements(new T[elementCount])
			, m_indices(new Index[elementCount])
		{
			//Check power of two
			assert((m_elementCount & (m_elementCount - 1)) == 0);

			//Fill indices array with starting values
			for (unsigned int i = 0; i < m_elementCount; ++i)
			{
				m_indices[i].innerIndex = INVALID_VALUE;
				m_indices[i].outerIndex = i; //set start mesh ids
				m_indices[i].freeNext = i + 1; //set index for next element in freelist
			}

			//Set "freeptr" to first element
			m_nextFreeIndex = 0;

			//Link sentinels to each other
			m_head.next = &m_tail;
			m_tail.prev = &m_head;
		}

		~PackedArray()
		{
			delete[] m_elements;
			delete[] m_indices;
		}

		ElementId add()
		{
			//Check if mesh array full
			if (m_count == m_elementCount)
			{
				printf("ComponentEntityWorld: Can't add any more components. Already full.");
				return 0;
			}

			//Get next index struct from freelist
			Index& newIndex = m_indices[m_nextFreeIndex];

			//Set unique id
			newIndex.outerIndex += m_elementCount;

			//Set to last/new element in packed array
			newIndex.innerIndex = m_count;
			++m_count;

			//Set next free element in freelist
			m_nextFreeIndex = newIndex.freeNext;

			//Place index struct in linked list
			newIndex.prev = &m_head;
			newIndex.next = m_head.next;
			m_head.next = &newIndex;

			return newIndex.outerIndex;
		}


		void remove(ElementId id)
		{
			//Use mask instead of e.g. modulo to get original id for indices array
			Index& removeIndex = m_indices[id & (m_elementCount - 1)];

			//Check if id is valid
			if (removeIndex.innerIndex == INVALID_VALUE)
			{
				return;
			}

			--m_count;

			//Get index struct for last mesh in mesh array
			Index& lastIndex = *m_head.next;

			//swap mesh with last mesh in array to fill hole
			std::swap(m_elements[removeIndex.innerIndex], m_elements[lastIndex.innerIndex]);

			//Remove lastIndex from last position
			m_head.next = lastIndex.next;
			lastIndex.next->prev = &m_head;

			//Replace removeIndex with lastIndex in linked list
			lastIndex.next = removeIndex.next;
			lastIndex.prev = removeIndex.prev;

			//Place lastIndex at position of removeIndex
			removeIndex.next->prev = &lastIndex;
			removeIndex.prev->next = &lastIndex;

			//Point last element to new index
			lastIndex.innerIndex = removeIndex.innerIndex;

			//Delete inner index of old element
			removeIndex.innerIndex = INVALID_VALUE;

			//Place in freelist
			removeIndex.freeNext = m_nextFreeIndex;
			m_nextFreeIndex = id & (m_elementCount - 1);
		}


		T* lookup(ElementId id)
		{
			//Use mask instead of e.g. modulo to get original id for indices array
			Index& lookupIndex = m_indices[id & (m_elementCount - 1)];

			//Check if id is valid
			if (lookupIndex.innerIndex == INVALID_VALUE)
			{
				return nullptr;
			}

			return &m_elements[lookupIndex.innerIndex];
		}

		void iterateFunction(std::function<void(T*)> itFunction)
		{
			for (unsigned int i = 0; i < m_count; ++i)
			{
				itFunction(&m_elements[i]);
			}
		}

		inline T* getElementArray()
		{
			return m_elements;
		}

		inline const T* getElementArray() const
		{
			return m_elements;
		}

		inline unsigned int length() const
		{
			return m_count;
		}

		Iterator begin()
		{
			return Iterator(this, 0);
		}

		Iterator end()
		{
			return Iterator(this, m_count);
		}

		ConstIterator begin() const
		{
			return ConstIterator(this, 0);
		}

		ConstIterator end() const
		{
			return ConstIterator(this, m_count);
		}
	private:
		//Index struct for decoupling ids and mesh array
		struct Index
		{
			ElementId		outerIndex;

			//Index for mesh array
			int				innerIndex;

			//Next element in freelist for unused indices
			unsigned int	freeNext;

			//Members for linked list
			Index*			prev;
			Index*			next;
		};

		size_t			m_elementCount;
		Index*			m_indices;

		//index for next free index -> freelist
		unsigned int	m_nextFreeIndex;

		//Sentinels for backward linked list to get last meshes for swapping
		Index			m_head;
		Index			m_tail;

		T*				m_elements;
		unsigned int	m_count;
	};
}