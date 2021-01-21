#pragma once

#include "common.h"
#include "NetHandle.h"
#include "HttpRequest.h"
#include "HttpResponse.h"
#include "DispatchUnit.hpp"

namespace small_http
{
	class ServerHandle : public DispatchUnit
	{
	public:

		ServerHandle()
			:
			m_pkReq(nullptr),
			m_pkRsp(nullptr),
			m_pkNetHandle(nullptr)
			//m_ID(-1)
		{

		}
		~ServerHandle()
		{
			if (m_pkReq) delete m_pkReq;
			if (m_pkRsp) delete m_pkRsp;
			if (m_pkNetHandle) delete m_pkNetHandle;
			m_pkReq = nullptr;
			m_pkRsp = nullptr;
			m_pkNetHandle = nullptr;
		}

		virtual DispatchMode GetMode()
		{
			return DISPATCH_SERVER;
		}

		bool isReadDataFull()
		{
			return m_pkReq->IsDataFull();
		}

		void addReadDataLength(int len)
		{
			m_pkReq->AddLen(len);
		}

		bool isWriteDataFull()
		{
			return m_pkRsp->IsDataFull();
		}

		void addWriteDataLength(int len)
		{
			m_pkRsp->AddLen(len);
		}

		char* GetWritePos()
		{
			return m_pkRsp->GetWritePos();
		}

		int GetNeedWriteLen()
		{
			return m_pkRsp->GetNeedWriteLen();
		}

		char* GetReadPos()
		{
			if (!m_pkReq)
			{
				m_pkReq = new HttpRequest;
			}
			return m_pkReq->GetReadPos();
		}

		int GetNeedReadLen()
		{
			if (!m_pkReq)
			{
				m_pkReq = new HttpRequest;
			}
			return m_pkReq->GetNeedReadLen();
		}

		void SetNetHandle(NetHandle* handle)
		{
			m_pkNetHandle = handle;
		}

		void SetRsp(HttpResponse* pkRsp)
		{
			m_pkRsp = pkRsp;
		}

		const char* GetVersion()
		{
			static std::string tem = "HTTP/1.0";

			if (m_pkReq)
			{
				return m_pkReq->GetVersion();
			}
			return tem.c_str();
		}

		const char* GetReqPath()
		{
			static std::string tem = "/";
			if (m_pkReq)
			{
				return m_pkReq->GetReqPath();
			}
			return tem.c_str();
		}

		const char* GetReqData()
		{
			static std::string tem = "";
			if (m_pkReq)
			{
				return m_pkReq->GetReqData();
			}
			return tem.c_str();
		}

		SocketHandle GetNetHandle()
		{
			return m_pkNetHandle->m_Handle;
		}
		SocketType GetNetType()
		{
			return m_pkNetHandle->m_eType;
		}



		HttpRequest* m_pkReq;
		HttpResponse* m_pkRsp;
		NetHandle* m_pkNetHandle;
	};

}
