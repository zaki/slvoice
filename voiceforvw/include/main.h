/* main.h -- exported function header
 *
 *			Copyright 2008, 3di.jp Inc
 */

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


#endif //_MAIN_H_
