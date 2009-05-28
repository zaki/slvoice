/* parsing.cpp -- parsing module
 *
 *			Copyright 2008, 3di.jp Inc
 */

#include <main.h>
#include <parsing.hpp>

#ifndef WIN32
// this is for Linux/Unix
#include <netinet/in.h>
#else
#include <winsock2.h>
#endif

//=============================================================================
class parse_error : public std::logic_error
{
    public:
        explicit parse_error (const std::string& what) 
            : std::logic_error(what) {}
};

//=============================================================================
static inline bool
has_substring (const string& str, const char* sub, string::size_type pos = 0)
{
    return (str.find (sub, pos) != string::npos);
}

EventBase::EventBase (const string& t) 
	: type (t), OKString("OK")
{
	if (g_config->Version < 122)
	{
		OKCode = "200";
	}
	else
	{
		OKCode = "0";
	}
}

//=============================================================================
void
RequestParser::parse_vector_ (const TiXmlElement *vector, float buf [3])
{
    fill_n (buf, 3, 0.0f);
    vector-> QueryFloatAttribute ("X", buf + 0);
    vector-> QueryFloatAttribute ("Y", buf + 1);
    vector-> QueryFloatAttribute ("Z", buf + 2);
}

//=============================================================================
Orientation
RequestParser::parse_voice_orientation_ (const TiXmlElement *orient)
{
    Orientation res;
    float buf [3];
    const TiXmlElement *vec;

    vec = get_element_ (orient, "Position");
    parse_vector_ (vec, buf);
    res.set_position (buf);

    vec = get_element_ (orient, "Velocity");
    parse_vector_ (vec, buf);
    res.set_velocity (buf);

    vec = get_element_ (orient, "AtOrientation");
    parse_vector_ (vec, buf);
    res.set_at (buf);

    vec = get_element_ (orient, "UpOrientation");
    parse_vector_ (vec, buf);
    res.set_up (buf);

    vec = get_element_ (orient, "LeftOrientation");
    parse_vector_ (vec, buf);
    res.set_left (buf);

    return res;
}


//=============================================================================
auto_ptr <const Request>
RequestParser::parse_AccountLogin_ ()
{
    AccountLoginRequest *req (new AccountLoginRequest (requestid_));

    req-> ConnectorHandle = get_root_text_ ("ConnectorHandle");
    req-> AccountName = get_root_text_ ("AccountName");
    req-> AccountPassword = get_root_text_ ("AccountPassword");
    //req-> AccountURI = get_root_text_ ("AccountURI");
    //req-> AudioSessionAnswerMode = get_root_text_ ("AudioSessionAnswerMode");
    //req-> ParticipantPropertyFrequency = get_root_text_ ("ParticipantPropertyFrequency");
    //req-> EnableBuddiesAndPresence = get_root_text_ ("EnableBuddiesAndPresence");

    return auto_ptr <const Request> (req);
}

//=============================================================================
auto_ptr <const Request>
RequestParser::parse_AccountLogout_ ()
{
    AccountLogoutRequest *req (new AccountLogoutRequest (requestid_));

    req-> AccountHandle = get_root_text_ ("AccountHandle");

    return auto_ptr <const Request> (req);
}

//=============================================================================
auto_ptr <const Request>
RequestParser::parse_AuxCaptureAudioStart_ ()
{
    AuxCaptureAudioStartRequest *req 
        (new AuxCaptureAudioStartRequest (requestid_));

    req-> Duration = get_root_text_ ("Duration");

    return auto_ptr <const Request> (req);
}

//=============================================================================
auto_ptr <const Request>
RequestParser::parse_AuxCaptureAudioStop_ ()
{
	g_logger->Info() << "Version " << g_config->Version << endl;
    AuxCaptureAudioStopRequest *req 
        (new AuxCaptureAudioStopRequest (requestid_));
    
    return auto_ptr <const Request> (req);
}

//=============================================================================
auto_ptr <const Request>
RequestParser::parse_AuxGetCaptureDevices_ ()
{
    AuxGetCaptureDevicesRequest *req 
        (new AuxGetCaptureDevicesRequest (requestid_));

    return auto_ptr <const Request> (req);
}

//=============================================================================
auto_ptr <const Request>
RequestParser::parse_AuxGetRenderDevices_ ()
{
    AuxGetRenderDevicesRequest *req 
        (new AuxGetRenderDevicesRequest (requestid_));

    return auto_ptr <const Request> (req);
}

