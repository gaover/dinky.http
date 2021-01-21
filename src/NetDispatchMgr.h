#pragma once


#include "common.h"
#include "NetHandle.h"
#include "NetHandle.h"
#define MAX_REQ_PER_TIME 8192

namespace small_http
{

#define ACCEPT_LIST_SIZE 1000

	enum NetEvent
	{
		NE_INVALID,
		NE_ACCEPT,
		NE_READ,
		NE_WRITE,
		NE_CLOSE,
	};

	class ServerHandle;
	class DispatchUnit;

#if defined(__LINUX__)
	typedef int DispatchHandle;
#define INVALID_DISPATCH -1


	struct NetEventInfo
	{
		NetEvent _event;
		int _len;	// 接收或发送的数据长度
		DispatchUnit* _netHandleID;
	};


#else
	typedef HANDLE DispatchHandle;
#define INVALID_DISPATCH NULL
	struct NetEventInfo : public OVERLAPPED
	{
		NetEvent _event;
		int _len;	// 接收或发送的数据长度
		union 
		{
			DispatchUnit* _netHandleID;
			void * pObj;
		};
	};

	struct AcceptOverlapped : public OVERLAPPED
	{
		NetEvent _event;
		SocketHandle serverSocket;
		SocketHandle clientSocket;

		char lpOutputBuf[128];
		DWORD dwBytes;
	};

#endif

	struct NetDispatchMgr
	{
		NetDispatchMgr();


		int TickEvent(NetEventInfo* pEvent);

		bool AddWriteHandle(DispatchUnit*);
		bool AddReadHandle(DispatchUnit*);



		void Stop();

		DispatchHandle _dispatch;

#if defined(__LINUX__)
		struct epoll_event _EpollEvent[MAX_REQ_PER_TIME];
#else
		LPFN_ACCEPTEX lpfnAcceptEx = NULL;
		OVERLAPPED_ENTRY _DispatchA[MAX_REQ_PER_TIME];
		bool AddAcceptEx(SocketHandle hand, void* data);
		bool addNewClientAccept(AcceptOverlapped*);
#endif


	};

}
