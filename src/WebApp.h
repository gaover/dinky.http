#pragma once

#include "common.h"

#include "NetHandle.h"
#include "HttpRequest.h"
#include "HttpResponse.h"
#include "NetDispatchMgr.h"
#include "Thread.h"
#include "freelist.hpp"
#include "DispatchUnit.hpp"



namespace small_http
{
	class ServerHandle;

	// �ص�������Ǽ�ʱ����Ӧ�����߼�����Ҫ��¼��id
	// ���߼��㷵��Ӧ��ʱ�����void PushResponse(CLIENTID id, const string& data)
	// ����ֵ��true,������Ӧ����������ͣ����۷��������Ƿ�Ϊ�գ���false:˵�����߼���ص�ʱ�ٷ���
	// �������볬ʱʱ���Է���ʱ��ر�����
	typedef bool(*HSCB)(CLIENTID id, const char* req, string&);


	struct WebApp : public Thread
	{
		WebApp(const string& addr);
		~WebApp();

		// ��֧�ַ�����һ����ͬ·�����Բ�ͬ�����ĵ��ã���ֻ��·����ΪΨһ����
		// ˭��֧�ֶ�Ԫ����������+·�����Լ�д
		void Handle(const string& path, HSCB cb, const string& method = "POST");

		void PushResponse(ServerHandle*, const string& data);
		bool PushFullRequest(ServerHandle*);
		void PushFullResponse(ServerHandle*);
		bool NetDispatch();

		unsigned short GetListenPort();
		void PrintListenInfo()
		{
			std::string ipp = ip;
			if (ip == "0.0.0.0")
			{
				ipp = "127.0.0.1";
			}
			cout << "SERVER INFO:http://" << ipp << ":" << port << endl;
		}
		void Stop();
		void TickOnce();

		bool SyncCallback();
		bool ASyncCallback();

		bool StartApp();
		bool PrePareApp();

		// ���߼������
		bool ServerLogicThreadTickCallback();

	private:

		// ��ʹ��AcceptEx,ֱ�Ӽ��������client
		void ServerAccept();

		bool TickSendRsp();

		void DeleteUnit(DispatchUnit* pkUnit, std::uint32_t et = 0)
		{
			if (!et)
			{
				delete pkUnit;
				return;
			}
		}

		inline CLIENTID NewID() { return id__++; }

		std::string _addr;
		std::string ip;
		std::string port;
		CLIENTID id__;
		unsigned int m_uiPort;

		NetDispatchMgr _Dicpatch;
		NetEventInfo _aEvent[MAX_REQ_PER_TIME];
		ServerHandle* m_pkServer;
		NetHandle* m_pkSocketHandle;

		unordered_map<string, HSCB> _callback;

		FreeList<ServerHandle*> _waitServerCBQueue;
		FreeList<ServerHandle*> _waitRspQueue;

		std::thread* m_pkASyncThreadHandle;
		std::string m_rspData;
		std::string m_TemPath;

	};

}
