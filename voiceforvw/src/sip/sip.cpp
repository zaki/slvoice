/* sip.cpp -- sip module
 *
 *			Copyright 2008, 3di.jp Inc
 */

#include <main.h>
#include <sip.hpp>

//=============================================================================
/* Custom log function */
static void my_pj_log_ (int level, const char *data, int len) 
{
	if (level < pj_log_get_level())
		g_logger->Debug("PjSIP") << data << endl;
	/*
    static ofstream log_ ("my_pj_log.txt");

    if (!log_) throw runtime_error ("unable to write to log file");
    if (level < pj_log_get_level()) log_ << data << endl;
	*/
}

//=============================================================================
/* Callback called by the library upon receiving incoming call */
static void on_incoming_call (pjsua_acc_id acc_id, pjsua_call_id call_id,
                              pjsip_rx_data *rdata) {

	pj_status_t status;
	pjsua_call_info ci;

    status = pjsua_call_get_info(call_id, &ci);

	g_logger->Info() << "Incoming call from " << ci.remote_info.ptr << endl;

	SessionEvent *ev = new DialIncomingEvent();

	ev->uri = ci.remote_info.ptr;
	ev->acc_id = (int)acc_id;
	ev->call_id = (int)call_id;

	g_eventManager.blockQueue.enqueue(ev);
}

//=============================================================================
/* Callback called by the library when call's state has changed */
static void on_call_state (pjsua_call_id call_id, pjsip_event *e) {

	pj_status_t status;
    pjsua_call_info ci;
	string handle;

	SessionEvent *ev = NULL;

    status = pjsua_call_get_info(call_id, &ci);

	g_logger->Info() << "Call " << call_id << " state=" << ci.state_text.ptr << endl;

    /*PJSIP_INV_STATE_NULL 	Before INVITE is sent or received
      PJSIP_INV_STATE_CALLING 	After INVITE is sent
      PJSIP_INV_STATE_INCOMING 	After INVITE is received.
      PJSIP_INV_STATE_EARLY 	After response with To tag.
      PJSIP_INV_STATE_CONNECTING 	After 2xx is sent/received.
      PJSIP_INV_STATE_CONFIRMED 	After ACK is sent/received.
      PJSIP_INV_STATE_DISCONNECTED 	Session is terminated.*/

    switch (ci.state) {

    case PJSIP_INV_STATE_EARLY: {
        // After response with To tag
		/*
        DialEarlyEvent ev;
        info->machine.process_event(ev);
		*/
		ev = new DialEarlyEvent();
    }
    break;
    case PJSIP_INV_STATE_CONNECTING: {
        // After response with To tag
		ev = new DialConnectingEvent();
    }
    break;
    case PJSIP_INV_STATE_CONFIRMED: {
        // After response with To tag
		ev = new DialSucceedEvent();
    }
    break;
    case PJSIP_INV_STATE_DISCONNECTED: {
        // After response with To tag
		ev = new DialDisconnectedEvent();
    }
    break;
    default:
        break;
    }

	if (ev != NULL) {
		ev->call_id = (int)call_id;
		g_eventManager.blockQueue.enqueue(ev);
	}
}

//=============================================================================
/* Callback called by the library when call's media state has changed */
static void on_call_media_state(pjsua_call_id call_id) {

	pj_status_t status;
    pjsua_call_info ci;
    pjsua_call_get_info (call_id, &ci);

	g_logger->Info() << "Media state= " << ci.media_status << " Callid = " << call_id << endl;

    if (ci.media_status == PJSUA_CALL_MEDIA_ACTIVE) {
        status = pjsua_conf_connect(ci.conf_slot, 0);
        status = pjsua_conf_connect(0, ci.conf_slot);
    }
}

//=============================================================================
/* Callback called by the library when register's state has changed */
static void on_reg_state(pjsua_acc_id acc_id) {

	pj_status_t status;
    pjsua_acc_info ai;
	string handle;
	AccountEvent *ev = NULL;

    status = pjsua_acc_get_info(acc_id, &ai);

	g_logger->Info() << "Account " << acc_id << " state=" << ai.status_text.ptr << endl;

	switch (ai.status / 100) {
    case 1:
        break;
    case 2: {
        // 2xx
		ev = new RegSucceedEvent();
    }
    break;
    case 3:
    case 4:
    case 5:
    case 6: {
        // 3xx-6xx
		ev = new RegFailedEvent();
    }
    break;
    default:
        break;
    }

	if (ev != NULL) {
		ev->acc_id = (int)acc_id;
		g_eventManager.blockQueue.enqueue(ev);
	}
}

//=============================================================================
/* Display error and exit application */
static void error_exit (const char *title, pj_status_t status) 
{
	g_logger->Fatal() << "PjSIP Error " << title << "," << status << endl;

    pjsua_perror ("voice app", title, status);
    pjsua_destroy ();
    exit (1);
}