//=============================================================================
auto_ptr <const Request>
RequestParser::parse_AuxSetCaptureDevice_ ()
{
    AuxSetCaptureDeviceRequest *req 
        (new AuxSetCaptureDeviceRequest (requestid_));

    return auto_ptr <const Request> (req);
}

//=============================================================================
auto_ptr <const Request>
RequestParser::parse_AuxSetMicLevel_ ()
{
    AuxSetMicLevelRequest *req (new AuxSetMicLevelRequest (requestid_));

    req-> Level = get_root_text_ ("Level");

    return auto_ptr <const Request> (req);
}

//=============================================================================
auto_ptr <const Request>
RequestParser::parse_AuxSetRenderDevice_ ()
{
    AuxSetRenderDeviceRequest *req 
        (new AuxSetRenderDeviceRequest (requestid_));

    return auto_ptr <const Request> (req);
}

//=============================================================================
auto_ptr <const Request>
RequestParser::parse_AuxSetSpeakerLevel_ ()
{
    AuxSetSpeakerLevelRequest *req 
        (new AuxSetSpeakerLevelRequest (requestid_));

    req-> Level = get_root_text_ ("Level");

    return auto_ptr <const Request> (req);
}

//=============================================================================
auto_ptr <const Request>
RequestParser::parse_ConnectorCreate_ ()
{
    ConnectorCreateRequest *req (new ConnectorCreateRequest (requestid_));

    //req-> ClientName = get_root_text_ ("ClientName");
    //req-> AttemptStun = get_root_text_ ("AttemptStun");
    //req-> AccountManagementServer = get_root_text_ ("AccountManagementServer");
    //req-> MinimumPort = get_root_text_ ("MinimumPort");
    //req-> MaximumPort = get_root_text_ ("MaximumPort");

    return auto_ptr <const Request> (req);
}

//=============================================================================
auto_ptr <const Request>
RequestParser::parse_ConnectorInitiateShutdown_ ()
{
    ConnectorInitiateShutdownRequest *req 
        (new ConnectorInitiateShutdownRequest (requestid_));

    req-> ConnectorHandle = get_root_text_ ("ConnectorHandle");

    return auto_ptr <const Request> (req);
}

//=============================================================================
auto_ptr <const Request>
RequestParser::parse_ConnectorMuteLocalMic_ ()
{
    ConnectorMuteLocalMicRequest *req 
        (new ConnectorMuteLocalMicRequest (requestid_));

    req-> ConnectorHandle = get_root_text_ ("ConnectorHandle");
    req-> Value = get_root_text_ ("Value");

    return auto_ptr <const Request> (req);
}

//=============================================================================
auto_ptr <const Request>
RequestParser::parse_ConnectorMuteLocalSpeaker_ ()
{
    ConnectorMuteLocalSpeakerRequest *req 
        (new ConnectorMuteLocalSpeakerRequest (requestid_));

    req-> ConnectorHandle = get_root_text_ ("ConnectorHandle");
    req-> Value = get_root_text_ ("Value");

    return auto_ptr <const Request> (req);
}

//=============================================================================
auto_ptr <const Request>
RequestParser::parse_ConnectorSetLocalMicVolume_ ()
{
    ConnectorSetLocalMicVolumeRequest *req 
        (new ConnectorSetLocalMicVolumeRequest (requestid_));

    req-> ConnectorHandle = get_root_text_ ("ConnectorHandle");
    req-> Value = get_root_text_ ("Value");

    return auto_ptr <const Request> (req);
}

//=============================================================================
auto_ptr <const Request>
RequestParser::parse_ConnectorSetLocalSpeakerVolume_ ()
{
    ConnectorSetLocalSpeakerVolumeRequest *req 
        (new ConnectorSetLocalSpeakerVolumeRequest (requestid_));

    req-> ConnectorHandle = get_root_text_ ("ConnectorHandle");
    req-> Value = get_root_text_ ("Value");

    return auto_ptr <const Request> (req);
}

//=============================================================================
auto_ptr <const Request>
RequestParser::parse_SessionCreate_ ()
{
    SessionCreateRequest *req (new SessionCreateRequest (requestid_));

    req-> AccountHandle = get_root_text_ ("AccountHandle");
    req-> URI = get_root_text_ ("URI");
    //req-> Name = get_root_text_ ("Name");
    //req-> Password = get_root_text_ ("Password");
    //req-> JoinAudio = get_root_text_ ("JoinAudio");
    //req-> JoinText = get_root_text_ ("JoinText");
    //req-> PasswordHashAlgorithm = get_root_text_ ("PasswordHashAlgorithm");

    return auto_ptr <const Request> (req);
}

