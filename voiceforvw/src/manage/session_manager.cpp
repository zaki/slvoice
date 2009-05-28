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

		g_logger->Info() << "Created SessionInfo handle=" << info->handle << endl;
	}
	catch (exception e) 
	{
		g_logger->Fatal() << "SessionManager::create Error " << e.what() << endl;

		if (info != NULL) {
			delete info;
		}
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
	catch (exception e) 
	{
		g_logger->Fatal() << "SessionManager::controlAudioLevel Error " << e.what() << endl;
		throw e;
	}
}

