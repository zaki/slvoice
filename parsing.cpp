/* parsing.cpp -- parsing module
 *
 *			Ryan McDougall -- 2008
 */

#include <main.h>
#include <parsing.hpp>

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
    AccountLoginRequest *req (new AccountLoginRequest (sequenceid_));

    req-> ConnectorHandle = get_root_text_ ("ConnectorHandle");
    req-> AccountName = get_root_text_ ("AccountName");
    req-> AccountPassword = get_root_text_ ("AccountPassword");
    req-> AccountURI = get_root_text_ ("AccountURI");
    //req-> AudioSessionAnswerMode = get_root_text_ ("AudioSessionAnswerMode");
    //req-> ParticipantPropertyFrequency = get_root_text_ ("ParticipantPropertyFrequency");
    //req-> EnableBuddiesAndPresence = get_root_text_ ("EnableBuddiesAndPresence");

    return auto_ptr <const Request> (req);
}

//=============================================================================
auto_ptr <const Request>
RequestParser::parse_AccountLogout_ ()
{
    AccountLogoutRequest *req (new AccountLogoutRequest (sequenceid_));

    req-> AccountHandle = get_root_text_ ("AccountHandle");

    return auto_ptr <const Request> (req);
}

//=============================================================================
auto_ptr <const Request>
RequestParser::parse_AuxCaptureAudioStart_ ()
{
    AuxCaptureAudioStartRequest *req 
        (new AuxCaptureAudioStartRequest (sequenceid_));

    req-> Duration = get_root_text_ ("Duration");

    return auto_ptr <const Request> (req);
}

//=============================================================================
auto_ptr <const Request>
RequestParser::parse_AuxCaptureAudioStop_ ()
{
    AuxCaptureAudioStopRequest *req 
        (new AuxCaptureAudioStopRequest (sequenceid_));
    
    return auto_ptr <const Request> (req);
}

//=============================================================================
auto_ptr <const Request>
RequestParser::parse_AuxGetCaptureDevices_ ()
{
    AuxGetCaptureDevicesRequest *req 
        (new AuxGetCaptureDevicesRequest (sequenceid_));

    return auto_ptr <const Request> (req);
}

//=============================================================================
auto_ptr <const Request>
RequestParser::parse_AuxGetRenderDevices_ ()
{
    AuxGetRenderDevicesRequest *req 
        (new AuxGetRenderDevicesRequest (sequenceid_));

    return auto_ptr <const Request> (req);
}

//=============================================================================
auto_ptr <const Request>
RequestParser::parse_AuxSetCaptureDevice_ ()
{
    AuxSetCaptureDeviceRequest *req 
        (new AuxSetCaptureDeviceRequest (sequenceid_));

    return auto_ptr <const Request> (req);
}

//=============================================================================
auto_ptr <const Request>
RequestParser::parse_AuxSetMicLevel_ ()
{
    AuxSetMicLevelRequest *req (new AuxSetMicLevelRequest (sequenceid_));

    req-> Level = get_root_text_ ("Level");

    return auto_ptr <const Request> (req);
}

//=============================================================================
auto_ptr <const Request>
RequestParser::parse_AuxSetRenderDevice_ ()
{
    AuxSetRenderDeviceRequest *req 
        (new AuxSetRenderDeviceRequest (sequenceid_));

    return auto_ptr <const Request> (req);
}

//=============================================================================
auto_ptr <const Request>
RequestParser::parse_AuxSetSpeakerLevel_ ()
{
    AuxSetSpeakerLevelRequest *req 
        (new AuxSetSpeakerLevelRequest (sequenceid_));

    req-> Level = get_root_text_ ("Level");

    return auto_ptr <const Request> (req);
}

//=============================================================================
auto_ptr <const Request>
RequestParser::parse_ConnectorCreate_ ()
{
    ConnectorCreateRequest *req (new ConnectorCreateRequest (sequenceid_));

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
        (new ConnectorInitiateShutdownRequest (sequenceid_));

    req-> ConnectorHandle = get_root_text_ ("ConnectorHandle");

    return auto_ptr <const Request> (req);
}

//=============================================================================
auto_ptr <const Request>
RequestParser::parse_ConnectorMuteLocalMic_ ()
{
    ConnectorMuteLocalMicRequest *req 
        (new ConnectorMuteLocalMicRequest (sequenceid_));

    req-> ConnectorHandle = get_root_text_ ("ConnectorHandle");
    req-> Value = get_root_text_ ("Value");

    return auto_ptr <const Request> (req);
}

//=============================================================================
auto_ptr <const Request>
RequestParser::parse_ConnectorMuteLocalSpeaker_ ()
{
    ConnectorMuteLocalSpeakerRequest *req 
        (new ConnectorMuteLocalSpeakerRequest (sequenceid_));

    req-> ConnectorHandle = get_root_text_ ("ConnectorHandle");
    req-> Value = get_root_text_ ("Value");

    return auto_ptr <const Request> (req);
}