//=============================================================================
auto_ptr <const Request>
RequestParser::parse_SessionSet3DPosition_ ()
{
    SessionSet3DPositionRequest *req 
        (new SessionSet3DPositionRequest (requestid_));

    const TiXmlElement *speaker, *listener; 

    // TODO: doesnt parse the XML yet

	req-> SessionHandle = get_root_text_ ("SessionHandle");

    speaker = get_root_element_ ("SpeakerPosition");
    if (!speaker)
        throw parse_error ("cannot parse speaker position");

    listener = get_root_element_ ("ListenerPosition");
    if (!listener)
        throw parse_error ("cannot parse listener position");
        
    return auto_ptr <const Request> (req);
}

//=============================================================================
auto_ptr <const Request>
RequestParser::parse_SessionSetParticipantMuteForMe_ ()
{
    SessionSetParticipantMuteForMeRequest *req 
        (new SessionSetParticipantMuteForMeRequest (requestid_));

    req-> SessionHandle = get_root_text_ ("SessionHandle");
    req-> ParticipantURI = get_root_text_ ("ParticipantURI");
    req-> Mute = get_root_text_ ("Mute");

    return auto_ptr <const Request> (req);
}

//=============================================================================
auto_ptr <const Request>
RequestParser::parse_SessionSetParticipantVolumeForMe_ ()
{
    SessionSetParticipantVolumeForMeRequest *req 
        (new SessionSetParticipantVolumeForMeRequest (requestid_));

    req-> SessionHandle = get_root_text_ ("SessionHandle");
    req-> ParticipantURI = get_root_text_ ("ParticipantURI");
    req-> Volume = get_root_text_ ("Volume");

    return auto_ptr <const Request> (req);
}

//=============================================================================
auto_ptr <const Request>
RequestParser::parse_SessionTerminate_ ()
{
    SessionTerminateRequest *req (new SessionTerminateRequest (requestid_));
    
    req-> SessionHandle = get_root_text_ ("SessionHandle");

    return auto_ptr <const Request> (req);
}

//=============================================================================
auto_ptr <const Request>
RequestParser::parse_SessionConnect_ ()
{
    SessionConnectRequest *req (new SessionConnectRequest (requestid_));
    
    req-> SessionHandle = get_root_text_ ("SessionHandle");
    req-> AudioMedia = get_root_text_ ("AudioMedia");

    return auto_ptr <const Request> (req);
}

// v1.22
//=============================================================================
auto_ptr <const Request>
RequestParser::parse_AccountBlockListRules_ ()
{
    AccountBlockListRulesRequest *req 
        (new AccountBlockListRulesRequest (requestid_));

    return auto_ptr <const Request> (req);
}

auto_ptr <const Request>
RequestParser::parse_AccountListAutoAcceptRules_ ()
{
    AccountListAutoAcceptRulesRequest *req 
        (new AccountListAutoAcceptRulesRequest (requestid_));

    return auto_ptr <const Request> (req);
}

auto_ptr <const Request>
RequestParser::parse_SessionMediaDisconnect_ ()
{
    SessionMediaDisconnectRequest *req 
        (new SessionMediaDisconnectRequest (requestid_));

    return auto_ptr <const Request> (req);
}
//=============================================================================
RequestParser::RequestParser (const char *message)
{
    doc_.Parse (message); // parse the XML into DOM object
    requestid_ = get_request_id_();
    type_ = get_action_type_();
}

