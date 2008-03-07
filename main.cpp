/* main.cpp -- main module
 *
 *			Ryan McDougall -- 20070101
 */

#include <iostream>
#include <sstream>
#include <list>

#include <cstdlib>
#include <cstring>

#include <tinyxml/tinyxml.h>
#include <sockets/Sockets.h>
#include <pjsua-lib/pjsua.h>

#include <boost/mpl/list.hpp>
#include <boost/statechart/event.hpp>
#include <boost/statechart/custom_reaction.hpp>
#include <boost/statechart/state_machine.hpp>
#include <boost/statechart/simple_state.hpp>

using namespace std;
using namespace boost::statechart;

const int default_port (44124);


//=============================================================================
//
void dump_to_stdout (TiXmlNode* pParent, unsigned int indent = 0);
void print_usage_and_exit (char **argv);
char get_short_option (char *arg);

//=============================================================================
// Message Constants

const string endmesg ("\n\n\n");

const string AuxAudioPropertiesEventString ("AuxAudioPropertiesEvent");
const string LoginStateChangeEventString ("LoginStateChangeEvent");
const string ParticipantPropertiesEventString ("ParticipantPropertiesEvent");
const string ParticipantStateChangeEventString ("ParticipantStateChangeEvent");
const string SessionStateChangeEventString ("SessionStateChangeEvent");

const string AccountLogin1String ("Account.Login.1");
const string AccountLogout1String ("Account.Logout.1");
const string AuxCaptureAudioStart1String ("Aux.CaptureAudioStart.1");
const string AuxCaptureAudioStop1String ("Aux.CaptureAudioStop.1");
const string AuxGetCaptureDevices1String ("Aux.GetCaptureDevices.1");
const string AuxGetRenderDevices1String ("Aux.GetRenderDevices.1");
const string AuxSetCaptureDevice1String ("Aux.SetCaptureDevice.1");
const string AuxSetMicLevel1String ("Aux.SetMicLevel.1");
const string AuxSetRenderDevice1String ("Aux.SetRenderDevice.1");
const string AuxSetSpeakerLevel1String ("Aux.SetSpeakerLevel.1");
const string ConnectorCreate1String ("Connector.Create.1");
const string ConnectorInitiateShutdown1String ("Connector.InitiateShutdown.1");
const string ConnectorMuteLocalMic1String ("Connector.MuteLocalMic.1");
const string ConnectorMuteLocalSpeaker1String ("Connector.MuteLocalSpeaker.1");
const string ConnectorSetLocalMicVolume1String ("Connector.SetLocalMicVolume.1");
const string ConnectorSetLocalSpeakerVolume1String ("Connector.SetLocalSpeakerVolume.1");
const string SessionCreate1String ("Session.Create.1");
const string SessionSet3DPosition1String ("Session.Set3DPosition.1");
const string SessionSetParticipantMuteForMe1String ("Session.SetParticipantMuteForMe.1");
const string SessionSetParticipantVolumeForMe1String ("Session.SetParticipantVolumeForMe.1");
const string SessionTerminate1String ("Session.Terminate.1");

//=============================================================================
// Actions
enum ActionType
{
    None,
    AccountLogin1,
    AccountLogout1,
    AuxCaptureAudioStart1,
    AuxCaptureAudioStop1,
    AuxGetCaptureDevices1,
    AuxGetRenderDevices1,
    AuxSetCaptureDevice1,
    AuxSetMicLevel1,
    AuxSetRenderDevice1,
    AuxSetSpeakerLevel1,
    ConnectorCreate1,
    ConnectorInitiateShutdown1,
    ConnectorMuteLocalMic1,
    ConnectorMuteLocalSpeaker1,
    ConnectorSetLocalMicVolume1,
    ConnectorSetLocalSpeakerVolume1,
    SessionCreate1,
    SessionSet3DPosition1,
    SessionSetParticipantMuteForMe1,
    SessionSetParticipantVolumeForMe1,
    SessionTerminate1
};

ActionType get_action_type (const TiXmlDocument& doc);

//=============================================================================
// Response Objects
struct EventMessage
{
    EventMessage (const string& t) 
        : type (t), status_code (0), state (0) {}

    string type;
    int status_code;
    int state;
};

struct ResponseMessage
{
    ResponseMessage (const string& a) 
        : action (a), return_code (0), status_code (0) {}

    string action;
    string handle;
    string echo;

    int return_code;
    int status_code;
};


