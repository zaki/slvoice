/* server_util.cpp -- server utility module
 *
 *			Copyright 2008, 3di.jp Inc
 */

#include <curl/curl.h>

#include "main.h"
#include "server_util.hpp"

//=============================================================================
static string responseBuf;
static int handle_returned_data(
        char* ptr,
        size_t size,
        size_t nmemb,
        void* stream)
{
    char* pbuf = new char[size*nmemb+1];
    memcpy(pbuf, ptr, size*nmemb);
    pbuf[size*nmemb] = 0;
    responseBuf += pbuf;
    delete[] pbuf;
    return size*nmemb;
}

void ServerUtil::getContent(string& url, string& content)
{
	CURL *curl = NULL;
	CURLcode res;

    string header;
//    struct curl_slist *headerlist = NULL;
    long status = 0;
	string ret = "";

	try {
		curl = curl_easy_init();

		if (curl == NULL) 
		{
			g_logger->Fatal() << "Error initializing curl" << endl;
			exception e;
			throw e;
		}

		g_logger->Debug() << "Curl easy_init OK" << endl;

		curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
	    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, handle_returned_data);

	    responseBuf = "";

		g_logger->Info() << "Curl requesting to " << url << endl;

		res = curl_easy_perform(curl);

		g_logger->Debug() << "Curl easy_perform result=" << res << endl;

		if (res != CURLE_OK) 
		{
			g_logger->Fatal() << "Curl error" << endl;
			exception e;
			throw e;
		}

		g_logger->Debug() << "Curl easy_perform OK" << endl;

		res = curl_easy_getinfo(curl, CURLINFO_HTTP_CODE, &status);

		if (res != CURLE_OK || status != 200) 
		{
			g_logger->Fatal() << "Curl error" << endl;
			exception e;
			throw e;
		}

		ret = responseBuf;

		g_logger->Info() << "Curl response " << ret << endl;

		content = responseBuf;

		curl_easy_cleanup(curl);
	}
	catch (exception e) 
	{
		g_logger->Fatal() << "Curl error " << e.what() << endl;
		if (curl != NULL) {
			curl_easy_cleanup(curl);
		}
		throw e;
	}
}

void ServerUtil::getServerInfo(string& url, SIPServerInfo& sipinfo)
{
	string ret = "";

	try {
		getContent(url, ret);

		stringstream ss(ret);

		ss >> sipinfo.sipuri >> sipinfo.reguri >> sipinfo.proxyuri;
	}
	catch (exception e) 
	{
		g_logger->Fatal() << "getServerInfo error " << e.what() << endl;
		throw e;
	}
}