//=============================================================================
auto_ptr <const Request> 
RequestParser::Parse ()
{
    switch (type_)
    {
        case AccountLogin1: return parse_AccountLogin_ ();
        case AccountLogout1: return parse_AccountLogout_ ();
        case AuxCaptureAudioStart1: return parse_AuxCaptureAudioStart_ ();
        case AuxCaptureAudioStop1: return parse_AuxCaptureAudioStop_ ();
        case AuxGetCaptureDevices1: return parse_AuxGetCaptureDevices_ ();
        case AuxGetRenderDevices1: return parse_AuxGetRenderDevices_ ();
        case AuxSetCaptureDevice1: return parse_AuxSetCaptureDevice_ ();
        case AuxSetMicLevel1: return parse_AuxSetMicLevel_ ();
        case AuxSetRenderDevice1: return parse_AuxSetRenderDevice_ ();
        case AuxSetSpeakerLevel1: return parse_AuxSetSpeakerLevel_ ();
        case ConnectorCreate1: return parse_ConnectorCreate_ ();
        case ConnectorInitiateShutdown1: return parse_ConnectorInitiateShutdown_ ();
        case ConnectorMuteLocalMic1: return parse_ConnectorMuteLocalMic_ ();
        case ConnectorMuteLocalSpeaker1: return parse_ConnectorMuteLocalSpeaker_ ();
        case ConnectorSetLocalMicVolume1: return parse_ConnectorSetLocalMicVolume_ ();
        case ConnectorSetLocalSpeakerVolume1: return parse_ConnectorSetLocalSpeakerVolume_ ();
        case SessionCreate1: return parse_SessionCreate_ ();
        case SessionSet3DPosition1: return parse_SessionSet3DPosition_ ();
        case SessionSetParticipantMuteForMe1: return parse_SessionSetParticipantMuteForMe_ ();
        case SessionSetParticipantVolumeForMe1: return parse_SessionSetParticipantVolumeForMe_ ();
        case SessionTerminate1: return parse_SessionTerminate_ ();
        case SessionConnect1: return parse_SessionConnect_ ();
		case AccountBlockListRules1: return parse_AccountBlockListRules_ ();			//v1.22
		case AccountListAutoAcceptRules1: return parse_AccountListAutoAcceptRules_ ();	//v1.22
		case SessionMediaDisconnect1: return parse_SessionMediaDisconnect_ ();			//v1.22

        default: throw parse_error ("unable to parse type: " + get_action_() ); 
    }
}

//=============================================================================
string RequestParser::get_request_id_ ()
{
    return doc_.RootElement()-> Attribute ("requestId");
}

//=============================================================================
string RequestParser::get_action_ ()
{    
    return doc_.RootElement()-> Attribute ("action");
}

ActionType RequestParser::get_action_type_ ()
{
    string action (get_action_ ());

    // TODO: we can do better than this
    string::size_type n;
    if (has_substring (action, "Aux"))
    {
        n = action.find ('.');
        if (has_substring (action, "CaptureAudioStart", n))
            return AuxCaptureAudioStart1;

        else if (has_substring (action, "CaptureAudioStop", n))
            return AuxCaptureAudioStop1;

        else if (has_substring (action, "GetCaptureDevices", n))
            return AuxGetCaptureDevices1;

        else if (has_substring (action, "GetRenderDevices", n))
            return AuxGetRenderDevices1;

        else if (has_substring (action, "SetCaptureDevice", n))
            return AuxSetCaptureDevice1;

        else if (has_substring (action, "SetRenderDevice", n))
            return AuxSetRenderDevice1;

        else if (has_substring (action, "SetMicLevel", n))
            return AuxSetMicLevel1;

        else if (has_substring (action, "SetSpeakerLevel", n))
            return AuxSetSpeakerLevel1;
    }
    else if (has_substring (action, "Account"))
    {
        n = action.find ('.');
        if (has_substring (action, "Login", n))
            return AccountLogin1;

        else if (has_substring (action, "Logout", n))
            return AccountLogout1;

		else if (has_substring (action, "Block", n))
            return AccountBlockListRules1;

		else if (has_substring (action, "AutoAccept", n))
            return AccountListAutoAcceptRules1;
    }
    else if (has_substring (action, "Connector"))
    {
        n = action.find ('.');
        if (has_substring (action, "Create", n))
            return ConnectorCreate1;

        else if (has_substring (action, "InitiateShutdown", n))
            return ConnectorInitiateShutdown1;

        else if (has_substring (action, "MuteLocalMic", n))
            return ConnectorMuteLocalMic1;

        else if (has_substring (action, "MuteLocalSpeaker", n))
            return ConnectorMuteLocalSpeaker1;

        else if (has_substring (action, "SetLocalMicVolume", n))
            return ConnectorSetLocalMicVolume1;

        else if (has_substring (action, "SetLocalSpeakerVolume", n))
            return ConnectorSetLocalSpeakerVolume1;
    }
    else if (has_substring (action, "Session"))
    {
        n = action.find ('.');
        if (has_substring (action, "Create", n))
            return SessionCreate1;

        else if (has_substring (action, "Terminate", n))
            return SessionTerminate1;

        else if (has_substring (action, "Connect", n))
            return SessionConnect1;

        else if (has_substring (action, "Set3DPosition", n))
            return SessionSet3DPosition1;

        else if (has_substring (action, "SetParticipantMuteForMe", n))
            return SessionSetParticipantMuteForMe1;

        else if (has_substring (action, "SetParticipantVolumeForMe", n))
            return SessionSetParticipantVolumeForMe1;

		else if (has_substring (action, "MediaDisconnect", n))
            return SessionMediaDisconnect1;

	}

    cerr << "unable to find type " << action << endl;    
    return None;
}

