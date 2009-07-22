/* logger.cpp -- logger module
 *
 *			Copyright 2009, 3di.jp Inc
 */

#include "main.h"
#include "logger.hpp"
#include <time.h>
#include <iostream>
#include <fstream>

void Logger::Init()
{
	// output file
	outputFilePath = g_config->LogFilePath;
	file.open(outputFilePath.c_str(), ios::app);

	// log level
	for(int i = 0; i < LL_LEVELCOUNT; i++)
	{
		if (LogLevelStrings[i] == g_config->LogLevel)
		{
			logLevel = (LogLevels)i;
			break;
		}
	}
}

ostream& Logger::Debug(string section) {return(Log(LL_DEBUG, section));}
ostream& Logger::Info(string section) {return(Log(LL_INFO, section));}
ostream& Logger::Terse(string section) {return(Log(LL_TERSE, section));}
ostream& Logger::Warn(string section) {return(Log(LL_WARN, section));}
ostream& Logger::Error(string section) {return(Log(LL_ERROR, section));}
ostream& Logger::Fatal(string section) {return(Log(LL_FATAL, section));}

ostream& Logger::Log(LogLevels level, string section)
{
	if (level < logLevel)
	{
		return((ostream&)ns);
	}

	if (g_config->LogFilter != "" && section != "")
	{
		if (section.find(g_config->LogFilter) != string.npos)
			return((ostream&)ns);
	}

	time_t currTime;
	struct tm* timeinfo;

	time(&currTime);
	timeinfo = localtime(&currTime);
	char timebuffer[20];
	strftime(timebuffer, 20, "%Y-%m-%d %H:%M:%S", timeinfo);
	
	try
	{
		file << timebuffer << " ";
		file << LogLevelStrings[level] << " [" << section << "]: " ;
	}
	catch(...) {/* TODO */}

	return(file);
}

void Logger::Close()
{
	file.close();
}