//=============================================================================
SIPConference::SIPConference(const SIPServerInfo& s) :
        server_ (s) 
{
	g_logger->Debug() << "Entering SIPConference(const SIPServerInfo&)" << endl;
    start_sip_stack_();
}

//=============================================================================
SIPConference::~SIPConference() 
{
    g_logger->Debug() << "Entering SIPConference()" << endl;
    stop_sip_stack_();
}

//=============================================================================
void SIPConference::Register(const SIPUserInfo& user, int* accid) {

	pj_status_t status;
    pjsua_acc_config cfg;

	g_logger->Debug() << "Entering Register(const SIPUserInfo&)" << endl;

    string temp_useruri(user.sipuri);
    string temp_username(user.name);
    string temp_userpasswd(user.password);
    string temp_serverreguri(server_.reguri);

	g_logger->Info() << "temp_useruri      = " << temp_useruri << endl;
	g_logger->Info() << "temp_username     = " << temp_username << endl;
	g_logger->Info() << "temp_userpasswd   = " << temp_userpasswd << endl;
	g_logger->Info() << "temp_serverreguri = " << temp_serverreguri << endl;

	pjsua_acc_config_default (&cfg);

    cfg.id = pj_str (const_cast <char*> (temp_useruri.c_str()));
	cfg.reg_uri = pj_str (const_cast <char*> (temp_serverreguri.c_str()));
//    cfg.reg_uri = pj_str("");

    cfg.cred_count = 1;
    cfg.cred_info[0].scheme = pj_str ("digest");
    cfg.cred_info[0].data_type = PJSIP_CRED_DATA_PLAIN_PASSWD;
    cfg.cred_info[0].realm = pj_str (const_cast<char*>(g_config->Realm.c_str()));
    cfg.cred_info[0].username = pj_str (const_cast <char*> (temp_username.c_str()));
    cfg.cred_info[0].data = pj_str (const_cast <char*> (temp_userpasswd.c_str()));

    status = pjsua_acc_add(&cfg, PJ_TRUE, (pjsua_acc_id*)accid);

    if (status != PJ_SUCCESS)
        error_exit ("Error adding account", status);
}

//=============================================================================
void SIPConference::UnRegister(const int accid) {

	pj_status_t status;

	g_logger->Debug() << "Entering UnRegister(const int)" << endl;

    status = pjsua_acc_del((pjsua_acc_id)accid);

    if (status != PJ_SUCCESS)
        error_exit ("Error deleting account", status);
}

//=============================================================================
void SIPConference::Join(const string& joinuri, int acc_id, int* callid) {

	pj_status_t status;

	g_logger->Debug() << "Entering Join() URI=" << joinuri << endl;

	pj_str_t uri = pj_str(const_cast <char*> (joinuri.c_str()));

	// Set prefered codec - Zaki [2008-07-16]
    string codec;
	if (g_config->DisableOtherCodecs && g_config->Codec != "")		
	{
		unsigned int count;
		pjsua_codec_info id[256];

		status = pjsua_enum_codecs(id, &count);
		if (status != PJ_SUCCESS)
			error_exit("Error enumerating codecs", status);

		g_logger->Info() << "Disabling " << count-1 << " unselected codecs" << endl;

		for (unsigned int i = 0; i < count; i++)
		{
			pjsua_codec_set_priority(&(id[i].codec_id), 0);
		}
	}

	g_logger->Info() << "Codec selected " << g_config->Codec << endl;

	// Set the priority of the selected codec to be highest (255)
	const pj_str_t codec_name = pj_str(const_cast <char*>(g_config->Codec.c_str()));
	status = pjsua_codec_set_priority(&codec_name, 255);
	if (status != PJ_SUCCESS)
	{
		g_logger->Warn() << "Selected codec " << g_config->Codec << " could not be set as default" << endl;
	}
	else
	{
		g_logger->Info() << "Codec " << g_config->Codec << " was set as default" << endl;
	}

	status = pjsua_call_make_call(
		acc_id, &uri, 0, NULL, NULL, (pjsua_call_id*)callid);

	if (status != PJ_SUCCESS)
		error_exit ("Error making call", status);
}

//=============================================================================
void SIPConference::Answer(const int call_id, const unsigned int status_code) {

	pj_status_t status;

	g_logger->Info() << "Entering Answer call_id=" << call_id << endl;

    status = pjsua_call_answer((pjsua_call_id)call_id, status_code, NULL, NULL);

    if (status != PJ_SUCCESS)
        error_exit ("Error answering", status);
}

//=============================================================================
void SIPConference::Leave(const int call_id) {

	pj_status_t status;
	
	g_logger->Info() << "Entering Leave call_id=" << call_id << endl;

//    pjsua_call_hangup_all();
	status = pjsua_call_hangup((pjsua_call_id)call_id, 0, NULL, NULL);

    if (status != PJ_SUCCESS)
        error_exit ("Error hanging up", status);
}

