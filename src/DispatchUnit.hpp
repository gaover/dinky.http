#pragma once

#include "common.h"

namespace small_http
{


	enum DispatchMode
	{
		DISPATCH_BEGIN,

		DISPATCH_SERVER,
		DISPATCH_CLIENT,

		DISPATCH_END,
	};


	class DispatchUnit
	{
	public:
		DispatchUnit()
			:
#if defined(__LINUX__)
			_event(0)
#else
			_attch(false)
#endif
			, m_ID(-1)
		{
		}
		virtual ~DispatchUnit()
		{
#if defined(__LINUX__)
			_event = 0;
#else
			_attch = false;

#endif
		}
		virtual bool isReadDataFull() = 0;
		virtual void addReadDataLength(int len) = 0;
		virtual bool isWriteDataFull() = 0;
		virtual void addWriteDataLength(int len) = 0;
		virtual DispatchMode GetMode() = 0;
		virtual char* GetWritePos() = 0;
		virtual int GetNeedWriteLen() = 0;
		virtual char* GetReadPos() = 0;
		virtual int GetNeedReadLen() = 0;
		virtual SocketHandle GetNetHandle() = 0;
		virtual SocketType GetNetType() = 0;

		bool IsOtherClose() {
			return m_bOtherSideClose;
		}

		void SetOtherClose(bool flag) { m_bOtherSideClose = flag; }

		CLIENTID GetID() {
			return m_ID;
		}

		void SetMyID(CLIENTID id)
		{
			m_ID = id;
		}
#if defined(__LINUX__)

		virtual int Send()
		{
			int n = 0;
			char* ptr = GetWritePos();
			int iNeedLen = GetNeedWriteLen();
			while (iNeedLen > 0)
			{
				int len = send(GetNetHandle(), ptr + n, iNeedLen, 0);
				if (len >= 0)
				{
					n += len;
					//iNeedLen -= len;
					return n;
				}
				int e = errno;

				if (e == EAGAIN || e == EWOULDBLOCK)
				{
					break;
				}
				if (e == EINTR)
				{
					continue;
				}
				return -1;
			}

			return n;
		}
		virtual int Recv()
		{
			int n = 0;
			char* ptr = GetReadPos();
			int iNeedLen = GetNeedReadLen();
			while (iNeedLen > 0)
			{
				int len = recv(GetNetHandle(), ptr + n, iNeedLen, 0);
				if (len > 0)
				{
					n += len;
					//iNeedLen -= len;
					return n;
				}
				if (len == 0)
				{
					break;
				}
				int e = errno;
				if (e == EAGAIN || e == EWOULDBLOCK)
				{
					break;
				}
				if (e == EINTR)
				{
					continue;
				}
				return -1;
			}

			return n;
		}

		int _event;

#else
		bool IsAttch() { return _attch; }
		void SetAttch() { _attch = true; }
		bool _attch;
#endif

		CLIENTID m_ID;
		bool m_bOtherSideClose = false;
	};



}