const string glb_event_xml ("<Event type=\"\" ><StatusCode /><StatusString /><State /></Event>");
const string glb_response_xml ("<Response requestId=\"\" action=\"\" ><ReturnCode /><Results><StatusCode /><StatusString /></Results><InputXml /></Response>");

string format_response (const ResponseMessage& resp);
string format_response (const EventMessage& ev);

//=============================================================================
// Request Objects
// TODO: these are meant to be filled in when a request is parsed
struct Request 
{
    int sequenceid;
};

struct Account : public Request
{
    string name;
    string password;
    string uri;
    string handle;
    string connector_handle;
};

struct Connection : public Request
{
    string handle;
    string account_server;
};

struct Session : public Request
{
    string name;
    string password;
    string uri;
    string handle;
    string connector_handle;
    string account_handle;
    string hash_algorithm;
};

struct RequestItem
{
    RequestItem (auto_ptr <const Request> p, ActionType t) 
        : params (p), type (t) {}

    auto_ptr <const Request> params;
    ActionType type;
};

typedef list <const RequestItem *> RequestQueue;

auto_ptr <const Request> parse_request (const TiXmlDocument& doc);
auto_ptr <const Request> parse_account_request (const TiXmlDocument& doc);
auto_ptr <const Request> parse_connection_request (const TiXmlDocument& doc);
auto_ptr <const Request> parse_session_request (const TiXmlDocument& doc);

//=============================================================================
// State machine

struct StateMachine;
struct StartState;
struct ConnectorState;
struct AccountState;
struct SessionState;
struct StopState;

struct ViewerEvent { RequestQueue messages; };

struct StartEvent : public ViewerEvent, event <StartEvent> {};
struct AccountEvent : public ViewerEvent, event <AccountEvent> {};
struct ConnectionEvent : public ViewerEvent, event <ConnectionEvent> {};
struct SessionEvent : public ViewerEvent, event <SessionEvent> {};
struct StopEvent : public ViewerEvent, event <StopEvent> {};

//struct MessageReceivedEvent : public event <MessageReceivedEvent> {};

struct StateMachine : state_machine <StateMachine, StartState> 
{
    StateMachine () : socket (NULL) {}
    StateMachine (TCPSocketWrapper *s) : socket (s) {}
    TCPSocketWrapper *socket;
};

struct StartState : simple_state <StartState, StateMachine> 
{
    typedef custom_reaction <ConnectionEvent> reactions;

    StartState () { cout << "start entered" << endl; }
    ~StartState () { cout << "start exited" << endl; }

    result react (const ConnectionEvent& ev) 
    { 
        cout << "start state react" << endl;
        ostringstream mesg;

        ResponseMessage getcap (AuxGetCaptureDevices1String), 
                        getrend (AuxGetRenderDevices1String), 
                        conncreate (ConnectorCreate1String);

        conncreate.handle = "xxxx";

        mesg << format_response (getcap);
        mesg << endmesg;
        mesg << format_response (getrend);
        mesg << endmesg;
        mesg << format_response (conncreate);
        mesg << endmesg;

        context <StateMachine>().socket->
            write (mesg.str().c_str(), mesg.str().size());

        return transit <ConnectorState> ();
    }
};

struct ConnectorState : simple_state <ConnectorState, StateMachine> 
{
    typedef custom_reaction <AccountEvent> reactions;

    ConnectorState () { cout << "connector entered" << endl; }
    ~ConnectorState () { cout << "connector exited" << endl; }

    result react (const AccountEvent& ev) 
    {
        cout << "connector state react" << endl;
        ostringstream mesg;

        ResponseMessage login (AccountLogin1String);
        EventMessage loginstate (LoginStateChangeEventString);
        
        login.handle = "xxxx";
        loginstate.state = 1;

        mesg << format_response (login);
        mesg << endmesg;
        mesg << format_response (loginstate);
        mesg << endmesg;

        context <StateMachine>().socket->
            write (mesg.str().c_str(), mesg.str().size());

        return transit <AccountState> ();
    }
};

struct AccountState : simple_state <AccountState, StateMachine> 
{
    typedef custom_reaction <SessionEvent> reactions;

    AccountState () { cout << "account entered" << endl; }
    ~AccountState () { cout << "account exited" << endl; }

