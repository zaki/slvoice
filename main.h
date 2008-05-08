/* main.h -- exported function header
 *
 *			Ryan McDougall -- 20070101
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

#include <parameters.hpp>
#include <parsing.hpp>
#include <sip.hpp>
#include <state.hpp>
#include <server.hpp>

// global reference to server instance 
// created in main.cpp
extern Server *glb_server;

#endif //_MAIN_H_
