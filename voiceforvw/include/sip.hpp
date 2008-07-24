/* sip.hpp -- sip definition
 *
 *			Copyright 2008, 3di.jp Inc
 */

#ifndef _SIP_HPP
#define _SIP_HPP

#include <pjsua-lib/pjsua.h>

#define VFVW_REALM	"asterisk"

//=============================================================================
// Voice classes

struct SIPUserInfo
{
    SIPUserInfo () {}
    SIPUserInfo (const string& n, const string& d) 
        : name (n), domain (d) {
		sipuri = "sip:" + name + "@" + domain;
	}

    string name;
    string domain;
    string password;
	string sipuri;
};


struct SIPServerInfo
{
	string sipuri;
	string proxyuri;
	string reguri;
};

class SIPConference
{
    public:
        typedef list <SIPUserInfo> SIPUserList;

        SIPConference (const SIPServerInfo&);
        ~SIPConference(); 

        void Register(const SIPUserInfo&, int*); 
        void UnRegister(const int); 

		void Join(const string&, int, int*);
		void Answer(const int, const unsigned int);
        void Leave(const int);

		void AdjustTranVolume(pjsua_call_id, float);
		void AdjustRecvVolume(pjsua_call_id, float);

    private:
        void start_sip_stack_(); 
        void stop_sip_stack_(); 

    private:
        SIPServerInfo server_;
    
    private:
        SIPConference (const SIPConference&);
        void operator= (const SIPConference&);
};

istream& operator>> (istream&, SIPUserInfo&);
stringstream& operator>> (stringstream&, SIPUserInfo&);

#endif //_SIP_HPP
