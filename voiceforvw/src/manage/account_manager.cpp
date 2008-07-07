/* account_manager.cpp -- account management module
 *
 *			Copyright 2008, 3di.jp Inc
 */

#include <main.h>
#include <state.hpp>

string AccountManager::create() {

	AccountInfo *info = NULL;
	string ret;

	try {
		info = new AccountInfo();
		
		ret = registHandle(info);

		VFVW_LOG("created AccountInfo handle=%s", info->handle.c_str());
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

AccountInfo* AccountManager::find(const string& handle) {
	return (AccountInfo *)findBase(handle);
}

