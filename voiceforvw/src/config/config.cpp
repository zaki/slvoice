/* config.cpp -- config module
 *
 *			Copyright 2009, 3di.jp Inc
 */

#include "main.h"
#include "config.hpp"

void Config::LoadConfig(string configFilePath)
{
	ConfigFilePath = configFilePath;
	doc_.LoadFile(ConfigFilePath);

	string value;
	// Port
	value = get_value("Port");
	if (value != "")
	{
		Port = atoi(value.c_str());
		VFVW_LOG("Port: %d", Port);
	}

	// Version
	value = get_value("Version");
	if (value != "")
	{
		Version = atoi(value.c_str());
		VFVW_LOG("Version: %d", Version);
	}

	// Realm
	value = get_value("VoiceServerURI");
	if (value != "")
	{
		VoiceServerURI = value;
		VFVW_LOG("VoiceServerURI: %s", VoiceServerURI.c_str());
	}

	// Realm
	value = get_value("Realm");
	if (value != "")
	{
		Realm = value;
		VFVW_LOG("Realm: %s", Realm.c_str());
	}

	// Codec
	value = get_value("Codec");
	if (value != "")
	{
		Codec = value;
		VFVW_LOG("Codec: %s", Codec.c_str());
	}

	// Disable other codecs
	value = get_value("Disable");
	DisableOtherCodecs = (value.compare("true") == 0);
	VFVW_LOG("Disable: %s", value.c_str());
}

//=============================================================================
string Config::get_value(const string& name)
{
    return get_value_ (doc_.RootElement(), name);
}

string Config::get_value_(const TiXmlElement *e, const string& name)
{
    const TiXmlElement *p(e->FirstChildElement(name));
	if (p) return string(p->GetText());
    else
        return(string(""));
}