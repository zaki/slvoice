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

		if (curl == NULL) {
			VFVW_LOG("error");
			exception e;
			throw e;
		}

		VFVW_LOG("curl_easy_init ok");

		curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
	    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, handle_returned_data);

	    responseBuf = "";

		VFVW_LOG("requesting to %s", url.c_str());

		res = curl_easy_perform(curl);

		VFVW_LOG("curl_easy_perform result=%d", res);

		if (res != CURLE_OK) {
			VFVW_LOG("error");
			exception e;
			throw e;
		}

		VFVW_LOG("curl_easy_perform ok");

		res = curl_easy_getinfo(curl, CURLINFO_HTTP_CODE, &status);

		if (res != CURLE_OK || status != 200) {
			VFVW_LOG("error");
			exception e;
			throw e;
		}

		ret = responseBuf;

		VFVW_LOG("response body : %s", ret.c_str());

		content = responseBuf;

		curl_easy_cleanup(curl);
	}
	catch (...) {

		VFVW_LOG("Error");

		if (curl != NULL) {
			curl_easy_cleanup(curl);
		}

		exception e;
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
	catch (...) {
		VFVW_LOG("Error");
		exception e;
		throw e;
	}
}

