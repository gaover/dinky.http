#pragma once


#include "common.h"
#include "NetHandle.h"

namespace small_http
{
	enum HttpMethod
	{
		METHOD_GET,
		METHOD_POST,
		METHOD_NUM,
	};

#define HTTP_MSG_BLANK " "
#define HTTP_MSG_SEP ":"
#define HTTP_MSG_RETURN "\r\n"
#define CONTENT_LENGTH "Content-Length"

#define HTTP_VERSION_MAX_LEN 32
#define REQUEST_INFO_MAX_LEN 2048
#define REQUEST_PATH_MAX_LEN 256
#define REQUEST_DATA_MAX_LEN (REQUEST_INFO_MAX_LEN + 512)

	struct HttpRequest
	{
		HttpRequest()
		{
			memset(m_byteDataBuff, 0, sizeof(m_byteDataBuff));
			m_iDataLength = 0;
			m_iBodyLen = -1;
			m_bDataFull = false;
			m_strReqPath[0] = 0;
			m_strReqData[0] = 0;
			strcpy(m_strVersion, "HTTP/1.0");
		}
		bool IsDataFull();
		bool IsDataFull_noString();
		void AddLen(int len)
		{
			m_iDataLength += len;
		}

		void AddHeaderOption(const string& key, const string& val);
		void AddHeaderString(string& header);
		void SetMethod(HttpMethod& _method)
		{
			method = _method;
		}
		void SetReqData(const string& data)
		{
			strncpy(m_strReqPath, data.c_str(), sizeof(m_strReqPath));
		}
		void SetReqPath(const string& path)
		{
			strncpy(m_strReqPath, path.c_str(), sizeof(m_strReqPath));
		}
		char* GetReqPath()
		{
			return m_strReqPath;
		}
		char* GetVersion()
		{
			return m_strVersion;
		}
		char* GetReqData()
		{
			return m_strReqData;
		}

		char* GetReadPos()
		{
			return m_byteDataBuff + m_iDataLength;
		}

		int GetNeedReadLen()
		{
			return REQUEST_DATA_MAX_LEN - m_iDataLength;
		}

		static HttpRequest* NewRequest(const std::string  & method, const string& url);
		HttpMethod method;
		char m_strReqPath[REQUEST_PATH_MAX_LEN];
		char m_strReqData[REQUEST_INFO_MAX_LEN];
		char m_strVersion[HTTP_VERSION_MAX_LEN];
		bool m_bDataFull;
		int m_iBodyLen; // 此字段只在post方法时有意义
		// 请求数据
		char m_byteDataBuff[REQUEST_DATA_MAX_LEN];
		int m_iDataLength;
		std::unordered_map<string, string> m_HeaderOption;

	};

}