//=============================================================================
const TiXmlElement* 
RequestParser::get_root_element_ (const string& name)
{
    return get_element_ (doc_.RootElement(), name);
}

const TiXmlElement* 
RequestParser::get_element_ (const TiXmlElement *e, const string& name)
{
    return e-> FirstChildElement (name);
}

string 
RequestParser::get_root_text_ (const string& name)
{
    return get_text_ (doc_.RootElement(), name);
}

string 
RequestParser::get_text_ (const TiXmlElement *e, const string& name)
{
    const TiXmlElement *p (e-> FirstChildElement (name));
    if (p) return p->GetText ();
    else
        throw parse_error ("failed to get text: " + name);
}

ResponseBase* Request::CreateResponse(const string& return_code)
{
	return NULL;
}

AccountLoginResponse* AccountLoginRequest::CreateResponse(const string& return_code)
{
	return new AccountLoginResponse(Action, RequestId, return_code);
}

AccountLogoutResponse* AccountLogoutRequest::CreateResponse(const string& return_code)
{
	return new AccountLogoutResponse(Action, RequestId, return_code);
}

AuxCaptureAudioStartResponse* AuxCaptureAudioStartRequest::CreateResponse(const string& return_code)
{
	return new AuxCaptureAudioStartResponse(Action, RequestId, return_code);
}

AuxCaptureAudioStopResponse* AuxCaptureAudioStopRequest::CreateResponse(const string& return_code)
{
	return new AuxCaptureAudioStopResponse(Action, RequestId, return_code);
}

AuxGetCaptureDevicesResponse* AuxGetCaptureDevicesRequest::CreateResponse(const string& return_code)
{
	return new AuxGetCaptureDevicesResponse(Action, RequestId, return_code);
}

AuxGetRenderDevicesResponse* AuxGetRenderDevicesRequest::CreateResponse(const string& return_code)
{
	return new AuxGetRenderDevicesResponse(Action, RequestId, return_code);
}

AuxSetCaptureDeviceResponse* AuxSetCaptureDeviceRequest::CreateResponse(const string& return_code)
{
	return new AuxSetCaptureDeviceResponse(Action, RequestId, return_code);
}

AuxSetMicLevelResponse* AuxSetMicLevelRequest::CreateResponse(const string& return_code)
{
	return new AuxSetMicLevelResponse(Action, RequestId, return_code);
}

AuxSetRenderDeviceResponse* AuxSetRenderDeviceRequest::CreateResponse(const string& return_code)
{
	return new AuxSetRenderDeviceResponse(Action, RequestId, return_code);
}

AuxSetSpeakerLevelResponse* AuxSetSpeakerLevelRequest::CreateResponse(const string& return_code)
{
	return new AuxSetSpeakerLevelResponse(Action, RequestId, return_code);
}

ConnectorCreateResponse* ConnectorCreateRequest::CreateResponse(const string& return_code)
{
	return new ConnectorCreateResponse(Action, RequestId, return_code);
}

ConnectorInitiateShutdownResponse* ConnectorInitiateShutdownRequest::CreateResponse(const string& return_code)
{
	return new ConnectorInitiateShutdownResponse(Action, RequestId, return_code);
}

ConnectorMuteLocalMicResponse* ConnectorMuteLocalMicRequest::CreateResponse(const string& return_code)
{
	return new ConnectorMuteLocalMicResponse(Action, RequestId, return_code);
}

ConnectorMuteLocalSpeakerResponse* ConnectorMuteLocalSpeakerRequest::CreateResponse(const string& return_code)
{
	return new ConnectorMuteLocalSpeakerResponse(Action, RequestId, return_code);
}

ConnectorSetLocalMicVolumeResponse* ConnectorSetLocalMicVolumeRequest::CreateResponse(const string& return_code)
{
	return new ConnectorSetLocalMicVolumeResponse(Action, RequestId, return_code);
}

ConnectorSetLocalSpeakerVolumeResponse* ConnectorSetLocalSpeakerVolumeRequest::CreateResponse(const string& return_code)
{
	return new ConnectorSetLocalSpeakerVolumeResponse(Action, RequestId, return_code);
}

SessionCreateResponse* SessionCreateRequest::CreateResponse(const string& return_code)
{
	return new SessionCreateResponse(Action, RequestId, return_code);
}

SessionSetParticipantVolumeForMeResponse* SessionSetParticipantVolumeForMeRequest::CreateResponse(const string& return_code)
{
	return new SessionSetParticipantVolumeForMeResponse(Action, RequestId, return_code);
}

