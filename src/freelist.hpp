#pragma once

#ifndef __FREE_LIST__
#define __FREE_LIST__

#include <iostream>

namespace small_http
{
	typedef std::uint32_t FL_SIZE;


	template<class T>
	class FreeList
	{
	public:

		FreeList()
			:
			head(0)
			, tail(0)
			, capacity(0)
			, data(NULL)
		{

		}

		~FreeList()
		{
			UnInit();
		}

		bool Init(FL_SIZE len)
		{
			if (!len) return false;

			data = (T*)malloc(len * sizeof(T));
			if (!data) return false;

			capacity = len;
			head = 0;
			tail = 0;
			return true;
		}

		bool UnInit()
		{
			if (!data) return false;
			free(data);
			data = NULL;
			capacity = 0;
			head = 0;
			tail = 0;
			return true;
		}

		T pop()
		{
			if (tail == head) return NULL;

			T pData = data[tail];
			data[tail] = NULL;
			tail = (tail + 1) % capacity;

			return pData;
		}

		bool push(T pData)
		{
			if (((head + 1) % capacity) == tail) return false;
			data[head] = pData;
			head = (head + 1) % capacity;

			return true;
		}

	private:
		volatile FL_SIZE head;
		volatile FL_SIZE tail;
		FL_SIZE capacity;
		T* data;
	};
}

#endif
