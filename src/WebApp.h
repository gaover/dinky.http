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

	// 回调如果不是即时返回应答，则逻辑层需要记录此id
	// 待逻辑层返回应答时请调用void PushResponse(CLIENTID id, const string& data)
	// 返回值：true,表明此应答可以立马发送（无论返回数据是否为空），false:说明在逻辑层回调时再发送
	// 可以引入超时时间或对方超时后关闭连接
	typedef bool(*HSCB)(CLIENTID id, const char* req, string&);


	struct WebApp : public Thread
	{
		WebApp(const string& addr);
		~WebApp();

		// 不支持方法不一样，同路径可以不同方法的调用，即只按路径作为唯一索引
		// 谁想支持二元索引（方法+路径）自己写
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

		// 在逻辑层调用
		bool ServerLogicThreadTickCallback();

	private:

		// 不使用AcceptEx,直接检测有无新client
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
