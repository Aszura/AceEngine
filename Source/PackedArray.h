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
			: mCount(0)
			, mElementCount(elementCount)
			, mElements(new T[elementCount])
			, mIndices(new Index[elementCount])
		{
			//Check power of two
			assert((mElementCount & (mElementCount - 1)) == 0);

			//Fill indices array with starting values
			for (unsigned int i = 0; i < mElementCount; ++i)
			{
				mIndices[i].innerIndex = INVALID_VALUE;
				mIndices[i].outerIndex = i; //set start mesh ids
				mIndices[i].freeNext = i + 1; //set index for next element in freelist
			}

			//Set "freeptr" to first element
			mNextFreeIndex = 0;

			//Link sentinels to each other
			mHead.next = &mTail;
			mTail.prev = &mHead;
		}

		~PackedArray()
		{
			delete[] mElements;
			delete[] mIndices;
		}

		ElementId add()
		{
			//Check if mesh array full
			if (mCount == mElementCount)
			{
				printf("ComponentEntityWorld: Can't add any more components. Already full.");
				return 0;
			}

			//Get next index struct from freelist
			Index& newIndex = mIndices[mNextFreeIndex];

			//Set unique id
			newIndex.outerIndex += mElementCount;

			//Set to last/new element in packed array
			newIndex.innerIndex = mCount;
			++mCount;

			//Set next free element in freelist
			mNextFreeIndex = newIndex.freeNext;

			//Place index struct in linked list
			newIndex.prev = &mHead;
			newIndex.next = mHead.next;
			mHead.next = &newIndex;

			return newIndex.outerIndex;
		}


		void remove(ElementId id)
		{
			//Use mask instead of e.g. modulo to get original id for indices array
			Index& removeIndex = mIndices[id & (mElementCount - 1)];

			//Check if id is valid
			if (removeIndex.innerIndex == INVALID_VALUE)
			{
				return;
			}

			--mCount;

			//Get index struct for last mesh in mesh array
			Index& lastIndex = *mHead.next;

			//swap mesh with last mesh in array to fill hole
			std::swap(mElements[removeIndex.innerIndex], mElements[lastIndex.innerIndex]);

			//Remove lastIndex from last position
			mHead.next = lastIndex.next;
			lastIndex.next->prev = &mHead;

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
			removeIndex.freeNext = mNextFreeIndex;
			mNextFreeIndex = id & (mElementCount - 1);
		}


		T* lookup(ElementId id)
		{
			//Use mask instead of e.g. modulo to get original id for indices array
			Index& lookupIndex = mIndices[id & (mElementCount - 1)];

			//Check if id is valid
			if (lookupIndex.innerIndex == INVALID_VALUE)
			{
				return nullptr;
			}

			return &mElements[lookupIndex.innerIndex];
		}

		void iterateFunction(std::function<void(T*)> itFunction)
		{
			for (unsigned int i = 0; i < mCount; ++i)
			{
				itFunction(&mElements[i]);
			}
		}

		inline T* getElementArray()
		{
			return mElements;
		}

		inline const T* getElementArray() const
		{
			return mElements;
		}

		inline unsigned int length() const
		{
			return mCount;
		}

		Iterator begin()
		{
			return Iterator(this, 0);
		}

		Iterator end()
		{
			return Iterator(this, mCount);
		}

		ConstIterator begin() const
		{
			return ConstIterator(this, 0);
		}

		ConstIterator end() const
		{
			return ConstIterator(this, mCount);
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

		size_t			mElementCount;
		Index*			mIndices;

		//index for next free index -> freelist
		unsigned int	mNextFreeIndex;

		//Sentinels for backward linked list to get last meshes for swapping
		Index			mHead;
		Index			mTail;

		T*				mElements;
		unsigned int	mCount;
	};
}