/* main.h -- exported function header
 *
 *			Copyright 2008, 3di.jp Inc
 */

#define _3DI

#ifndef _MAIN_H_
#define _MAIN_H_

#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <utility>
#include <list>
#include <vector>

#include <cstdlib>
#include <cstring>

using namespace std;

#include <config.hpp>
#include <logger.hpp>
#include <parameters.hpp>
#include <parsing.hpp>
#include <sip.hpp>
#include <event.hpp>
#include <state.hpp>
#include <server.hpp>

extern Config *g_config;
extern Logger *g_logger;

// global reference to server instance 
// created in main.cpp
extern Server *glb_server;

extern EventManager g_eventManager;

// defines
#define	VFVW_CONNECTOR_HANDLE	"xxxx"

#define VFVW_SL_VOLUME_MIN		(-100)
#define VFVW_SL_VOLUME_MAX		24
#define VFVW_SL_VOLUME_RANGE	(VFVW_SL_VOLUME_MAX - VFVW_SL_VOLUME_MIN)

#define VFVW_PJ_VOLUME_MIN		0
//#define VFVW_PJ_VOLUME_MAX		255
#define VFVW_PJ_VOLUME_MAX		3
#define VFVW_PJ_VOLUME_RANGE	(VFVW_PJ_VOLUME_MAX - VFVW_PJ_VOLUME_MIN)

//=============================================================================
static string get_line_ (istream& in) {
    string line;

    ios_base::fmtflags ff (in.flags());
    in.setf (ios_base::skipws);
    getline (in, line);
    in.flags (ff);

    return line;
}

static string take_after_ (string pre, string s) {
    size_t p (s.find (pre));
    return (p != string::npos)? s.substr (p + pre.size()) : string();
}

static string take_before_ (string pre, string s) {
    size_t p (s.find (pre));
    return (p != string::npos)? s.substr (0, p) : s;
}

static pair <string,string> parse_sip_uri_ (string uri) {
    static const string sip_ ("sip:");
    static const string at_ ("@");

    string addr (take_after_ (sip_, uri));
    string target (take_before_ (at_, addr));
    string domain (take_after_ (at_, addr));

    return make_pair (target, domain);
}

#endif //_MAIN_H_
