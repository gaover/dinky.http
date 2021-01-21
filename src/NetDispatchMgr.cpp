#include "NetDispatchMgr.h"
#include "ServerHandle.h"

namespace small_http
{


#if defined(__LINUX__)

	NetDispatchMgr::NetDispatchMgr()
	{
		_dispatch = epoll_create1(EPOLL_CLOEXEC);
		if (_dispatch == INVALID_DISPATCH)
		{
			abort();
		}

	}

	void NetDispatchMgr::Stop()
	{
		if (_dispatch != INVALID_DISPATCH)
		{
			close(_dispatch);
		}
	}

	bool NetDispatchMgr::AddReadHandle(DispatchUnit* pkElem)
	{
		if (!pkElem) return false;
#if defined(USE_EPOLL_ET_MODE)
		epoll_event ev;
		ev.events = 0;
		int op = pkElem->_event ? EPOLL_CTL_MOD : EPOLL_CTL_ADD;
		pkElem->_event = EPOLLIN | EPOLLET;
		ev.events |= pkElem->_event;
		ev.data.ptr = pkElem;
		int ret = ::epoll_ctl(_dispatch, op, pkElem->GetNetHandle(), &ev);
		if (ret) {
			printf("et [%lld]ID ctl error %s\n", pkElem->GetID(), strerror(errno));
		}
		return !ret;
#else
		epoll_event ev;
		if (pkElem->_event & EPOLLIN)
		{
			return true;
		}
		//memset(&ev, 0, sizeof(ev));
		int op = pkElem->_event ? EPOLL_CTL_MOD : EPOLL_CTL_ADD;
		pkElem->_event |= EPOLLIN;
		ev.events = 0;
		ev.events |= pkElem->_event;
		ev.data.ptr = pkElem;
		int ret = ::epoll_ctl(_dispatch, op, pkElem->GetNetHandle(), &ev);
		if (ret) {
			printf("[%lld]ID ctl error %s\n", pkElem->GetID(), strerror(errno));
		}
		return !ret;
#endif

	}

	bool NetDispatchMgr::AddWriteHandle(DispatchUnit* pkElem)
	{
		if (!pkElem) return false;
#if defined(USE_EPOLL_ET_MODE)
		epoll_event ev;
		ev.events = 0;
		int op = pkElem->_event ? EPOLL_CTL_MOD : EPOLL_CTL_ADD;
		pkElem->_event = EPOLLOUT | EPOLLET;
		ev.events |= pkElem->_event;
		ev.data.ptr = pkElem;
		int ret = ::epoll_ctl(_dispatch, op, pkElem->GetNetHandle(), &ev);
		if (ret) {
			printf("et [%lld]ID ctl error %s\n", pkElem->GetID(), strerror(errno));
		}
		return !ret;
#else
		epoll_event ev;
		if (pkElem->_event & EPOLLOUT)
		{
			return true;
		}
		//memset(&ev, 0, sizeof(ev));
		int op = pkElem->_event ? EPOLL_CTL_MOD : EPOLL_CTL_ADD;
		pkElem->_event |= EPOLLOUT;
		ev.events = 0;
		ev.events |= pkElem->_event;
		ev.data.ptr = pkElem;
		int ret = ::epoll_ctl(_dispatch, op, pkElem->GetNetHandle(), &ev);
		if (ret) {
			printf("[%lld]ID ctl error %s\n", pkElem->GetID(), strerror(errno));
		}
		return !ret;
#endif
	}

	int NetDispatchMgr::TickEvent(NetEventInfo* pEvent)
	{
		//memset(_EpollEvent, 0, sizeof(_EpollEvent));
		int sz = 0;
		sz = ::epoll_wait(_dispatch, _EpollEvent, MAX_REQ_PER_TIME, 10);
		if (sz < 0)
		{
			return 0;
		}
		// 这里需要对 NetEventInfo 里的信息加工
		for (int i = 0; i != sz; ++i)
		{
			struct epoll_event& rkEvent = _EpollEvent[i];
			NetEventInfo* pkTempEvent = pEvent + i;
			pkTempEvent->_netHandleID = (DispatchUnit*)rkEvent.data.ptr;
			pkTempEvent->_len = 0;
			pkTempEvent->_event = NE_INVALID;
			if (rkEvent.events & EPOLLERR || rkEvent.events & EPOLLHUP)
			{
				pkTempEvent->_event = NE_CLOSE;
			}
			else if (rkEvent.events & EPOLLIN)
			{
				pkTempEvent->_event = NE_READ;
			}
			else if (rkEvent.events & EPOLLOUT)
			{
				pkTempEvent->_event = NE_WRITE;
			}
		}
		return sz;
	}






#else

	NetDispatchMgr::NetDispatchMgr()
	{
		_dispatch = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
		if (_dispatch == INVALID_DISPATCH)
		{
			abort();
		}
	}

	void NetDispatchMgr::Stop()
	{
		if (_dispatch != INVALID_DISPATCH)
		{
			CloseHandle(_dispatch);
		}
	}