//=============================================================================
auto_ptr <const Request>
RequestParser::parse_ConnectorSetLocalMicVolume_ ()
{
    ConnectorSetLocalMicVolumeRequest *req 
        (new ConnectorSetLocalMicVolumeRequest (sequenceid_));

    req-> ConnectorHandle = get_root_text_ ("ConnectorHandle");
    req-> Value = get_root_text_ ("Value");

    return auto_ptr <const Request> (req);
}

//=============================================================================
auto_ptr <const Request>
RequestParser::parse_ConnectorSetLocalSpeakerVolume_ ()
{
    ConnectorSetLocalSpeakerVolumeRequest *req 
        (new ConnectorSetLocalSpeakerVolumeRequest (sequenceid_));

    req-> ConnectorHandle = get_root_text_ ("ConnectorHandle");
    req-> Value = get_root_text_ ("Value");

    return auto_ptr <const Request> (req);
}

//=============================================================================
auto_ptr <const Request>
RequestParser::parse_SessionCreate_ ()
{
    SessionCreateRequest *req (new SessionCreateRequest (sequenceid_));

    req-> AccountHandle = get_root_text_ ("AccountHandle");
    req-> URI = get_root_text_ ("URI");
    req-> Name = get_root_text_ ("Name");
    req-> Password = get_root_text_ ("Password");
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
        (new SessionSet3DPositionRequest (sequenceid_));

    const TiXmlElement *speaker, *listener; 

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
        (new SessionSetParticipantMuteForMeRequest (sequenceid_));

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
        (new SessionSetParticipantVolumeForMeRequest (sequenceid_));

    req-> SessionHandle = get_root_text_ ("SessionHandle");
    req-> ParticipantURI = get_root_text_ ("ParticipantURI");
    req-> Volume = get_root_text_ ("Volume");

    return auto_ptr <const Request> (req);
}

//=============================================================================
auto_ptr <const Request>
RequestParser::parse_SessionTerminate_ ()
{
    SessionTerminateRequest *req (new SessionTerminateRequest (sequenceid_));
    
    req-> SessionHandle = get_root_text_ ("SessionHandle");

    return auto_ptr <const Request> (req);
}

//=============================================================================
RequestParser::RequestParser (char *message)
{
    doc_.Parse (message); // parse the XML into DOM object
    sequenceid_ = get_sequence_id_();
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

        default: throw parse_error ("unable to parse type: " + get_action_() ); 
    }
}

//=============================================================================
int RequestParser::get_sequence_id_ ()
{
    int res, id;

    res = doc_.RootElement()-> QueryIntAttribute ("requestId", &id);
    if (res == TIXML_SUCCESS) return id;
    else
        throw parse_error ("unable to parse sequence id");
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

        else if (has_substring (action, "Set3DPosition", n))
            return SessionSet3DPosition1;

        else if (has_substring (action, "SetParticipantMuteForMe", n))
            return SessionSetParticipantMuteForMe1;

        else if (has_substring (action, "SetParticipantVolumeForMe", n))
            return SessionSetParticipantVolumeForMe1;
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
    if (p) return e->GetText ();
    else
        throw parse_error ("failed to get text: " + name);
}


//=============================================================================
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

    if (ev.params.size())
    {
        EventMessage::ParameterList::const_iterator 
            i (ev.params.begin()),
              end (ev.params.end());

        for (; i != end; ++i)
        {
            TiXmlElement param (i->first);
            param.InsertEndChild (TiXmlText (i->second));
            root->InsertEndChild (param);
        }
    }

    ss << doc;

    return ss.str();
}

//=============================================================================
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
void 
AccountLoginRequest::SetState (Account& state) const
{
    stringstream ss;

    ss.str (AccountName);
    ss >> state.name;

    ss.str (AccountPassword);
    ss >> state.password;
    
    ss.str (AccountURI);
    ss >> state.uri;
}

void 
ConnectorMuteLocalMicRequest::SetState (Audio& state) const
{
    stringstream ss;
    ss << boolalpha;
        
    ss.str (Value);
    ss >> state.mic_mute;
}

void 
ConnectorMuteLocalSpeakerRequest::SetState (Audio& state) const
{
    stringstream ss;
    ss << boolalpha;
        
    ss.str (Value);
    ss >> state.speaker_mute;
}

void 
ConnectorSetLocalMicVolumeRequest::SetState (Audio& state) const
{
    stringstream ss;
        
    ss.str (Value);
    ss >> state.mic_volume;
}

void 
ConnectorSetLocalSpeakerVolumeRequest::SetState (Audio& state) const
{
    stringstream ss;
        
    ss.str (Value);
    ss >> state.speaker_volume;
}

void 
SessionCreateRequest::SetState (Session& state) const
{
    stringstream ss;

    ss.str (Name);
    ss >> state.name;

    ss.str (Password);
    ss >> state.password;
    
    ss.str (URI);
    ss >> state.uri;
}
