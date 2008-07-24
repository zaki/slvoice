/* server_util.hpp -- server utility definition
 *
 *			Copyright 2008, 3di.jp Inc
 */

#ifndef _SERVER_UTIL_HPP_
#define _SERVER_UTIL_HPP_

//=============================================================================
// ServerUtil class

class ServerUtil
{
    public:
		static void getServerInfo(string&, SIPServerInfo&);
		static void getContent(string&, string&);
};

#endif //_SERVER_UTIL_HPP_