SessionTerminateResponse* SessionTerminateRequest::CreateResponse(const string& return_code)
{
	return new SessionTerminateResponse(Action, RequestId, return_code);
}

SessionConnectResponse* SessionConnectRequest::CreateResponse(const string& return_code)
{
	return new SessionConnectResponse(Action, RequestId, return_code);
}

// v1.22
AccountBlockListRulesResponse* AccountBlockListRulesRequest::CreateResponse(const string& return_code)
{
	return new AccountBlockListRulesResponse(Action, RequestId, return_code);
}

AccountListAutoAcceptRulesResponse* AccountListAutoAcceptRulesRequest::CreateResponse(const string& return_code)
{
	return new AccountListAutoAcceptRulesResponse(Action, RequestId, return_code);
}

SessionMediaDisconnectResponse* SessionMediaDisconnectRequest::CreateResponse(const string& return_code)
{
	return new SessionMediaDisconnectResponse(Action, RequestId, return_code);
}

string ResponseBase::ToString()
{
	string retval;

	retval	= "<Response requestId=\"" + requestId + "\" action=\"" + action + "\">"
			+ "<ReturnCode>" + ReturnCode + "</ReturnCode>"
			+ "<Results><StatusCode>" + (StatusCode.empty() ? "0" : StatusCode) + "</StatusCode>"
			+ "<StatusString>" + (StatusString.empty() ? "OK" : StatusString) + "</StatusString>"
			+ "</Results>"
			+ "<InputXml>" + InputXml + "</InputXml></Response>\n\n\n";
	
	return retval;
}

string ConnectorCreateResponse::ToString()
{
	string retval;

	retval	= "<Response requestId=\"" + requestId + "\" action=\"" + action + "\">"
			+ "<ReturnCode>" + ReturnCode + "</ReturnCode>"
			+ "<Results><StatusCode>" + (StatusCode.empty() ? "0" : StatusCode) + "</StatusCode>"
			+ "<StatusString>" + (StatusString.empty() ? "OK" : StatusString) + "</StatusString>"

			+ "<VersionID>" + VersionID + "</VersionID>"
			+ "<ConnectorHandle>" + ConnectorHandle + "</ConnectorHandle>"

			+ "</Results>"
			+ "<InputXml>" + InputXml + "</InputXml></Response>\n\n\n";
	
	return retval;
}

string AuxGetCaptureDevicesResponse::ToString()
{
	string retval;

	retval	= "<Response requestId=\"" + requestId + "\" action=\"" + action + "\">"
			+ "<ReturnCode>" + ReturnCode + "</ReturnCode>"
			+ "<Results><StatusCode>" + (StatusCode.empty() ? "0" : StatusCode) + "</StatusCode>"
			+ "<StatusString>" + (StatusString.empty() ? "OK" : StatusString) + "</StatusString>"

			+ "<Device>" + Device + "</Device>"
			+ "<CurrentCaptureDevice>" + CurrentCaptureDevice + "</CurrentCaptureDevice>"

			+ "</Results>"
			+ "<InputXml>" + InputXml + "</InputXml></Response>\n\n\n";
	
	return retval;
}

string AccountLoginResponse::ToString()
{
	string retval;

	retval	= "<Response requestId=\"" + requestId + "\" action=\"" + action + "\">"
			+ "<ReturnCode>" + ReturnCode + "</ReturnCode>"
			+ "<Results><StatusCode>" + (StatusCode.empty() ? "0" : StatusCode) + "</StatusCode>"
			+ "<StatusString>" + (StatusString.empty() ? "OK" : StatusString) + "</StatusString>"

			+ "<AccountHandle>" + AccountHandle + "</AccountHandle>"

			+ "</Results>"
			+ "<InputXml>" + InputXml + "</InputXml></Response>\n\n\n";
	
	return retval;
}

string SessionCreateResponse::ToString()
{
	string retval;

	retval	= "<Response requestId=\"" + requestId + "\" action=\"" + action + "\">"
			+ "<ReturnCode>" + ReturnCode + "</ReturnCode>"
			+ "<Results><StatusCode>" + (StatusCode.empty() ? "0" : StatusCode) + "</StatusCode>"
			+ "<StatusString>" + (StatusString.empty() ? "OK" : StatusString) + "</StatusString>"

			+ "<SessionHandle>" + SessionHandle + "</SessionHandle>"

			+ "</Results>"
			+ "<InputXml>" + InputXml + "</InputXml></Response>\n\n\n";
	
	return retval;
}

