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
        : name (n), domain (d) {}

    string name;
    string domain;
    string password;

    string get_uri () const { return "sip:" + name + "@" + domain; }
};


struct SIPServerInfo
{
    SIPServerInfo () {}
//	SIPServerInfo (const string& c, const string& d) 
//        : conference (c), domain (d) {}

//    string conference;
    string domain;
    
//    string get_conf_uri () const { return "sip:" + conference + "@" + domain; }
    string get_reg_uri () const { return "sip:" + domain; }
};

class SIPConference
{
    public:
        typedef list <SIPUserInfo> SIPUserList;

        SIPConference (const SIPServerInfo&);
        ~SIPConference(); 

        void Register(const SIPUserInfo&, int*); 
        void UnRegister(const int); 

		void Join(const SIPUserInfo&, int, int*);
		void Answer(const int, const unsigned int);
        void Leave(const int);

		void AdjustTranVolume(pjsua_call_id, float);
		void AdjustRecvVolume(pjsua_call_id, float);

		string resolveDomain(const string&);

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
istream& operator>> (istream&, SIPServerInfo&);

stringstream& operator>> (stringstream&, SIPUserInfo&);
stringstream& operator>> (stringstream&, SIPServerInfo&);

#endif //_SIP_HPP
