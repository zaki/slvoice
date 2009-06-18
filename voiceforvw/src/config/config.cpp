/* config.cpp -- config module
 *
 *			Copyright 2009, 3di.jp Inc
 */

#include "main.h"
#include "config.hpp"

void Config::LoadConfig(string configFilePath)
{
	ConfigFilePath = configFilePath;
	if (doc_.LoadFile(ConfigFilePath))
	{
		string value;
		// LogFilePath
		value = get_value("LogFilePath");
		if (value != "")
		{
			LogFilePath = value;
		}

		// LogFilter
		value = get_value("LogFilter");
		if (value != "")
		{
			LogFilter = value;
		}

		// LogLevel
		value = get_value("LogLevel");
		if (value != "")
		{
			LogLevel = value;
		}

		// Port
		value = get_value("Port");
		if (value != "")
		{
			Port = atoi(value.c_str());
		}

		// Version
		value = get_value("Version");
		if (value != "")
		{
			Version = atoi(value.c_str());
		}

		// VoiceServerURI
		value = get_value("VoiceServerURI");
		if (value != "")
		{
			VoiceServerURI = value;
		}

		// Realm
		value = get_value("Realm");
		if (value != "")
		{
			Realm = value;
		}

		// Codec
		value = get_value("Codec");
		if (value != "")
		{
			Codec = value;
		}

		// Disable other codecs
		value = get_value("Disable");
		DisableOtherCodecs = (value.compare("true") == 0);
	}
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