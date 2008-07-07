/* session_manager.cpp -- session management module
 *
 *			Copyright 2008, 3di.jp Inc
 */

#include <main.h>
#include <state.hpp>

string SessionManager::create(const AccountInfo *account) {

	SessionInfo *info = NULL;
	string ret;

	try {
		info = new SessionInfo(account);

		ret = registHandle(info);

		VFVW_LOG("created SessionInfo handle=%s", info->handle.c_str());
	}
	catch (...) {

		VFVW_LOG("Error");

		if (info != NULL) {
			delete info;
		}
		exception e;
		throw e;
	}

	return ret;
}

SessionInfo* SessionManager::find(const string& handle) {
	return (SessionInfo *)findBase(handle);
}

void SessionManager::controlAudioLevel() {

	map<string, BaseInfo*>::iterator ite;
	SessionInfo *info = NULL;
	AudioEvent ev;

	try {
		for (ite = infos.begin(); ite != infos.end(); ite++) {
			info = (SessionInfo *)ite->second;
			info->machine.process_event(ev);
		}
	}
	catch (...) {
		VFVW_LOG("Error");
		exception e;
		throw e;
	}
}