// v1.22
string AccountBlockListRulesResponse::ToString()
{
	string retval;

	retval	= "<Response requestId=\"" + requestId + "\" action=\"" + action + "\">"
			+ "<ReturnCode>" + ReturnCode + "</ReturnCode>"
			+ "<Results><StatusCode>0</StatusCode>"
			+ "<StatusString>OK</StatusString>"
			+ "</Results>"
			+ "<InputXml>" + InputXml + "</InputXml></Response>\n\n\n";
	
	return retval;
}

string AccountListAutoAcceptRulesResponse::ToString()
{
	string retval;

	retval	= "<Response requestId=\"" + requestId + "\" action=\"" + action + "\">"
			+ "<ReturnCode>" + ReturnCode + "</ReturnCode>"
			+ "<Results><StatusCode>0</StatusCode>"
			+ "<StatusString>OK</StatusString>"
			+ "</Results>"
			+ "<InputXml>" + InputXml + "</InputXml></Response>\n\n\n";
	
	return retval;
}

string SessionMediaDisconnectResponse::ToString()
{
	string retval;

	retval	= "<Response requestId=\"" + requestId + "\" action=\"" + action + "\">"
			+ "<ReturnCode>" + ReturnCode + "</ReturnCode>"
			+ "<Results><StatusCode>0</StatusCode>"
			+ "<StatusString>OK</StatusString>"
			+ "</Results>"
			+ "<InputXml>" + InputXml + "</InputXml></Response>\n\n\n";
	
	return retval;
}

string AuxGetRenderDevicesResponse::ToString()
{
	string retval;

	retval	= "<Response requestId=\"" + requestId + "\" action=\"" + action + "\">"
			+ "<ReturnCode>" + ReturnCode + "</ReturnCode>"
			+ "<Results><StatusCode>" + (StatusCode.empty() ? "0" : StatusCode) + "</StatusCode>"
			+ "<StatusString>" + (StatusString.empty() ? "OK" : StatusString) + "</StatusString>"

			+ "<Device>" + Device + "</Device>"
			+ "<CurrentRenderDevice>" + CurrentRenderDevice + "</CurrentRenderDevice>"

			+ "</Results>"
			+ "<InputXml>" + InputXml + "</InputXml></Response>\n\n\n";
	
	return retval;
}

// Events

string LoginStateChangeEvent::ToString()
{
	string retval;

	string type;
	if (g_config->Version < 122)
	{
		type = string(LoginStateChangeEventString);
	}
	else
	{
		type = string(AccountLoginStateChangeEventString);
	}

	retval  = "<Event type=\"" + type + "\">"
			+ "<AccountHandle>" + AccountHandle + "</AccountHandle>"
			+ "<StatusCode>" + (StatusCode.empty() ? "0" : StatusCode) + "</StatusCode>"
			+ "<StatusString>" + (StatusString.empty() ? "OK" : StatusString) + "</StatusString>"
			+ "<State>" + State + "</State>"
			+ "</Event>\n\n\n";

	return retval;
}


string SessionNewEvent::ToString()
{
	string retval;

	retval  = "<Event type=\"" + type + "\">"
			+ "<AccountHandle>" + AccountHandle + "</AccountHandle>"
			+ "<SessionHandle>" + SessionHandle + "</SessionHandle>"
			+ "<State>" + State + "</State>"
			+ "<URI>" + URI + "</URI>"
			+ "<Name>" + Name + "</Name>"
			+ "<IsChannel>" + IsChannel + "</IsChannel>"
			+ "<AudioMedia>" + AudioMedia + "</AudioMedia>"
			+ "<HasText>" + HasText + "</HasText>"
			+ "<HasAudio>" + HasAudio + "</HasAudio>"
			+ "<HasVideo>" + HasVideo + "</HasVideo>"
			+ "</Event>\n\n\n";

	return retval;
}

string SessionStateChangeEvent::ToString()
{
	string retval;

	retval  = "<Event type=\"" + type + "\">"
			+ "<SessionHandle>" + SessionHandle + "</SessionHandle>"
			+ "<StatusCode>" + (StatusCode.empty() ? "0" : StatusCode) + "</StatusCode>"
			+ "<StatusString>" + (StatusString.empty() ? "OK" : StatusString) + "</StatusString>"
			+ "<State>" + State + "</State>"
			+ "<URI>" + URI + "</URI>"
			+ "<IsChannel>" + IsChannel + "</IsChannel>"
			+ "<ChannelName>" + ChannelName + "</ChannelName>"
			+ "</Event>\n\n\n";

	return retval;
}

