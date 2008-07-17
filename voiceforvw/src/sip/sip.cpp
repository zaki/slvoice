/* sip.cpp -- sip module
 *
 *			Copyright 2008, 3di.jp Inc
 */

#include <main.h>
#include <sip.hpp>

//=============================================================================
/* Custom log function */
static void my_pj_log_ (int level, const char *data, int len) {
    static ofstream log_ ("my_pj_log.txt");

    if (!log_) throw runtime_error ("unable to write to log file");
    if (level < pj_log_get_level()) log_ << data << endl;
}

//=============================================================================
/* Callback called by the library upon receiving incoming call */
static void on_incoming_call (pjsua_acc_id acc_id, pjsua_call_id call_id,
                              pjsip_rx_data *rdata) {

	pj_status_t status;
	pjsua_call_info ci;

    status = pjsua_call_get_info(call_id, &ci);

	VFVW_LOG("Incoming call from %s", ci.remote_info.ptr);

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

    VFVW_LOG("Call %d state=%s", call_id, ci.state_text.ptr);

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

    VFVW_LOG("Media state=%d (callid=%d)", ci.media_status, call_id);

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

    VFVW_LOG("Account %d state=%s", acc_id, ai.status_text.ptr);

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
static void error_exit (const char *title, pj_status_t status) {
    VFVW_LOG("pjsip error [%s, %d]", title, status);

    pjsua_perror ("voice app", title, status);
    pjsua_destroy ();
    exit (1);
}

//=============================================================================
SIPConference::SIPConference(const SIPServerInfo& s) :
        server_ (s) {
    VFVW_LOG("entering SIPConference(const SIPServerInfo&)");
    start_sip_stack_();
}

//=============================================================================
SIPConference::~SIPConference() {
    VFVW_LOG("entering ~SIPConference()");
    stop_sip_stack_();
}

//=============================================================================
void SIPConference::Register(const SIPUserInfo& user, int* accid) {

	pj_status_t status;
    pjsua_acc_config cfg;

	VFVW_LOG("entering Register(const SIPUserInfo&)");

    string temp_useruri(user.get_uri());
    string temp_username(user.name);
    string temp_userpasswd(user.password);
    string temp_serverreguri(server_.get_reg_uri());
    string temp_serverdomain(server_.domain);

	VFVW_LOG("temp_useruri = %s", temp_useruri.c_str());
	VFVW_LOG("temp_username = %s", temp_username.c_str());
	VFVW_LOG("temp_userpasswd = %s", temp_userpasswd.c_str());
	VFVW_LOG("temp_serverreguri = %s", temp_serverreguri.c_str());
	VFVW_LOG("temp_serverdomain = %s", temp_serverdomain.c_str());

    pjsua_acc_config_default (&cfg);

    cfg.id = pj_str (const_cast <char*> (temp_useruri.c_str()));

    // don't send REGISTER
    cfg.reg_uri = pj_str (const_cast <char*> (temp_serverreguri.c_str()));
//    cfg.reg_uri = pj_str("");

    cfg.cred_count = 1;
    cfg.cred_info[0].scheme = pj_str ("digest");
    cfg.cred_info[0].data_type = PJSIP_CRED_DATA_PLAIN_PASSWD;
//    cfg.cred_info[0].realm = pj_str (const_cast <char*> (temp_serverdomain.c_str()));
    cfg.cred_info[0].realm = pj_str (VFVW_REALM);
    cfg.cred_info[0].username = pj_str (const_cast <char*> (temp_username.c_str()));
    cfg.cred_info[0].data = pj_str (const_cast <char*> (temp_userpasswd.c_str()));

    status = pjsua_acc_add(&cfg, PJ_TRUE, (pjsua_acc_id*)accid);

    if (status != PJ_SUCCESS)
        error_exit ("Error adding account", status);
}

//=============================================================================
void SIPConference::UnRegister(const int accid) {

	pj_status_t status;

	VFVW_LOG("entering UnRegister(const int)");

    status = pjsua_acc_del((pjsua_acc_id)accid);

    if (status != PJ_SUCCESS)
        error_exit ("Error deleting account", status);
}

//=============================================================================
void SIPConference::Join(const SIPUserInfo& joinuri, int acc_id, int* callid) {

	pj_status_t status;
	SIPUserInfo uriinfo = joinuri;

	string temp;

	VFVW_LOG("entering Join() uri=%s", uriinfo.get_uri().c_str());

	uriinfo.domain = resolveDomain(uriinfo.domain);

	temp = uriinfo.get_uri();

	VFVW_LOG("uri=%s", temp.c_str());

	pj_str_t uri = pj_str(const_cast <char*> (temp.c_str()));

	// Set prefered codec - Zaki [2008-07-16]
    	string codec;
	string disable_codecs;
	ifstream file("slvoice.ini");
	if (!file) throw runtime_error ("unable to open slvoice.ini file");
	file >> codec;
	if (file.eof())
	{
		VFVW_LOG("Codec was not set via the ini file");
		file.close();
	}
	else
	{
		file >> codec;

		bool disable = false;
		if (!file.eof())
		{
			file >> disable_codecs;
			disable = (disable_codecs == "disable");
		}
		file.close();

		if (disable && codec != "")
		{
			unsigned int count;
			pjsua_codec_info id[256];

			status = pjsua_enum_codecs(id, &count);
			if (status != PJ_SUCCESS)
				error_exit("Error enumerating codecs", status);

			VFVW_LOG("Disabling %d unselected codecs", count - 1);

			for (unsigned int i = 0; i < count; i++)
			{
				pjsua_codec_set_priority(&(id[i].codec_id), 0);
			}
		}

		VFVW_LOG("codec selected = %s", codec.c_str());

		// Set the priority of the selected codec to be highest (255)
		const pj_str_t codec_name = pj_str(const_cast <char*>(codec.c_str()));
		status = pjsua_codec_set_priority(&codec_name, 255);
		if (status != PJ_SUCCESS)
		{
			VFVW_LOG("Selected codec could not be set as default");
		}
		else
		{
			VFVW_LOG("Codec %s was set as default", codec.c_str());
		}
	}

	status = pjsua_call_make_call(
		acc_id, &uri, 0, NULL, NULL, (pjsua_call_id*)callid);

	if (status != PJ_SUCCESS)
		error_exit ("Error making call", status);
}

//=============================================================================
void SIPConference::Answer(const int call_id, const unsigned int status_code) {

	pj_status_t status;

	VFVW_LOG("entering Answer call_id=%d", call_id);

    status = pjsua_call_answer((pjsua_call_id)call_id, status_code, NULL, NULL);

    if (status != PJ_SUCCESS)
        error_exit ("Error answering", status);
}

//=============================================================================
void SIPConference::Leave(const int call_id) {

	pj_status_t status;
	
	VFVW_LOG("entering Leave call_id=%d", call_id);

//    pjsua_call_hangup_all();
	status = pjsua_call_hangup((pjsua_call_id)call_id, 0, NULL, NULL);

    if (status != PJ_SUCCESS)
        error_exit ("Error hanging up", status);
}

//=============================================================================
void SIPConference::AdjustTranVolume(int call_id, float level) {
    VFVW_LOG("entering AdjustTranVolume()");

	pj_status_t status;
    pjsua_call_info ci;
    pjsua_call_get_info((pjsua_call_id)call_id, &ci);

    VFVW_LOG("AdjustTranVolume call_id=%d,level=%f", call_id, level);

#ifdef DEBUG
    unsigned tx_level = 0;
    unsigned rx_level = 0;
    status = pjsua_conf_get_signal_level(ci.conf_slot, &tx_level, &rx_level);
    if (status != PJ_SUCCESS)
        error_exit ("Error get signal level", status);

    VFVW_LOG("current tx_level=%d,rx_level=%d", tx_level, rx_level);
#endif

    status = pjsua_conf_adjust_tx_level(ci.conf_slot, level);
    if (status != PJ_SUCCESS)
        error_exit ("Error adjust tx level", status);
}

//=============================================================================
void SIPConference::AdjustRecvVolume(int call_id, float level) {
    VFVW_LOG("entering AdjustRecvVolume()");

	pj_status_t status;
    pjsua_call_info ci;
    pjsua_call_get_info((pjsua_call_id)call_id, &ci);

    VFVW_LOG("AdjustRecvVolume call_id=%d,level=%f", call_id, level);

#ifdef DEBUG
    unsigned tx_level = 0;
    unsigned rx_level = 0;
    status = pjsua_conf_get_signal_level(ci.conf_slot, &tx_level, &rx_level);
    if (status != PJ_SUCCESS)
        error_exit ("Error get signal level", status);

    VFVW_LOG("current tx_level=%d,rx_level=%d", tx_level, rx_level);
#endif

    status = pjsua_conf_adjust_rx_level(ci.conf_slot, level);
    if (status != PJ_SUCCESS)
        error_exit ("Error adjust rx level", status);
}

//=============================================================================
void SIPConference::start_sip_stack_ () {
    VFVW_LOG("entering start_sip_stack_()");

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
void SIPConference::stop_sip_stack_ () {
    VFVW_LOG("entering stop_sip_stack_()");
    pjsua_destroy ();
}

string SIPConference::resolveDomain(const string& domain) {

	if (domain == "bhr.vivox.com") {
		return server_.domain;
	}
	else {
		return domain;
	}
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
istream& operator>> (istream& in, SIPServerInfo& srv) {
    string line (get_line_ (in));
    pair <string,string> result (parse_sip_uri_ (line));

//    srv.conference = result.first;
    srv.domain = result.second;

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

//=============================================================================
stringstream& operator>> (stringstream& in, SIPServerInfo& srv) {
    string line = in.str();
    pair <string,string> result (parse_sip_uri_ (line));

//    srv.conference = result.first;
    srv.domain = result.second;

    return in;
}

