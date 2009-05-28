/* base_manager.cpp -- manager base module
 *
 *			Copyright 2008, 3di.jp Inc
 */

#include <main.h>
#include <state.hpp>

string BaseManager::registHandle(BaseInfo* baseInfo) {

	string ret;
	stringstream ss;

	try {
		ss << hex << (unsigned int)baseInfo;

		baseInfo->handle = ss.str();
		infos.insert(pair<string, BaseInfo*>(baseInfo->handle, baseInfo));
		ret = baseInfo->handle;

		g_logger->Info("BaseManager") << "Created Info handle=" << baseInfo->handle << endl;
	}
	catch (exception e) 
	{
		g_logger->Fatal("BaseManager") << "Error in registHandle " << e.what() << endl;
		throw e;
	}

	return ret;
}

BaseInfo* BaseManager::findBase(const string& handle) {
	
	BaseInfo *result = NULL;

	map<string, BaseInfo*>::iterator ite;

	try {
		ite = infos.find(handle);

		if (ite != infos.end()) {
			result = ite->second;
		}
	}
	catch (exception e) {
		g_logger->Fatal("BaseManager") << "Error in findBase " << e.what() << endl;
		throw e;
	}
	
	return result;
}

void BaseManager::registId(const int id, const string& handle) {

	try {
		handles.insert(pair<int, string>(id, handle));
	}
	catch (exception e) {
		g_logger->Fatal("BaseManager") << "Error in registId " << e.what() << endl;
		throw e;
	}
}

string BaseManager::convertId(const int id) {
	
	string handle;
	map<int, string>::iterator ite;

	try {
		ite = handles.find(id);

		if (ite != handles.end()) {
			handle = ite->second;
		}
	}
	catch (exception e) {
		g_logger->Fatal("BaseManager") << "Error in convertId " << e.what() << endl;
		throw e;
	}
	
	return handle;
}

void BaseManager::remove(const string& handle) {

	try {
		BaseInfo* info = findBase(handle);

		if (info != NULL) {
			handles.erase(info->id);
			infos.erase(handle);
			delete info;
			g_logger->Info("BaseManager") << "Removed Info handle=" << handle << endl;
		}
	}
	catch (exception e) {
		g_logger->Fatal("BaseManager") << "Error in convertId " << e.what() << endl;
		throw e;
	}
}
