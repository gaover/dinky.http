#include "common.h"
#include "HttpRequest.h"
#include <sstream>

namespace small_http
{
	HttpRequest* HttpRequest::NewRequest(const std::string  & _method, const string& url)
	{
		HttpRequest* pkReq = nullptr;

		HttpMethod eMethod = _method.compare("GET") ? METHOD_GET : METHOD_POST;
		switch (eMethod)
		{
		case METHOD_GET:
		case METHOD_POST:
		{
			// url不能放数据，即使是get
			auto it = url.find("/");
			if (it == string::npos)
			{
				return pkReq;
			}
			auto it2 = url.find("?");
			if (it2 != string::npos)
			{
				return pkReq;
			}
			pkReq = new HttpRequest;
			pkReq->SetReqPath(url.substr(it, string::npos));
		}break;
		default:
			return pkReq;
			break;
		}
		pkReq->SetMethod(eMethod);
		return pkReq;
	}

	void HttpRequest::AddHeaderOption(const string& key, const string& val)
	{
		if (!key.compare(CONTENT_LENGTH))
		{
			m_HeaderOption[key] = val;
		}
	}

	void HttpRequest::AddHeaderString(string& header)
	{
		if (!m_HeaderOption.empty())
		{
			for (auto it = m_HeaderOption.begin(); it != m_HeaderOption.end(); ++it)
			{
				header += it->first + HTTP_MSG_SEP + it->second + HTTP_MSG_RETURN;
			}
		}
	}

	bool HttpRequest::IsDataFull()
	{
		if (m_bDataFull) return m_bDataFull;

		char * it = strstr(m_byteDataBuff, "\r\n\r\n");
		if (it != NULL)
		{
			char _method[16] = { 0 };
			char req[512] = { 0 };
			// 说明头已经收完了
			sscanf(m_byteDataBuff, "%s %s %s", _method, req, m_strVersion);
			if (strcmp(_method, "GET") == 0)
			{
				method = METHOD_GET;
				char* pp = strchr(req, '?');
				if (pp != NULL)
				{
					*pp = 0;
					strncpy(m_strReqPath, req, sizeof(m_strReqPath));
					strncpy(m_strReqData, pp + 1, sizeof(m_strReqPath));
				}
				else
				{
					strncpy(m_strReqPath, req, sizeof(m_strReqPath));
				}
				m_bDataFull = true;
				return true;
			}
			else
			{

				method = METHOD_POST;
				strncpy(m_strReqPath, req, sizeof(m_strReqPath));

				int temLen = m_iDataLength - (int)(it - m_byteDataBuff) - 4;
				if (temLen < 0) temLen = 0;
				// 查找Content-Length
				if (m_iBodyLen == -1)
				{
					char * pContentLength = strstr(m_byteDataBuff, CONTENT_LENGTH);
					if (!pContentLength || it < pContentLength)
					{
						// 直接认为非法http请求, 让逻辑层直接失败
						printf("length invalid 1");
						return true;
					}

					char *pContenlengthoffset = pContentLength + sizeof(CONTENT_LENGTH);
					for (; pContenlengthoffset < it && *pContenlengthoffset != ':'; ++pContenlengthoffset);

					if (*pContenlengthoffset != ':')
					{
						// 直接认为非法http请求, 让逻辑层直接失败
						printf("length invalid 2");
						return true;
					}
					++pContenlengthoffset;
					for (; pContenlengthoffset < it && !isalnum(*pContenlengthoffset); ++pContenlengthoffset);

					if (!isalnum(*pContenlengthoffset))
					{
						// 直接认为非法http请求, 让逻辑层直接失败
						printf("length invalid 3");
						return true;
					}

					m_iBodyLen = atoi(pContenlengthoffset);

					if (m_iBodyLen == -1)
					{
						m_iBodyLen = temLen;
					}
				}

				if (temLen >= m_iBodyLen)
				{
					strncpy(m_strReqData, it + 4, sizeof(m_strReqData));
					m_bDataFull = true;
					return true;
				}
			}
		}
		return false;
	}

	//bool HttpRequest::IsDataFull()
	//{
	//	if (m_bDataFull) return m_bDataFull;

	//	std::string data;
	//	data.assign(m_byteDataBuff, m_iDataLength);

	//	auto it = data.find("\r\n\r\n");
	//	if (it != string::npos)
	//	{
	//		char _method[20] = { 0 };
	//		char req[2014] = { 0 };
	//		char ver[20] = { 0 };
	//		// 说明头已经收完了
	//		std::istringstream stringcin(data);
	//		//sscanf_s(m_byteDataBuff, "%s %s %s", _method, req, ver);
	//		stringcin >> _method >> req >> m_strVersion;
	//		if (strcmp(_method, "GET") == 0)
	//		{
	//			method = METHOD_GET;
	//			std::string reqData;
	//			reqData.assign(req, strlen(req));
	//			auto pp = reqData.find_first_of("?");
	//			m_strReqPath = reqData.substr(0, pp);
	//			if (pp != string::npos)
	//			{
	//				m_strReqData = reqData.substr(pp + 1, string::npos);
	//			}
	//			m_bDataFull = true;
	//			return true;
	//		}
	//		else
	//		{

	//			method = METHOD_POST;
	//			m_strReqPath = req;
	//			int temLen = (int)data.size() - (int)it - 4;
	//			if (temLen < 0) temLen = 0;
	//			// 查找Content-Length
	//			if (m_iBodyLen == -1)
	//			{
	//				size_t it1 = 0;
	//				auto it2 = data.find("\r\n", it1);
	//				it1 = it2 + 2;
	//				it2 = data.find("\r\n", it1);
	//				while (it1 != string::npos && it1 <= it && it2 <= it)
	//				{
	//					std::string tem = data.substr(it1, it2 - it1);
	//					if (tem.length() > sizeof(CONTENT_LENGTH))
	//					{
	//						if (tem.substr(0, sizeof(CONTENT_LENGTH) - 1) == CONTENT_LENGTH)
	//						{
	//							auto ppp = tem.find(":");
	//							m_iBodyLen = atoi(tem.substr(ppp + 1, string::npos).c_str());
	//							break;
	//						}
	//					}
	//					it1 = it2 + 2;
	//					it2 = data.find("\r\n", it1);
	//				}
	//				if (m_iBodyLen == -1)
	//				{
	//					m_iBodyLen = temLen;
	//				}
	//			}
	//			if (temLen == m_iBodyLen)
	//			{
	//				m_strReqData = data.substr(it + 4, string::npos);
	//				m_bDataFull = true;
	//				return true;
	//			}
	//		}
	//	}
	//	return false;
	//}

}
