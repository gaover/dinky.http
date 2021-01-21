
#include "WebApp.h"
#include "ServerHandle.h"

namespace small_http
{


	WebApp::WebApp(const string& addr)
		:
		Thread(addr.c_str()),
		id__(0),
		m_uiPort(0xffff),
		m_pkServer(nullptr),
		m_pkSocketHandle(nullptr)
	{
#if defined(__LINUX__)


#else
		WSADATA wsaData;
		WSAStartup(MAKEWORD(2, 0), &wsaData);
#endif
		m_pkASyncThreadHandle = nullptr;
		_addr = addr;
	}

	WebApp::~WebApp()
	{
		// ���ر��˻�����ʲô��˭��д��д
		Stop();
	}

	bool WebApp::PrePareApp()
	{
		// addr��ʽ��"ip:port"

		auto it = _addr.find_first_of(":");
		std::string ip = _addr.substr(0, it);
		std::string port = _addr.substr(it + 1, string::npos);

#ifndef __LINUX__
		SocketHandle hListenSock = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);
#else
		SocketHandle hListenSock = ::socket(PF_INET, SOCK_STREAM, IPPROTO_IP);
#endif


		if (hListenSock == INVALID_SOCKET)
		{
			return false;
		}

		ReuseSocketHandle(hListenSock, 1);
		ReuseSocketPort(hListenSock, 1);

		if (ip.empty())
		{
			ip = "0.0.0.0";
		}

		unsigned short usport = atoi(port.c_str());
		struct sockaddr_in sa;
		memset(&sa, 0, sizeof(sa));
		sa.sin_family = AF_INET;
		sa.sin_port = htons(usport);
		if (!inet_pton(AF_INET, ip.c_str(), &sa.sin_addr.s_addr))
		{
			return false;
		}

		if (::bind(hListenSock, (sockaddr*)&sa, sizeof(sockaddr_in)) == SOCKET_ERROR)
		{
			return false;
		}

		if (::listen(hListenSock, ACCEPT_LIST_SIZE) == SOCKET_ERROR)
		{
			return false;
		}

		m_pkSocketHandle = new NetHandle(hListenSock, NHT_LISTEN);
		Nonblocking(hListenSock);

		m_pkServer = new ServerHandle;
		m_pkServer->SetMyID(NewID());
		m_pkServer->SetNetHandle(m_pkSocketHandle);

		m_rspData.reserve(4192);
		m_TemPath.reserve(256);

#ifdef __LINUX__

		_Dicpatch.AddReadHandle(m_pkServer);
#else
		if (!_Dicpatch.AddAcceptEx(hListenSock, this))
		{
			return false;
		}

#endif
		GetListenPort();
		PrintListenInfo();