//=============================================================================
void SIPConference::AdjustTranVolume(int call_id, float level) 
{
	g_logger->Debug() << "Entering AdjustTranVolume()" << endl;

	pj_status_t status;
    pjsua_call_info ci;
    pjsua_call_get_info((pjsua_call_id)call_id, &ci);

	g_logger->Info() << "AdjustTranVolume call_id" << call_id << ",Level=" << level << endl;

#ifdef DEBUG
    unsigned tx_level = 0;
    unsigned rx_level = 0;
    status = pjsua_conf_get_signal_level(ci.conf_slot, &tx_level, &rx_level);
    if (status != PJ_SUCCESS)
        error_exit ("Error get signal level", status);

    g_logger->Info() << "Current tx_level" << tx_level << ", rx_level=" << rx_level << endl;
#endif

    status = pjsua_conf_adjust_tx_level(ci.conf_slot, level);
    if (status != PJ_SUCCESS)
        error_exit ("Error adjust tx level", status);
}

//=============================================================================
void SIPConference::AdjustRecvVolume(int call_id, float level) 
{
	g_logger->Debug() << "Entering AdjustRecvVolume()" << endl;

	pj_status_t status;
    pjsua_call_info ci;
    pjsua_call_get_info((pjsua_call_id)call_id, &ci);

	g_logger->Info() << "AdjustRecvVolume call_id=" << call_id << ", level=" << level << endl;

#ifdef DEBUG
    unsigned tx_level = 0;
    unsigned rx_level = 0;
    status = pjsua_conf_get_signal_level(ci.conf_slot, &tx_level, &rx_level);
    if (status != PJ_SUCCESS)
        error_exit ("Error get signal level", status);

	g_logger->Info() << "Current tx_level" << tx_level << ", rx_level=" << rx_level << endl;
#endif

    status = pjsua_conf_adjust_rx_level(ci.conf_slot, level);
    if (status != PJ_SUCCESS)
        error_exit ("Error adjust rx level", status);
}

//=============================================================================
void SIPConference::start_sip_stack_() 
{
	g_logger->Debug() << "Entering start_sip_stack_()" << endl;

	pj_status_t status;

    status = pjsua_create ();
    if (status != PJ_SUCCESS)
        error_exit ("Error in pjsua_create()", status);

    pj_log_set_log_func (my_pj_log_);

    pjsua_media_config mcfg;
    pjsua_media_config_default(&mcfg);
    mcfg.ilbc_mode = 30;

    pjsua_config cfg;
    pjsua_config_default (&cfg);

    cfg.cb.on_incoming_call = &on_incoming_call;
    cfg.cb.on_call_media_state = &on_call_media_state;
    cfg.cb.on_call_state = &on_call_state;
    //cfg.cb.on_call_transfer_request =	// for REFER
    cfg.cb.on_reg_state = &on_reg_state;

	if (server_.proxyuri != "") {
		cfg.outbound_proxy_cnt = 1;
		cfg.outbound_proxy[0] = pj_str(const_cast<char*>(server_.proxyuri.c_str()));
	}

    status = pjsua_init (&cfg, NULL, &mcfg);
    if (status != PJ_SUCCESS)
        error_exit ("Error in pjsua_init()", status);

    pjsua_transport_config tcfg;
    pjsua_transport_config_default (&tcfg);

    tcfg.port = 5060;
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
	g_logger->Debug() << "Entering stop_sip_stack_()" << endl;
    pjsua_destroy ();
}

//=============================================================================
static string get_line_ (istream& in) {
    string line;

    ios_base::fmtflags ff (in.flags());
    in.setf (ios_base::skipws);
    getline (in, line);
    in.flags (ff);

    return line;
}

static string take_after_ (string pre, string s) {
    size_t p (s.find (pre));
    return (p != string::npos)? s.substr (p + pre.size()) : string();
}

static string take_before_ (string pre, string s) {
    size_t p (s.find (pre));
    return (p != string::npos)? s.substr (0, p) : s;
}

static pair <string,string> parse_sip_uri_ (string uri) {
    static const string sip_ ("sip:");
    static const string at_ ("@");

    string addr (take_after_ (sip_, uri));
    string target (take_before_ (at_, addr));
    string domain (take_after_ (at_, addr));

    return make_pair (target, domain);
}

//=============================================================================
istream& operator>> (istream& in, SIPUserInfo& usr) {
    string line (get_line_ (in));
    pair <string,string> result (parse_sip_uri_ (line));

    usr.name = result.first;
    usr.domain = result.second;

    return in;
}

//=============================================================================
stringstream& operator>> (stringstream& in, SIPUserInfo& usr) {
    string line = in.str();
    pair <string,string> result (parse_sip_uri_ (line));

    usr.name = result.first;
    usr.domain = result.second;

    return in;
}

