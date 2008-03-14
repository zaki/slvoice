/* messaging.cpp -- messaging module
 *
 *			Ryan McDougall -- 2008
 */

#include <main.h>
#include <sip.hpp>
#include <fstream>


//=============================================================================
/* Custom log function */
static void my_pj_log_ (int level, const char *data, int len)
{
    static ofstream log_ ("my_pj_log.txt");

    if (!log_) throw std::runtime_error ("unable to write to log file");
    if (level < pj_log_get_level()) log_ << data << endl;
}

//=============================================================================
/* Callback called by the library upon receiving incoming call */
static void on_incoming_call (pjsua_acc_id acc_id, pjsua_call_id call_id,
        pjsip_rx_data *rdata)
{
    pjsua_call_info ci;
    pjsua_call_get_info (call_id, &ci);

    cout << "Incoming call from " 
        << ci.remote_info.slen << ci.remote_info.ptr << endl;

    /* Automatically answer incoming calls with 200/OK */
    pjsua_call_answer (call_id, 200, NULL, NULL);
}

//=============================================================================
/* Callback called by the library when call's state has changed */
static void on_call_state (pjsua_call_id call_id, pjsip_event *e)
{
    pjsua_call_info ci;
    pjsua_call_get_info (call_id, &ci);

    cout << "Call " << call_id << "state= " 
        << ci.state_text.slen << ci.state_text.ptr << endl;
}

//=============================================================================
/* Callback called by the library when call's media state has changed */
static void on_call_media_state (pjsua_call_id call_id)
{
    pjsua_call_info ci;
    pjsua_call_get_info (call_id, &ci);

    if (ci.media_status == PJSUA_CALL_MEDIA_ACTIVE) 
    {
        pjsua_conf_connect (ci.conf_slot, 0);
        pjsua_conf_connect (0, ci.conf_slot);
    }
}

//=============================================================================
/* Display error and exit application */
static void error_exit (const char *title, pj_status_t status)
{
    pjsua_perror ("voice app", title, status);
    pjsua_destroy ();
    exit (1);
}

//=============================================================================
SIPConference::SIPConference () 
{ 
    start_sip_stack_(); 
}

//=============================================================================
SIPConference::SIPConference (const SIPServerInfo& s)
: server_ (s)
{ 
    start_sip_stack_(); 
}

//=============================================================================
SIPConference::~SIPConference () 
{ 
    stop_sip_stack_(); 
}

//=============================================================================
void SIPConference::Register (const SIPUserInfo& user)
{
    user_ = user;

    string temp_useruri (user_.get_uri());
    string temp_username (user_.name);
    string temp_userpasswd (user_.password);
    string temp_serverreguri (server_.get_reg_uri());
    string temp_serverdomain (server_.domain);

    pjsua_acc_config cfg;
    pjsua_acc_config_default (&cfg);

    cfg.id = pj_str (const_cast <char*> (temp_useruri.c_str()));
    cfg.reg_uri = pj_str (const_cast <char*> (temp_serverreguri.c_str()));

    cfg.cred_count = 1;
    cfg.cred_info[0].scheme = pj_str ("digest");
    cfg.cred_info[0].data_type = PJSIP_CRED_DATA_PLAIN_PASSWD;
    cfg.cred_info[0].realm = pj_str (const_cast <char*> (temp_serverdomain.c_str()));
    cfg.cred_info[0].username = pj_str (const_cast <char*> (temp_username.c_str()));
    cfg.cred_info[0].data = pj_str (const_cast <char*> (temp_userpasswd.c_str()));

    status = pjsua_acc_add (&cfg, PJ_TRUE, &acc_id);
    if (status != PJ_SUCCESS) 
        error_exit ("Error adding account", status);
}

//=============================================================================
void SIPConference::Join () 
{ 
    string temp_serveruri (server_.get_conf_uri());

    pj_str_t uri = pj_str (const_cast <char*> (temp_serveruri.c_str()));

    status = pjsua_call_make_call (acc_id, &uri, 0, NULL, NULL, NULL);
    if (status != PJ_SUCCESS) 
        error_exit ("Error making call", status);
}

//=============================================================================
void SIPConference::Leave () 
{ 
    pjsua_call_hangup_all(); 
}

//=============================================================================
void SIPConference::start_sip_stack_ ()
{
    status = pjsua_create ();
    if (status != PJ_SUCCESS) 
        error_exit ("Error in pjsua_create()", status);
    
    pj_log_set_log_func (my_pj_log_);
    
    pjsua_config cfg;
    pjsua_config_default (&cfg);

    cfg.cb.on_incoming_call = &on_incoming_call;
    cfg.cb.on_call_media_state = &on_call_media_state;
    cfg.cb.on_call_state = &on_call_state;

    status = pjsua_init (&cfg, NULL, NULL);
    if (status != PJ_SUCCESS) 
        error_exit ("Error in pjsua_init()", status);

    pjsua_transport_config tcfg;
    pjsua_transport_config_default (&tcfg);

    tcfg.port = 6060;
    status = pjsua_transport_create (PJSIP_TRANSPORT_UDP, &tcfg, NULL);
    if (status != PJ_SUCCESS) 
        error_exit ("Error creating transport", status);

    status = pjsua_start ();
    if (status != PJ_SUCCESS) 
        error_exit ("Error starting pjsua", status);
} 

//=============================================================================
void SIPConference::stop_sip_stack_ () 
{ 
    pjsua_destroy (); 
}