    result react (const SessionEvent& ev) 
    { 
        cout << "account state react" << endl;
        ostringstream mesg;

        ResponseMessage sessioncreate (SessionCreate1String);
        EventMessage sessionstate (SessionStateChangeEventString),
                     partstate (ParticipantStateChangeEventString),
                     partprop (ParticipantPropertiesEventString);

        sessioncreate.handle = "xxxx";
        sessionstate.state = 4;
        partstate.state = 7;

        mesg << format_response (sessioncreate);
        mesg << endmesg;
        mesg << format_response (sessionstate);
        mesg << endmesg;
        mesg << format_response (partstate);
        mesg << endmesg;
        mesg << format_response (partprop);
        mesg << endmesg;

        context <StateMachine>().socket->
            write (mesg.str().c_str(), mesg.str().size());

        return transit <SessionState> ();
    }
};

struct SessionState : simple_state <SessionState, StateMachine> 
{
    typedef custom_reaction <StopEvent> reactions;

    SessionState () 
    { 
        // TODO: this function should echo the current session info
        cout << "session entered" << endl; 
    }
    
    ~SessionState () 
    { 
        ostringstream mesg;

        ResponseMessage sessionterm (SessionTerminate1String);
        EventMessage sessionstate (SessionStateChangeEventString);

        mesg << format_response (sessionterm);
        mesg << endmesg;
        mesg << format_response (sessionstate);
        mesg << endmesg;

        cout << mesg.str() << endl;

        context <StateMachine>().socket->
            write (mesg.str().c_str(), mesg.str().size());

        cout << "session exited" << endl; 
    }

    result react (const StopEvent& ev) { return transit <StopState> (); }
};

struct StopState : simple_state <StopState, StateMachine> 
{
    StopState () 
    { 
        //ostringstream mesg;

        //EventMessage loginstate (LoginStateChangeEventString);
        //ResponseMessage accountlogout (AccountLogout1String),
        //                connshutdown (ConnectorInitiateShutdown1String);

        //mesg << format_response (accountlogout);
        //mesg << endmesg;
        //mesg << format_response (loginstate);
        //mesg << endmesg;
        //mesg << format_response (connshutdown);
        //mesg << endmesg;

        //cout << mesg.str() << endl;

        //context <StateMachine>().socket->
        //    write (mesg.str().c_str(), mesg.str().size());

        cout << "stopped entered" << endl; 
    }
    
    ~StopState () { cout << "stopped exited" << endl; }
};

//=============================================================================
// voice classes
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

/* Callback called by the library when call's state has changed */
static void on_call_state (pjsua_call_id call_id, pjsip_event *e)
{
    pjsua_call_info ci;
    pjsua_call_get_info (call_id, &ci);

    cout << "Call " << call_id << "state= " 
        << ci.state_text.slen << ci.state_text.ptr << endl;
}

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

/* Display error and exit application */
static void error_exit (const char *title, pj_status_t status)
{
    pjsua_perror ("voice app", title, status);
    pjsua_destroy ();
    exit (1);
}

class SIPConference
{
    public:
        typedef list <SIPUserInfo> SIPUserList;

        SIPConference () { start_sip_stack_(); }
        SIPConference (const SIPServerInfo& s) : server_ (s) { start_sip_stack_(); }
        ~SIPConference () { stop_sip_stack_(); }

        bool Register (const SIPServerInfo& serv, const SIPUserInfo& user)
        {
            server_ = serv;

            return Register (user);
        }

