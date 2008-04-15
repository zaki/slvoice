/* messaging.cpp -- messaging module
 *
 *			Ryan McDougall -- 2008
 */

#include <main.h>
#include <messaging.hpp>

//=============================================================================
static int 
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

//=============================================================================
static string 
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

//=============================================================================
static auto_ptr <const float>
parse_vector (const TiXmlElement *vec)
{
    auto_ptr <float> handle (new float [3]);
    float *array (handle.get());

    vec-> QueryFloatAttribute ("X", array+0);
    vec-> QueryFloatAttribute ("Y", array+1);
    vec-> QueryFloatAttribute ("Z", array+2);

    return auto_ptr <const float> (handle);
}

//=============================================================================
static auto_ptr <const VoiceOrientation>
parse_voice_orientation (const TiXmlElement *pos)
{
    auto_ptr <VoiceOrientation> voice (new VoiceOrientation);
    auto_ptr <const float> handle;
    const TiXmlElement *e;

    e = pos-> FirstChildElement ("Position");
    if (!e) throw runtime_error ("cannot parse 3d position");
    else
    {
        handle = parse_vector (e);
        voice->set_position (handle.get());
    }

    e = pos-> FirstChildElement ("Velocity");
    if (!e) throw runtime_error ("cannot parse 3d velocity");
    else
    {
        handle = parse_vector (e);
        voice->set_velocity (handle.get());
    }

    e = pos-> FirstChildElement ("AtOrientation");
    if (!e) throw runtime_error ("cannot parse 3d at orientation");
    else
    {
        handle = parse_vector (e);
        voice->set_at (handle.get());
    }

    e = pos-> FirstChildElement ("UpOrientation");
    if (!e) throw runtime_error ("cannot parse 3d up orientation");
    else
    {
        handle = parse_vector (e);
        voice->set_up (handle.get());
    }

    e = pos-> FirstChildElement ("LeftOrientation");
    if (!e) throw runtime_error ("cannot parse 3d left orientation");
    else
    {
        handle = parse_vector (e);
        voice->set_left (handle.get());
    }

    return auto_ptr <const VoiceOrientation> (voice);
}

//=============================================================================
static auto_ptr <const Request> 
parse_position_request (const TiXmlDocument& doc)
{
    auto_ptr <PositionSetRequest> req (new PositionSetRequest);
    auto_ptr <const VoiceOrientation> speakerpos, listenerpos;
    const TiXmlElement *speaker, *listener; 

    req->sequenceid = get_request_sequence_id (doc);

    speaker = doc.RootElement()-> FirstChildElement ("SpeakerPosition");
    if (!speaker)
        throw runtime_error ("cannot parse speaker position");

    listener = doc.RootElement()-> FirstChildElement ("ListenerPosition");
    if (!listener)
        throw runtime_error ("cannot parse listener position");
        
    speakerpos = parse_voice_orientation (speaker);
    listenerpos = parse_voice_orientation (listener);

    req->speaker = *speakerpos;
    req->listener = *listenerpos;

    return auto_ptr <const Request> (req);
}

//=============================================================================
static auto_ptr <const Request> 
parse_account_login_request (const TiXmlDocument& doc)
{
    auto_ptr <AccountLoginRequest> req (new AccountLoginRequest);

    req->sequenceid = get_request_sequence_id (doc);
    req->handle = get_request_parameter (doc, "ConnectorHandle");

    return auto_ptr <const Request> (req);
}

//=============================================================================
static auto_ptr <const Request> 
parse_account_logout_request (const TiXmlDocument& doc)
{
    auto_ptr <Request> req (new Request (AccountLogout1));

    req->sequenceid = get_request_sequence_id (doc);
    req->handle = get_request_parameter (doc, "AccountHandle");

    return auto_ptr <const Request> (req);
}

//=============================================================================
static auto_ptr <const Request> 
parse_connection_create_request (const TiXmlDocument& doc)
{
    auto_ptr <ConnectionCreateRequest> req (new ConnectionCreateRequest);

    req->sequenceid = get_request_sequence_id (doc);

    req->account_server = get_request_parameter (doc, "AccountManagementServer");

    return auto_ptr <const Request> (req);
}

//=============================================================================
static auto_ptr <const Request> 
parse_connection_shutdown_request (const TiXmlDocument& doc)
{
    auto_ptr <Request> req (new Request (ConnectorInitiateShutdown1));

    req->sequenceid = get_request_sequence_id (doc);
    req->handle = get_request_parameter (doc, "ConnectorHandle");

    return auto_ptr <const Request> (req);
}

//=============================================================================
static auto_ptr <const Request> 
parse_session_create_request (const TiXmlDocument& doc)
{
    auto_ptr <SessionCreateRequest> req (new SessionCreateRequest);

    req->sequenceid = get_request_sequence_id (doc);
    req->handle = get_request_parameter (doc, "AccountHandle");

    return auto_ptr <const Request> (req);
}

//=============================================================================
static auto_ptr <const Request> 
parse_session_terminate_request (const TiXmlDocument& doc)
{
    auto_ptr <Request> req (new Request (SessionTerminate1));

    req->sequenceid = get_request_sequence_id (doc);
    req->handle = get_request_parameter (doc, "SessionHandle");

    return auto_ptr <const Request> (req);
}

//=============================================================================
static auto_ptr <const Request> 
parse_mute_mic (const TiXmlDocument& doc)
{
    auto_ptr <Request> req (new Request (ConnectorMuteLocalMic1));

    req->sequenceid = get_request_sequence_id (doc);
    req-> = get_request_parameter (doc, "Value");

    return auto_ptr <const Request> (req);
}

//=============================================================================
static inline bool
has_substring (const string& str, const char* sub, string::size_type pos = 0)
{
    return (str.find (sub, pos) != string::npos);
}

//=============================================================================
ActionType 
get_action_type (const TiXmlDocument& doc)
{
    string action (doc.RootElement()->Attribute ("action"));

    // TODO: this can really be optimized
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
auto_ptr <const Request> 
parse_request (const TiXmlDocument& doc)
{
    ActionType type (get_action_type (doc));
    switch (type)
    {
        case ConnectorCreate1:
            return parse_connection_create_request (doc);

        case AccountLogin1:
            return parse_account_login_request (doc);

        case SessionCreate1:
            return parse_session_create_request (doc);

        case SessionSet3DPosition1:
            return parse_position_request (doc);

        case SessionTerminate1:
            return parse_session_terminate_request (doc);

        case AccountLogout1:
            return parse_account_login_request (doc);

        case ConnectorInitiateShutdown1:
            return parse_connection_shutdown_request (doc);

        default:
            cerr << "==== unable to parse request " << type << " ====" << endl;
            return auto_ptr <const Request> (NULL);
    }
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