	bool NetDispatchMgr::AddWriteHandle(DispatchUnit* pkUnit)
	{
		if (!pkUnit->IsAttch())
		{
			pkUnit->SetAttch();
			CreateIoCompletionPort((HANDLE)pkUnit->GetNetHandle(), _dispatch, (ULONG_PTR)pkUnit, 0);
		}

		NetEventInfo* pkEvt = new NetEventInfo();
		memset(pkEvt, 0, sizeof(NetEventInfo));
		pkEvt->_event = NE_WRITE;
		pkEvt->_netHandleID = pkUnit;
		WSABUF buf;

		buf.buf = pkUnit->GetWritePos();
		buf.len = pkUnit->GetNeedWriteLen();

		DWORD flags = 0;
		DWORD dwSz = 0;
		if (WSASend(pkUnit->GetNetHandle(), &buf, 1, &dwSz, 0, pkEvt, NULL) == SOCKET_ERROR)
		{
			int iSendError = WSAGetLastError();
			if (iSendError != ERROR_IO_PENDING)
			{
				// 返回false则删除该elem,由外层删除
				cout << "WSASend error " << iSendError << endl;
				return false;
			}
		}
		return true;



	}

	bool NetDispatchMgr::AddReadHandle(DispatchUnit* pkUnit)
	{
		if (!pkUnit->IsAttch())
		{
			pkUnit->SetAttch();
			CreateIoCompletionPort((HANDLE)pkUnit->GetNetHandle(), _dispatch, (ULONG_PTR)pkUnit, 0);
		}

		NetEventInfo* pkEvt = new NetEventInfo();
		memset(pkEvt, 0, sizeof(NetEventInfo));
		pkEvt->_event = NE_READ;
		pkEvt->_netHandleID = pkUnit;
		WSABUF buf;

		buf.buf = pkUnit->GetReadPos();
		buf.len = pkUnit->GetNeedReadLen();

		DWORD flags = 0;
		DWORD dwSz = 0;
		if (WSARecv(pkUnit->GetNetHandle(), &buf, 1, &dwSz, &flags, (LPWSAOVERLAPPED)pkEvt, 0) == SOCKET_ERROR)
		{
			int iSendError = WSAGetLastError();
			if (iSendError != ERROR_IO_PENDING)
			{
				// 返回false则删除该elem,由外层删除
				cout << "WSARecv error " << iSendError << endl;
				return false;
			}
		}
		return true;
	}

	bool NetDispatchMgr::addNewClientAccept(AcceptOverlapped* ov)
	{
		//存放网络地址的长度

		ZeroMemory(ov, sizeof(OVERLAPPED));

		int addrLen = sizeof(sockaddr_in) + 16;
		//创建socket
		SocketHandle _socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		ov->clientSocket = _socket;
		int bRetVal = lpfnAcceptEx(ov->serverSocket, _socket, ov->lpOutputBuf,
			0, addrLen, addrLen,
			&ov->dwBytes, (LPOVERLAPPED)ov);

		if (bRetVal == FALSE)
		{
			int error = WSAGetLastError();
			if (error != WSA_IO_PENDING)
			{
				cout << "err" << GetLastError();
				closesocket(_socket);
				return false;
			}
		}

		return true;
	}

	bool NetDispatchMgr::AddAcceptEx(SocketHandle hand, void* data)
	{
		int iRet;

		if (CreateIoCompletionPort((HANDLE)hand, _dispatch, (ULONG_PTR)data, 0) == NULL)
		{
			closesocket(hand);
			return false;
		}

		DWORD dwBytes = 0;

		GUID GuidAcceptEx = WSAID_ACCEPTEX;

		iRet = WSAIoctl(hand, SIO_GET_EXTENSION_FUNCTION_POINTER,
			&GuidAcceptEx, sizeof(GuidAcceptEx),
			&lpfnAcceptEx, sizeof(lpfnAcceptEx),
			&dwBytes, NULL, NULL);
		if (iRet == SOCKET_ERROR) {
			closesocket(hand);
			WSACleanup();
			return false;
		}

		for (int i = 0; i < ACCEPT_LIST_SIZE; i++)
		{
			AcceptOverlapped *ov = new AcceptOverlapped();
			ov->_event = NE_ACCEPT;
			ov->serverSocket = hand;
			addNewClientAccept(ov);
		}

		return true;
	}


	int NetDispatchMgr::TickEvent(NetEventInfo* pEvent)
	{
		ULONG ulSz = 0;

		if (!::GetQueuedCompletionStatusEx(_dispatch, _DispatchA, MAX_REQ_PER_TIME, &ulSz, 1, true))
		{
			int code = GetLastError();
			if (code != 258)
			{
				cout << "error code " << code << endl;
			}
			ulSz = 0;
		}
		int sz = ulSz;
		// 这里需要对 NetEventInfo 里的信息加工
		for (int i = 0; i != sz; ++i)
		{
			NetEventInfo* pkTempEvent = pEvent + i;
			LPOVERLAPPED_ENTRY pkEntry = _DispatchA + i;
			NetEventInfo* pkTempEvent2 = (NetEventInfo*)pkEntry->lpOverlapped;

			if (pkTempEvent2->_event == NE_ACCEPT)
			{
				pkTempEvent->_event = NE_ACCEPT;
				pkTempEvent->pObj = pkTempEvent2;
				continue;
			}

			pkTempEvent->_netHandleID = (DispatchUnit*)pkEntry->lpCompletionKey;
			pkTempEvent->_len = pkEntry->dwNumberOfBytesTransferred;
			pkTempEvent->_event = pkTempEvent2->_event;
			if (pkEntry->dwNumberOfBytesTransferred == 0)
			{
				pkTempEvent->_event = NE_CLOSE;
			}
			delete pkTempEvent2;
		}
		return sz;
	}



#endif

}

