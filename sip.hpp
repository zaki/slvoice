/* sip.hpp -- sip definition
 *
 *			Ryan McDougall -- 2008
 */

#ifndef _SIP_HPP
#define _SIP_HPP

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
    SIPServerInfo (const string& c, const string& d) 
        : conference (c), domain (d) {}

    string conference;
    string domain;
    
    string get_uri () const { return "sip:" + conference + "@" + domain; }
};


class SIPConference
{
    public:
        typedef list <SIPUserInfo> SIPUserList;

        SIPConference (); 
        SIPConference (const SIPServerInfo& s);
        ~SIPConference (); 

        void Register (const SIPUserInfo& user); 
        void Join (); 
        void Leave (); 

    private:
        void start_sip_stack_(); 
        void stop_sip_stack_(); 

    private:
        SIPServerInfo server_;
        SIPUserInfo user_;
    
    private:
        pjsua_acc_id acc_id;
        pj_status_t status;

    private:
        SIPConference (const SIPConference&);
        void operator= (const SIPConference&);
};


void on_incoming_call (pjsua_acc_id, pjsua_call_id, pjsip_rx_data *);
void on_call_state (pjsua_call_id, pjsip_event *);
void on_call_media_state (pjsua_call_id);
void error_exit (const char *, pj_status_t);

#endif //_SIP_HPP
