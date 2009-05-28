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

		g_logger->Info("ACCOUNTMANAGER::Create") << "Created AccountInfo handle=" << info->handle << endl;
	}
	catch (exception e) {

		g_logger->Fatal("ACCOUNTMANAGER::Create") << e.what() << endl;

		if (info != NULL) {
			delete info;
		}
		throw e;
	}

	return ret;
}

AccountInfo* AccountManager::find(const string& handle) {
	return (AccountInfo *)findBase(handle);
}

