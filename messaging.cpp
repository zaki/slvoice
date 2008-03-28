/* messaging.cpp -- messaging module
 *
 *			Ryan McDougall -- 2008
 */

#include <main.h>
#include <messaging.hpp>

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
static auto_ptr <const Request> 
parse_account_request (const TiXmlDocument& doc)
{
    auto_ptr <Account> req (new Account (AccountLogin1));

    req->sequenceid = get_request_sequence_id (doc);

    return auto_ptr <const Request> (req);
}

//=============================================================================
static auto_ptr <const Request> 
parse_connection_request (const TiXmlDocument& doc)
{
    auto_ptr <Connection> req (new Connection (ConnectorCreate1));

    req->sequenceid = get_request_sequence_id (doc);
    req->account_server = get_request_parameter (doc, "AccountManagementServer");

    return auto_ptr <const Request> (req);
}

//=============================================================================
static auto_ptr <const Request> 
parse_session_request (const TiXmlDocument& doc)
{
    auto_ptr <Session> req (new Session (SessionCreate1));

    req->sequenceid = get_request_sequence_id (doc);

    return auto_ptr <const Request> (req);
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
    auto_ptr <Position> req (new Position (SessionSet3DPosition1));
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

        case SessionSet3DPosition1:
            return parse_position_request (doc);

        default:
            cerr << "unable to parse request " << get_action_type (doc) << endl;
            return auto_ptr <const Request> (NULL);
    }
}
