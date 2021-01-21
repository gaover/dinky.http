#pragma once

#include "common.h"
#include "NetHandle.h"

namespace small_http
{

#define EMPTY_HTTP_RSP_BODY "HTTP/1.0 200 OK\r\n\r\n"
#define DEFAULT_RSP_BUFF_LENGTH  1024

	struct HttpResponse
	{
		HttpResponse()
			:
			m_iSendLen(0),
			m_iTotalLen(0),
			m_iDataOffset(0),
			pRealBuff(NULL)
		{

		}

		HttpResponse(const string& resData)
		{
			m_iTotalLen = (int)resData.size() + MACRO_LEN(EMPTY_HTTP_RSP_BODY);
			pRealBuff = GetBuff(m_iTotalLen);

			m_iSendLen = 0;
			m_iTotalLen = 0;
			m_iDataOffset = MACRO_LEN(EMPTY_HTTP_RSP_BODY);
			AppendCString(pRealBuff, m_iTotalLen, EMPTY_HTTP_RSP_BODY, MACRO_LEN(EMPTY_HTTP_RSP_BODY));
			AppendCString(pRealBuff, m_iTotalLen, resData.c_str(), resData.size());
		}

		HttpResponse(const char* ver, const string& resData)
		{

#define HTTP_OK_200 " 200 OK\r\n"
#define HTTP_RSP_CONTENT_LEN "Content-Length:"
#define HTTP_RSP_CONTENT_RETURN "\r\n"

			char ibuf[24];
			size_t ver_len = strlen(ver);
			m_iDataOffset = (int)ver_len + MACRO_LEN(HTTP_OK_200) + MACRO_LEN(HTTP_RSP_CONTENT_LEN) + MACRO_LEN(HTTP_RSP_CONTENT_RETURN HTTP_RSP_CONTENT_RETURN);
			m_iTotalLen = m_iDataOffset + resData.size();
			pRealBuff = GetBuff(m_iTotalLen);
			m_iTotalLen = 0;
			AppendCString(pRealBuff, m_iTotalLen, ver, ver_len);
			AppendCString(pRealBuff, m_iTotalLen, HTTP_OK_200 HTTP_RSP_CONTENT_LEN, MACRO_LEN(HTTP_OK_200 HTTP_RSP_CONTENT_LEN));
			ITOA(resData.size(), ibuf, 10);
			AppendCString(pRealBuff, m_iTotalLen, ibuf, strlen(ibuf));
			AppendCString(pRealBuff, m_iTotalLen, HTTP_RSP_CONTENT_RETURN HTTP_RSP_CONTENT_RETURN, MACRO_LEN(HTTP_RSP_CONTENT_RETURN HTTP_RSP_CONTENT_RETURN));
			AppendCString(pRealBuff, m_iTotalLen, resData.c_str(), resData.size());

			m_iSendLen = 0;
		}
		~HttpResponse()
		{
			if (pRealBuff && pRealBuff != vData)
			{
				free(pRealBuff);
				pRealBuff = NULL;
			}
		}

		const char* GetRspData()
		{
			if (pRealBuff)
			{
				return pRealBuff + m_iDataOffset;
			}
			return "";
		}

		bool IsDataFull()
		{
			return m_iTotalLen == m_iSendLen;
		}
		void AddLen(int len)
		{
			m_iSendLen += len;
		}

		char* GetWritePos()
		{
			return pRealBuff + m_iSendLen;
		}

		int GetNeedWriteLen()
		{
			return m_iTotalLen - m_iSendLen;
		}

	private:
		inline char * GetBuff(int len)
		{
			if (len <= DEFAULT_RSP_BUFF_LENGTH)
			{
				return vData;
			}
			else
			{
				return (char*)malloc(len);
			}
		}



		size_t m_iSendLen;
		size_t m_iTotalLen;
		size_t m_iDataOffset;
		char vData[DEFAULT_RSP_BUFF_LENGTH];
		char * pRealBuff;
	};

}
