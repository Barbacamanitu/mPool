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

public:
	void Allocate(int count, size_t size)
	{
		mObjectSize = size;
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

	MyObject* mObj = pool.Get<MyObject>();
	std::cout << mObj->a;

	while (true)
	{

	}
	return 0;
}

