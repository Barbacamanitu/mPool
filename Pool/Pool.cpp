#include "stdafx.h"
#include <vector>
#include <memory>
#include <iostream>
#include "Poolable.h"
#include <ctime>
class Pool
{
private:
	void * mPool;
	std::vector<int> mAvailableIndices;
	size_t mObjectSize;
	int mCount;

public:
	void Allocate(int count, size_t size)
	{
		mObjectSize = size;
		mCount = count;
		mPool = malloc(count * size);
		mAvailableIndices.reserve(count);
		for (int i = 0; i < count; i++)
		{
			mAvailableIndices.push_back(i);
		}
	}

	struct PoolReturner
	{
		PoolReturner(){};
		PoolReturner(Pool* p) :mPool(p){};
		void operator()(Poolable * ptr)
		{
			if (mPool != nullptr)
				mPool->Return(ptr);
		}
		Pool* mPool;
	};

	char * GetOffset(int index)
	{
		return (char *)((intptr_t)mPool) + (mObjectSize * index);
	}


	template<class T>
	std::unique_ptr<T,PoolReturner> Get()
	{
		if (mAvailableIndices.empty())
			return nullptr;

		int index = mAvailableIndices.back();
		mAvailableIndices.pop_back();

		T* newObj = new (GetOffset(index)) T;

		std::unique_ptr<T, PoolReturner> newPtr(newObj, PoolReturner(this));
		return newPtr;
	}

	void Return(Poolable * ptr)
	{
		ptr->~Poolable();
		intptr_t offset = (intptr_t)ptr - (intptr_t)mPool;
		if (offset < 0 || offset >= mCount)
			return;


		mAvailableIndices.push_back(offset);
	}
};

class MyObject : public Poolable
{
public:
	int a;
	MyObject()
	{
		a = 100;
	}
	~MyObject()
	{
		std::cout << "Deleting MyObject @ " << this << "\n";
	}
};

int _tmain(int argc, _TCHAR* argv[])
{
	srand(time(0));
	Pool pool;
	pool.Allocate(10, sizeof(MyObject));
	typedef std::unique_ptr<MyObject, Pool::PoolReturner> ObjectPtr;
	std::vector<ObjectPtr> objects;

	int nulled = 0;
	int created = 0;
	int deleted = 0;

	for (int i = 0; i < 15; i++)
	{
		ObjectPtr obj = pool.Get<MyObject>();
		if (obj != nullptr)
		{
			created++;
			objects.push_back(std::move(obj));
		}
		else
			nulled++;		
	}


	//DELETE POOL BEFORE OBJECTS DELETED

	//All objects are in vector. Loop through and delete some.
	int deleteAmount = rand() & objects.size();
	for (int i = 0; i < deleteAmount; i++)
	{
		objects.pop_back();
		deleted++;
		//Removes reference which automatically invokes PoolReturner functor.
	}

	std::cout << "Objects Created: " << created << "\n";
	std::cout << "Objects Deleted: " << deleted << "\n";
	std::cout << "Objects Null: " << nulled << "\n";

	while (true)
	{

	}
	return 0;
}

