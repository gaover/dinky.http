#pragma once

#include "common.h"

namespace small_http
{
#if defined(__LINUX__)
	typedef int SocketHandle;
#define INVALID_SOCKET -1
#define SOCKET_ERROR -1
#else
#include "winsock2.h"
#pragma comment(lib, "Ws2_32.lib")
	typedef SOCKET SocketHandle;
#endif


	typedef long long CLIENTID;
	typedef int SocketType;

#define NHT_LISTEN	0x0001
#define NHT_CLIENT	0x0002
#define NHT_READ	0x0004
#define NHT_SEND	0x0008

#define NHT_CLOSE_OTHER_SIDE 0x0001
#define NHT_CLOSE_INNER_ERROR 0x0002

	inline void Close(SocketHandle s)
	{
#if defined(__LINUX__)
		close(s);
#else
		//CancelIo((HANDLE)s);
		closesocket(s);
#endif
	}


	inline bool Nonblocking(SocketHandle fd)
	{
#if defined(__LINUX__)
		int arg = 1;
		::ioctl(fd, FIONBIO, &arg);
		return true;
#else
		u_long arg = 1;
		return (::ioctlsocket(fd, FIONBIO, &arg) == 0);
#endif

	}

	inline bool DisableBuffering(SocketHandle fd)
	{
		int arg = 1;
		return (setsockopt(fd, IPPROTO_TCP, TCP_NODELAY, (const char*)&arg, sizeof(arg)) == 0);
	}

	inline bool SetLinginfo(SocketHandle fd, int onoff, int linger_v)
	{
#ifdef __LINUX__
		struct linger stData;
#else
		struct linger stData;
#endif
		stData.l_onoff = onoff;
		stData.l_linger = linger_v;

		return (setsockopt(fd, SOL_SOCKET, SO_LINGER, (const char*)&stData, sizeof(stData)) == 0);
	}

	inline bool ReuseSocketHandle(SocketHandle fd, int reuse)
	{
		return setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, (const char*)&reuse, sizeof(reuse)) == 0;
	}

	inline bool ReuseSocketPort(SocketHandle fd, int reuse)
	{
#if defined(__LINUX__)
		return setsockopt(fd, SOL_SOCKET, SO_REUSEPORT, (const char*)&reuse, sizeof(reuse)) == 0;
#else
		return true;
#endif
	}

	struct NetHandle
	{
		NetHandle()
			:
			m_Handle(INVALID_SOCKET),
			m_eType(NHT_CLIENT)
		{

		}

		NetHandle(SocketHandle s, SocketType t)
			:
			m_Handle(s),
			m_eType(t)
		{

		}

		~NetHandle()
		{
			if (m_Handle != INVALID_SOCKET)
			{
				Close(m_Handle);
				m_Handle = INVALID_SOCKET;
			}
		}

		SocketHandle m_Handle;
		SocketType m_eType;
		std::uint32_t m_dwEvent;
	};

	inline void AppendCString(char* dest, size_t & offset, const char* src, size_t len) {
		memcpy(dest + offset, src, len);
		offset += (int)len;
	}

#define MACRO_LEN(a) (sizeof(a) -1)

	inline const char* ITOA(std::uint32_t val, char* buf, int radix)
	{
		snprintf(buf, 10, "%d", val);
		return buf;
	}



}