string ParticipantStateChangeEvent::ToString()
{
	string retval;

	retval  = "<Event type=\"" + type + "\">"
			+ "<StatusCode>" + (StatusCode.empty() ? "0" : StatusCode) + "</StatusCode>"
			+ "<StatusString>" + (StatusString.empty() ? "OK" : StatusString) + "</StatusString>"
			+ "<State>" + State + "</State>"
			+ "<ParticipantURI>" + ParticipantURI + "</ParticipantURI>"
			+ "<AccountName>" + AccountName + "</AccountName>"
			+ "<DisplayName>" + DisplayName + "</DisplayName>"
			+ "<ParticipantType>" + ParticipantType + "</ParticipantType>"
			+ "</Event>\n\n\n";

	return retval;
}

string ParticipantPropertiesEvent::ToString()
{
	string retval;

	retval  = "<Event type=\"" + (g_config->Version < 122 ? type : "ParticipantUpdatedEvent") + "\">"
			+ "<SessionHandle>" + SessionHandle + "</SessionHandle>"
			+ "<ParticipantURI>" + ParticipantURI + "</ParticipantURI>"
			+ "<IsLocallyMuted>" + IsLocallyMuted + "</IsLocallyMuted>"
			+ "<IsModeratorMuted>" + IsModeratorMuted + "</IsModeratorMuted>"
			+ "<Volume>" + Volume + "</Volume>"
			+ "<Energy>" + Energy + "</Energy>"
			+ "<IsSpeaking>" + IsSpeaking + "</IsSpeaking>"
			+ "</Event>\n\n\n";

	return retval;
}

string AuxAudioPropertiesEvent::ToString()
{
	string retval;

	retval  = "<Event type=\"" + type + "\">"
			+ "<MicIsActive>" + MicIsActive + "</MicIsActive>"
			+ "<MicEnergy>" + MicEnergy + "</MicEnergy>"
			+ "<MicVolume>" + MicVolume + "</MicVolume>"
			+ "<SpeakerVolume>" + SpeakerVolume + "</SpeakerVolume>"
			+ "</Event>\n\n\n";

	return retval;
}

// v1.22
string MediaStreamUpdatedEvent::ToString()
{
	string retval;

	retval  = "<Event type=\"" + type + "\">"
			+ "<SessionHandle>" + SessionHandle+ "</SessionHandle>"
			+ "<SessionGroupHandle>" + SessionGroupHandle+ "</SessionGroupHandle>"
			+ "<State>" + State + "</State>"
			+ "<StatusCode>" + StatusCode+ "</StatusCode>"
			+ "<StatusString>" + StatusString+ "</StatusString>"
			+ "<Incoming></Incoming>"
			+ "</Event>\n\n\n";

	return retval;
}

//=============================================================================
void Request::SetState (Account& state) const
{
}

void Request::SetState (Connection& state) const
{
}

void Request::SetState (Session& state) const
{
}

void Request::SetState (Audio& state) const
{
}

void Request::SetState (Orientation& state) const
{
}


//=============================================================================
void 
AccountLoginRequest::SetState (Account& state) const
{
	state.name = AccountName;
	state.password = AccountPassword;
	state.uri = AccountURI;
}

void 
ConnectorMuteLocalMicRequest::SetState (Audio& state) const
{
    stringstream ss;
       
    ss.str (Value);
    ss >> boolalpha >> state.mic_mute;

	g_logger->Debug() << "state.mic_mute = " << state.mic_mute << endl;
}

void 
ConnectorMuteLocalSpeakerRequest::SetState (Audio& state) const
{
    stringstream ss;

    ss.str (Value);
    ss >> boolalpha >> state.speaker_mute;

	g_logger->Debug() << "state.speaker_mute = " << state.speaker_mute << endl;
}

void 
ConnectorSetLocalMicVolumeRequest::SetState (Audio& state) const
{
	state.mic_volume = (float)atof(Value.c_str());
	g_logger->Debug() << "state.mic_volume = " << state.mic_volume << endl;
}

void 
ConnectorSetLocalSpeakerVolumeRequest::SetState (Audio& state) const
{
	state.speaker_volume = (float)atof(Value.c_str());
	g_logger->Debug() << "state.speaker_volume = " << state.speaker_volume << endl;
}

void 
SessionCreateRequest::SetState (Session& state) const
{
	state.name = Name;
	state.password = Password;
	state.uri = URI;
}


void 
SessionSet3DPositionRequest::SetState (Orientation& state) const
{
}