        bool Register (const SIPUserInfo& user)
        {
            user_ = user;

            string temp_useruri (user_.get_uri());
            string temp_username (user_.name);
            string temp_userpasswd (user_.password);
            string temp_serveruri (server_.get_uri());
            string temp_serverdomain (server_.domain);
            
            pjsua_acc_config cfg;
            pjsua_acc_config_default (&cfg);

            cfg.id = pj_str (const_cast <char*> (temp_useruri.c_str()));
            cfg.reg_uri = pj_str (const_cast <char*> (temp_serveruri.c_str()));

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

        void Join () 
        { 
            string temp_serveruri (server_.get_uri());

            pj_str_t uri = pj_str (const_cast <char*> (temp_serveruri.c_str()));

            status = pjsua_call_make_call (acc_id, &uri, 0, NULL, NULL, NULL);
            if (status != PJ_SUCCESS) 
                error_exit ("Error making call", status);
        }

        void Leave () { pjsua_call_hangup_all(); }

    private:
        void start_sip_stack_ ()
        {
            status = pjsua_create ();
            if (status != PJ_SUCCESS) 
                error_exit ("Error in pjsua_create()", status);

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

        void stop_sip_stack_ () 
        { 
            pjsua_destroy (); 
        }


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

//=============================================================================
// Server class
class Server
{
    public:
        Server (int port = default_port) 
            : port_ (port), bufsize_ (4096), buf_ (new char [bufsize_])
        {
            server_.listen (port_);
            sock_.reset (new TCPSocketWrapper (server_.accept ()));
            state_.socket = sock_.get();
            state_.initiate ();
        }

        void Start ()
        {
            state_.process_event (StartEvent());

            size_t nread (0);
            for (;;)
            {
                ActionType type;
                TiXmlDocument doc;

                memset (buf_, 0, bufsize_);
                nread = sock_->read (buf_, bufsize_);
                if (nread <= 0) break;
                cout << "received: " << buf_ << endl;

                doc.Parse (buf_);
                type = get_action_type (doc);

                queue_.push_back (new RequestItem (parse_request (doc), type)); 

                issue_events_ (type);
            }
        }

        ~Server () 
        { 
            server_.close();
            sock_->close();

            RequestQueue::iterator i = queue_.begin();
            while (i != queue_.end())
                delete *i++;
            delete buf_;
        }

    private:

        void pop_messages_on_event_ (ViewerEvent& ev)
        {
            ev.messages.splice 
                (ev.messages.end(), 
                 queue_, queue_.begin(), queue_.end());
        }

        void issue_events_ (ActionType type)
        {
            switch (type)
            {
                case AccountLogin1:
                    {
                        AccountEvent ev;
                        pop_messages_on_event_ (ev);
                        state_.process_event (ev);
                    }
                    break;

                case ConnectorCreate1:
                    {
                        ConnectionEvent ev;
                        pop_messages_on_event_ (ev);
                        state_.process_event (ev);
                    }
                    break;

                case SessionCreate1:
                    {
                        SessionEvent ev;
                        pop_messages_on_event_ (ev);
                        state_.process_event (ev);
                    }
                    break;

                case SessionTerminate1:
                    {
                        StopEvent ev;
                        pop_messages_on_event_ (ev);
                        state_.process_event (ev);
                    }
                    break;

                default:
                    {
                        //MessageReceivedEvent ev;
                        //state_.process_event (ev);
                    }
                    break;
            }
        }

    private:
        RequestQueue queue_;
        StateMachine state_;

        TCPSocketWrapper server_;
        auto_ptr <TCPSocketWrapper> sock_;

    private:
        const int port_;
        const size_t bufsize_;
        char *buf_;
    
    private:
        SIPConference bridge_;

    private:
        Server ();
        Server (const Server&);
        void operator= (const Server&);
};

//=============================================================================
// Main entry point
int main (int argc, char **argv)
{
    if ((argc == 2) && (get_short_option (argv[1]) == 'h'))
        print_usage_and_exit (argv);

    int port (default_port);
    if (argc > 1) 
    {
        if (argv [1][0] != '-') 
            port = atoi (argv [1]);
    }

    // connect to conference
    //SIPServerInfo sinfo ("conference", "10.8.1.149");
    //SIPUserInfo uinfo ("test0", "example.com");

    //conference.Register (sinfo, uinfo);
    //conference.Join ();

    Server server (port);
    server.Start ();

    return EXIT_SUCCESS;
}

//=============================================================================
//
string
format_response (const EventMessage& ev)
{
    ostringstream ss;
    TiXmlDocument doc;
    TiXmlElement *root;

    doc.Parse (glb_event_xml.c_str());

    root = doc.RootElement();
    root->SetAttribute ("type", ev.type);

    ss << ev.status_code;
    root->FirstChildElement ("StatusCode")->
        InsertEndChild (TiXmlText (ss.str()));
    ss.str ("");

    ss << ev.state;
    root->FirstChildElement ("State")->
        InsertEndChild (TiXmlText (ss.str()));
    ss.str ("");

    ss << doc;

    return ss.str();
}

string
format_response (const ResponseMessage& resp)
{
    ostringstream ss;
    TiXmlDocument doc;
    TiXmlElement *root, *result, *input; 

    doc.Parse (glb_response_xml.c_str());

    root = doc.RootElement();
    root->SetAttribute ("action", resp.action);
    result = root->FirstChildElement ("Results");
    input = root->FirstChildElement ("InputXml");

    ss << resp.return_code;
    root->FirstChildElement ("ReturnCode")->
        InsertEndChild (TiXmlText (ss.str()));
    ss.str ("");

    ss << resp.status_code;
    result->FirstChildElement ("StatusCode")->
        InsertEndChild (TiXmlText (ss.str()));
    ss.str ("");

    if (resp.handle.size())
    {
        string type;
        type.assign (resp.action, 0, resp.action.find ('.'));
        type += "Handle";

        result->InsertEndChild (TiXmlElement (type));
        result->FirstChildElement (type)->
            InsertEndChild (TiXmlText (resp.handle));
    }

    ss << doc;

    return ss.str();
}

//=============================================================================
//

ActionType 
get_action_type (const TiXmlDocument& doc)
{
    string action (doc.RootElement()->Attribute ("action"));

    // TODO: this can really be optimized
    string::size_type n;
    if (action.find ("Aux") != string::npos)
    {
        n = action.find ('.');
        if (action.find ("CaptureAudioStart", n) != string::npos)
            return AuxCaptureAudioStart1;
        else if (action.find ("CaptureAudioStop", n) != string::npos)
            return AuxCaptureAudioStop1;
        else if (action.find ("GetCaptureDevices", n) != string::npos)
            return AuxGetCaptureDevices1;
        else if (action.find ("GetRenderDevices", n) != string::npos)
            return AuxGetRenderDevices1;
        else if (action.find ("SetCaptureDevice", n) != string::npos)
            return AuxSetCaptureDevice1;
        else if (action.find ("SetRenderDevice", n) != string::npos)
            return AuxSetRenderDevice1;
        else if (action.find ("SetMicLevel", n) != string::npos)
            return AuxSetMicLevel1;
        else if (action.find ("SetSpeakerLevel", n) != string::npos)
            return AuxSetSpeakerLevel1;
    }
    else if (action.find ("Account") != string::npos)
    {
        n = action.find ('.');
        if (action.find ("Login", n) != string::npos)
            return AccountLogin1;
        else if (action.find ("Logout", n) != string::npos)
            return AccountLogout1;
    }
    else if (action.find ("Connector") != string::npos)
    {
        n = action.find ('.');
        if (action.find ("Create", n) != string::npos)
            return ConnectorCreate1;
        else if (action.find ("InitiateShutdown", n) != string::npos)
            return ConnectorInitiateShutdown1;
        else if (action.find ("MuteLocalMic", n) != string::npos)
            return ConnectorMuteLocalMic1;
        else if (action.find ("MuteLocalSpeaker", n) != string::npos)
            return ConnectorMuteLocalSpeaker1;
        else if (action.find ("SetLocalMicVolume", n) != string::npos)
            return ConnectorSetLocalMicVolume1;
        else if (action.find ("SetLocalSpeakerVolume", n) != string::npos)
            return ConnectorSetLocalSpeakerVolume1;
    }
    else if (action.find ("Session") != string::npos)
    {
        n = action.find ('.');
        if (action.find ("Create", n) != string::npos)
            return SessionCreate1;
        else if (action.find ("Terminate", n) != string::npos)
            return SessionTerminate1;
        else if (action.find ("Set3DPosition", n) != string::npos)
            return SessionSet3DPosition1;
        else if (action.find ("SetParticipantMuteForMe", n) != string::npos)
            return SessionSetParticipantMuteForMe1;
        else if (action.find ("SetParticipantVolumeForMe", n) != string::npos)
            return SessionSetParticipantVolumeForMe1;
    }

    cerr << "unable to find type " << action << endl;    
    return None;
}

int 
get_request_sequence_id (const TiXmlDocument& doc)
{
    int result, id;

    result = doc.RootElement()-> QueryIntAttribute ("requestId", &id);
    if (result == TIXML_SUCCESS)
        return id;
    else
    {
        cerr << "unable to parse requestId " << endl;
        return 0;
    }
}

string 
get_request_parameter (const TiXmlDocument& doc, const string& param)
{
    const TiXmlElement *e (doc.RootElement()-> FirstChildElement (param));
    if (e)
        return e->GetText ();
    else
    {
        cerr << "failed to parse parameter " << param << endl;
        return string();
    }
}

auto_ptr <const Request> 
parse_request (const TiXmlDocument& doc)
{
    switch (get_action_type (doc))
    {
        case ConnectorCreate1:
            return parse_connection_request (doc);

        case AccountLogin1:
            return parse_account_request (doc);

        case SessionCreate1:
            return parse_session_request (doc);

        default:
            cerr << "unable to parse request " << get_action_type (doc) << endl;
            return auto_ptr <const Request> (NULL);
    }
}

auto_ptr <const Request> 
parse_account_request (const TiXmlDocument& doc)
{
    auto_ptr <Account> req (new Account);

    req->sequenceid = get_request_sequence_id (doc);

    return auto_ptr <const Request> (req);
}

auto_ptr <const Request> 
parse_connection_request (const TiXmlDocument& doc)
{
    auto_ptr <Connection> req (new Connection);

    req->sequenceid = get_request_sequence_id (doc);
    req->account_server = get_request_parameter (doc, "AccountManagementServer");

    return auto_ptr <const Request> (req);
}

auto_ptr <const Request> 
parse_session_request (const TiXmlDocument& doc)
{
    auto_ptr <Session> req (new Session);

    req->sequenceid = get_request_sequence_id (doc);

    return auto_ptr <const Request> (req);
}

//=============================================================================
//
void
print_usage_and_exit (char **argv)
{
    cout << "usage: " << argv[0] << " [<PORT>]" << "\n" 
        << "where <PORT> is the address to communicate with SL viewer." 
        << endl;

    exit (0);
}

char
get_short_option (char *arg)
{
    if ((strlen (arg) == 2) && (arg[0] == '-'))
        return arg[1];
    else
        return '\0';
}

//=============================================================================
//
const unsigned int NUM_INDENTS_PER_SPACE=2;

const char * getIndent (unsigned int numIndents)
{
    static const char * pINDENT="                                      + ";
    static const unsigned int LENGTH=strlen( pINDENT );
    unsigned int n=numIndents*NUM_INDENTS_PER_SPACE;
    if ( n > LENGTH ) n = LENGTH;

    return &pINDENT[ LENGTH-n ];
}

// same as getIndent but no "+" at the end
const char * getIndentAlt (unsigned int numIndents)
{
    static const char * pINDENT="                                        ";
    static const unsigned int LENGTH=strlen( pINDENT );
    unsigned int n=numIndents*NUM_INDENTS_PER_SPACE;
    if ( n > LENGTH ) n = LENGTH;

    return &pINDENT[ LENGTH-n ];
}

int dump_attribs_to_stdout (TiXmlElement* pElement, unsigned int indent)
{
    if ( !pElement ) return 0;

    TiXmlAttribute* pAttrib=pElement->FirstAttribute();
    int i=0;
    int ival;
    double dval;
    const char* pIndent=getIndent(indent);
    cout << "\n";
    while (pAttrib)
    {
        cout << pIndent << pAttrib->Name() << ": value=[" << pAttrib->Value() << "]"; 

        if (pAttrib->QueryIntValue(&ival)==TIXML_SUCCESS)    cout << " int=" << ival;
        if (pAttrib->QueryDoubleValue(&dval)==TIXML_SUCCESS) cout << " d=" << dval;

        cout << "\n" ;
        i++;
        pAttrib=pAttrib->Next();
    }
    return i;	
}

void dump_to_stdout (TiXmlNode* pParent, unsigned int indent)
{
    if (!pParent) return;

    TiXmlNode* pChild;
    TiXmlText* pText;
    int t = pParent->Type();
    cout << getIndent(indent);
    int num;

    switch ( t )
    {
        case TiXmlNode::DOCUMENT:
            cout << "Document" ;
            break;

        case TiXmlNode::ELEMENT:
            cout << "Element [" << pParent->Value() << "]";
            num=dump_attribs_to_stdout(pParent->ToElement(), indent+1);
            switch(num)
            {
                case 0:  cout << " (No attributes)"; break;
                case 1:  cout << getIndentAlt(indent) << "1 attribute"; break;
                default: cout << getIndentAlt(indent) << num << " attributes"; break;
            }
            break;

        case TiXmlNode::COMMENT:
            cout << "Comment: [" << pParent->Value() << "]";
            break;

        case TiXmlNode::UNKNOWN:
            cout << "Unknown" ;
            break;

        case TiXmlNode::TEXT:
            pText = pParent->ToText();
            cout << "Text: [" << pText->Value() << "]";
            break;

        case TiXmlNode::DECLARATION:
            cout << "Declaration" ;
            break;
        default:
            break;
    }

    cout << "\n" ;
    for ( pChild = pParent->FirstChild(); pChild != 0; pChild = pChild->NextSibling()) 
    {
        dump_to_stdout( pChild, indent+1 );
    }
}

