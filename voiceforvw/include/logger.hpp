/* config.h -- exported function header
 *
 *			Copyright 2009, 3di.jp Inc
 */

#ifndef _LOGGER_H_
#define _LOGGER_H_

//=============================================================================
// Logger class

enum LogLevels 
{
	LL_DEBUG, LL_INFO, LL_TERSE, LL_WARN, LL_ERROR, LL_FATAL, LL_LEVELCOUNT
};

static const string LogLevelStrings[] = 
{
	"DEBUG", 
	"INFO",
	"TERSE",
	"WARN", 
	"ERROR", 
	"FATAL"
};

struct nullstream : std::ostream
{
	struct nullbuf : std::streambuf 
	{
		int overflow(int c) { return traits_type::not_eof(c); }
	} m_sbuf;
	nullstream() 
		: std::ios(&m_sbuf), std::ostream(&m_sbuf) {}
};


class Logger
{
    public:
        Logger ()
			: logLevel(LL_TERSE), outputFilePath("")
		{};

        ~Logger ();

	public:
		void Init();
		void Close();
		ostream& Log(LogLevels=LL_DEBUG, string section = "");

		ostream& Debug(string section = "");
		ostream& Info(string section  = "");
		ostream& Terse(string section  = "");
		ostream& Warn(string section  = "");
		ostream& Error(string section = "");
		ostream& Fatal(string section = "");

	private:
		string outputFilePath;
		std::ofstream file;
		LogLevels logLevel;
		string filter;
		const nullstream ns;
};

#endif