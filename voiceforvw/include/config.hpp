/* config.h -- exported function header
 *
 *			Copyright 2009, 3di.jp Inc
 */

#ifndef _CONFIG_H_
#define _CONFIG_H_

#include "tinyxml/tinyxml.h"

//=============================================================================
// Server class

class Config
{
    public:
        Config ()
			: Port(44215), 
			  ConfigFilePath(""),
			  LogFilePath("SLVoice.log"),
			  LogLevel("WARN"),
			  LogFilter(""),
			  VoiceServerURI(""),
			  Realm("asterisk"),
			  Codec("PCMU"),
			  DisableOtherCodecs(false),
			  Version(122)
		{};

        ~Config ();

		void LoadConfig(string configFilePath);

    public:
		string ConfigFilePath;
		string LogFilePath;
		string LogLevel;
		string LogFilter;
		int Port;					// Port to receive communication through
		string VoiceServerURI;		// Voice server URI to get user's SIP URI from
		string Realm;				// Authentication realm
		string Codec;				// Preferred codec
		bool DisableOtherCodecs;	// true disables all codecs other than the preferred one
		int Version;

	public:
		string get_value (const string& name);
    
	private:
		TiXmlDocument doc_;
		string get_value_ (const TiXmlElement *e, const string& name);
};

#endif