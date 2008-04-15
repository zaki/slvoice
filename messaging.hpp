/* messaging.hpp -- messaging module
 *
 *			Ryan McDougall -- 2008
 */

#ifndef _MESSAGING_HPP_
#define _MESSAGING_HPP_

#include <tinyxml/tinyxml.h>

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

const string glb_event_xml ("<Event type=\"\" ><StatusCode /><StatusString /><State /></Event>");
const string glb_response_xml ("<Response requestId=\"\" action=\"\" ><ReturnCode /><Results><StatusCode /><StatusString /></Results><InputXml /></Response>");


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


//=============================================================================
// Response Objects
struct EventMessage
{
    typedef vector <pair <string,string> > ParameterList;

    EventMessage (const string& t) 
        : type (t), status_code (0), state (0) {}

    string type;

    int status_code;
    int state;

    ParameterList params;
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


//=============================================================================
// Request Objects
// TODO: these are meant to be filled in when a request is parsed
struct Request 
{
    Request (ActionType t, int id = 0) 
        : type (t), sequenceid (id) {}

    ActionType type; 
    int sequenceid;
    
    // non-empty if request references a resource with a handle
    // the meaning of the handle is relative to the type
    string handle; 
};

struct SetHardwareRequest : public Request
{
    SetHardwareRequest (ActionType t, int id = 0) 
        : Request (t, id) {}

    string micmute;
    string micvol;
    
    string speakermute;
    string speakervol;
};

struct AccountLoginRequest : public Request
{
    AccountLoginRequest (int id = 0) 
        : Request (AccountLogin1, id) {}

    string name;
    string password;
    string uri;
    string connector_handle;
};

struct ConnectionCreateRequest : public Request
{
    ConnectionCreateRequest (int id = 0) 
        : Request (ConnectorCreate1, id) {}

    string account_server;
};

struct SessionCreateRequest : public Request
{
    SessionCreateRequest (int id = 0) 
        : Request (SessionCreate1, id) {}

    string name;
    string password;
    string uri;
    string connector_handle;
    string account_handle;
    string hash_algorithm;
};

struct PositionSetRequest : public Request
{
    PositionSetRequest (int id = 0) 
        : Request (SessionSet3DPosition1, id) {}

    VoiceOrientation speaker;
    VoiceOrientation listener;
};

typedef list <const Request *> RequestQueue;

//=============================================================================
// Functions

ActionType get_action_type (const TiXmlDocument& doc);
string format_response (const ResponseMessage& resp);
string format_response (const EventMessage& ev);
auto_ptr <const Request> parse_request (const TiXmlDocument& doc);

#endif //_MESSAGING_HPP_