		return _waitServerCBQueue.Init(MAX_REQ_PER_TIME)
			&& _waitRspQueue.Init(MAX_REQ_PER_TIME);
	}

	bool WebApp::SyncCallback()
	{
		while (IsRun())
		{
			ServerLogicThreadTickCallback();
		}

		return false;
	}

	bool WebApp::ASyncCallback()
	{
		m_pkASyncThreadHandle = new std::thread(
			std::bind([](WebApp* pkApp) 
		{
			if (!pkApp) return;
			while (pkApp->IsRun())
			{
				if (!pkApp->ServerLogicThreadTickCallback())
				{
#ifdef __LINUX__
					small_http::sleep(1);
#else
					small_http::sleep(0);
#endif
				}
			}
		}, this)
		);
		return m_pkASyncThreadHandle != nullptr;
	}


	bool WebApp::StartApp()
	{
		if (!PrePareApp())
		{
			return false;
		}

		return Run();
	}

	void WebApp::Handle(const string& path, HSCB cb, const string& method)
	{
		// ��ͬ��path�Ḳ��֮ǰ�Ļص�
		if (cb)
		{
			_callback[path] = cb;
		}
	}

	void WebApp::PushResponse(ServerHandle* pkElem, const string& data)
	{
		if (pkElem && !pkElem->m_pkRsp)
		{
			HttpResponse* rsp = new HttpResponse(pkElem->GetVersion(), data);
			pkElem->SetRsp(rsp);
			PushFullResponse(pkElem);
		}
	}

	bool WebApp::PushFullRequest(ServerHandle* pkElem)
	{
		return _waitServerCBQueue.push(pkElem);
	}

	void WebApp::PushFullResponse(ServerHandle* pkElem)
	{
		_waitRspQueue.push(pkElem);
	}

	unsigned short WebApp::GetListenPort()
	{
		if (m_uiPort == 0xffff)
		{
			sockaddr_storage sa;
			socklen_t salen = sizeof(sa);
			if (!::getsockname(m_pkSocketHandle->m_Handle, (struct sockaddr*)&sa, &salen))
			{
				if (sa.ss_family == AF_INET) {
					struct sockaddr_in *s = (struct sockaddr_in*)&sa;
					m_uiPort = ntohs(s->sin_port);
					char buf[40] = { 0 };
					//ip = inet_ntoa(s->sin_addr);
					inet_ntop(AF_INET, (void*)&s->sin_addr, buf, sizeof(buf));
					port = std::to_string(m_uiPort);
					ip = buf;
				}
			}
		}
		return m_uiPort;
	}

	bool WebApp::NetDispatch()
	{
		int sz = _Dicpatch.TickEvent(_aEvent);
		if (sz > 0)
		{
			for (int i = 0; i != sz; ++i)
			{
				NetEventInfo& pkEvent = _aEvent[i];
#ifndef __LINUX__
				if (pkEvent._event == NE_ACCEPT)
				{
					AcceptOverlapped* pkAccept = (AcceptOverlapped*)pkEvent.pObj;

					Nonblocking(pkAccept->clientSocket);
					DisableBuffering(pkAccept->clientSocket);
					SetLinginfo(pkAccept->clientSocket, 0, 0);
					NetHandle* newhandle = new NetHandle(pkAccept->clientSocket, NHT_CLIENT);
					ServerHandle* pkElem = new ServerHandle;
					pkElem->SetMyID(NewID());
					pkElem->SetNetHandle(newhandle);

					if (!_Dicpatch.AddReadHandle(pkElem))
					{
						DeleteUnit(pkElem);
					}
					_Dicpatch.addNewClientAccept(pkAccept);
					continue;
				}
#endif

				DispatchUnit* pkUnit = pkEvent._netHandleID;
				switch (pkUnit->GetMode())
				{
				case DISPATCH_SERVER:
				{
					ServerHandle* pkDataElem = static_cast<ServerHandle*>(pkUnit);
					switch (pkEvent._event)
					{
					case NE_READ:
					{
#if defined(__LINUX__)
						if (pkDataElem->GetNetType() != NHT_CLIENT)
						{
							ServerAccept();
#if defined(USE_EPOLL_ET_MODE)
							if (!_Dicpatch.AddReadHandle(pkDataElem))
							{
								printf("why accept add read failure\n");
							}
#endif
							continue;
						}
						else
						{
							// ����ֵС��0 ��Ϊ������
							pkEvent._len = pkDataElem->Recv();
							if (pkEvent._len < 0)
							{
								// ���ݷ���Ӧ��ɾ��
								DeleteUnit(pkDataElem);
								continue;
							}
						}
#endif
						pkDataElem->addReadDataLength(pkEvent._len);
						if (pkDataElem->isReadDataFull())
						{
							if (!PushFullRequest(pkDataElem))
							{
								// be careful
								DeleteUnit(pkDataElem);
							}
						}
						else
						{
							if (_Dicpatch.AddReadHandle(pkDataElem))
							{
								DeleteUnit(pkDataElem);
							}
						}
					}break;
					case NE_WRITE:
					{
#if defined(__LINUX__)
						pkEvent._len = pkDataElem->Send();
						if (pkEvent._len < 0)
						{
							// ���ݷ���Ӧ��ɾ��

							DeleteUnit(pkDataElem);
							continue;
						}
#endif

						pkDataElem->addWriteDataLength(pkEvent._len);
						if (pkDataElem->isWriteDataFull())
						{
							// ���ݷ���Ӧ��ɾ��
							DeleteUnit(pkDataElem);
						}
						else
						{
							if (!_Dicpatch.AddWriteHandle(pkDataElem))
							{
								// ���ݷ���Ӧ��ɾ��
								DeleteUnit(pkDataElem);
							}
						}
					}break;
					case NE_CLOSE:
					{
						// ����ǶԷ��ر�socket�����
						pkDataElem->SetOtherClose(true);
					}break;
					default: {}

					}
				}break;
				default:
					break;
				}


			}
		}

		return sz;
	}

	// windows��Ҫ�������߼��ã���ʹ��AcceptEx,ֱ�Ӽ��������client
	void WebApp::ServerAccept()
	{
		struct sockaddr addr;
		socklen_t len = sizeof(sockaddr);
		SocketHandle _s;
		int n = MAX_REQ_PER_TIME;
		while ((n--) && ((_s = accept(m_pkSocketHandle->m_Handle, &addr, &len)) != INVALID_SOCKET))
		{
			Nonblocking(_s);
			DisableBuffering(_s);
			SetLinginfo(_s, 0, 0);
			NetHandle* newhandle = new NetHandle(_s, NHT_CLIENT);
			ServerHandle* pkElem = new ServerHandle;
			pkElem->SetMyID(NewID());
			pkElem->SetNetHandle(newhandle);

			if (!_Dicpatch.AddReadHandle(pkElem))
			{
				DeleteUnit(pkElem);
			}
		}
	}

	// �˺�������������߳�������
	void WebApp::TickOnce()
	{
		bool bflag1;
		bool bflag2;
		while (IsRun())
		{
			bflag1 = NetDispatch();

			bflag2 = TickSendRsp();

			if (!bflag1 && !bflag2)
			{
#ifdef __LINUX__
				small_http::sleep(1);
#else
				small_http::sleep(0);
#endif
			}
		}
	}

	// �˺����ڷ������߼�������
	bool WebApp::ServerLogicThreadTickCallback()
	{
		DispatchUnit* pkUnit = NULL;
		// ������
		while ((pkUnit = _waitServerCBQueue.pop()))
		{
			// ���ȵ�����ʱ��iocpû�е��ȣ���epoll�ǿ�������һ�������߳�ɾ��������
			switch (pkUnit->GetMode())
			{
			case DISPATCH_SERVER:
			{
				ServerHandle *pkElem = static_cast<ServerHandle*>(pkUnit);
				if (pkElem)
				{
					//cout << "path: " << pkElem->GetReqPath() << "->req:" << pkElem->GetReqData() << endl;
					m_rspData.clear();
					m_TemPath = pkElem->GetReqPath();
					auto it = _callback.find(m_TemPath);
					if (it != _callback.end())
					{
						// ������
						bool ret = it->second(pkUnit->GetID(), pkElem->GetReqData(), m_rspData);
						if (ret)
						{
							//PushResponse(id, pkElem->GetReqPath() + " ->this path error" + pkElem->GetReqData());
						}

						PushResponse(pkElem, m_rspData);
					}
					else
					{
						// �������ڲ���
						m_rspData.append(m_TemPath);
						m_rspData.append(" @ this path is null:");
						m_rspData.append(pkElem->GetReqData());
						PushResponse(pkElem, m_rspData);
					}
				}
			}break;

			default:
				break;
			}
		}

		return !!pkUnit;
	}

	bool WebApp::TickSendRsp()
	{
		ServerHandle* pkElem = NULL;
		while ((pkElem = _waitRspQueue.pop()))
		{
			if (!_Dicpatch.AddWriteHandle(pkElem))
			{
				DeleteUnit(pkElem);
			}
		}

		return !!pkElem;
	}

	void WebApp::Stop()
	{
		if (!Thread::Stop()) return;

		//delete m_pkSocketHandle;
#ifndef __LINUX__
		SAFE_FREE(m_pkServer);
#endif
		_Dicpatch.Stop();
		_callback.clear();

		JoinThread(m_pkASyncThreadHandle);
	}

}
