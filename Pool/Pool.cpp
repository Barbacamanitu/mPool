// Pool.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <vector>
#include <memory>
#include <iostream>

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

	char * GetOffset(int index)
	{
		return (char *)((intptr_t)mPool) + (mObjectSize * index);
	}

	template<class T>
	T* Get()
	{
		if (mAvailableIndices.empty())
			return nullptr;

		int index = mAvailableIndices.back();
		mAvailableIndices.pop_back();

		T* newObj = new (GetOffset(index)) T;
		return newObj;
	}

	void Return(void * ptr)
	{
		intptr_t offset = (intptr_t)ptr - (intptr_t)mPool;
		if (offset < 0 || offset >= mCount)
			return;

		mAvailableIndices.push_back(offset);
	}
};

class MyObject
{
public:
	int a;
	MyObject()
	{
		a = 100;
	}
};

int _tmain(int argc, _TCHAR* argv[])
{

	Pool pool;
	pool.Allocate(10, sizeof(MyObject));

	std::vector<MyObject*> objects;

	int nulled = 0;
	for (int i = 0; i < 15; i++)
	{
		MyObject* obj = pool.Get<MyObject>();
		if (obj != nullptr)
		{
			objects.push_back(obj);
			if (rand() % 100 > 50)
			{
				pool.Return(objects.back());
				objects.pop_back();
			}
		}
		else
			nulled++;

		
	}

	std::cout << "Objects Created: " << objects.size() << "\n";
	std::cout << "Null: " << nulled << "\n";

	while (true)
	{

	}
	return 0;
}

