/* parsing.hpp -- parsing module
 *
 *			Copyright 2008, 3di.jp Inc
 */

#ifndef _MESSAGING_HPP_
#define _MESSAGING_HPP_

#include "tinyxml/tinyxml.h"

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
const string SessionConnect1String ("Session.Connect.1");
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
    SessionConnect1,
    SessionSet3DPosition1,
    SessionSetParticipantMuteForMe1,
    SessionSetParticipantVolumeForMe1,
    SessionTerminate1
};


//=============================================================================
// Response Objects

struct ResponseBase
{
    ResponseBase(const string& a, const string& request_id, const string& return_code) 
        : action(a), requestId(request_id), ReturnCode(return_code) {}

	string requestId;
    string action;
    string ReturnCode;
    string StatusCode;
    string StatusString;

	virtual string ToString();
};


struct ConnectorCreateResponse : public ResponseBase
{
    ConnectorCreateResponse(const string& a, const string& request_id, const string& return_code)
        : ResponseBase(a, request_id, return_code) {}

	string VersionID;
	string ConnectorHandle;

	string ToString();
};


struct AuxGetCaptureDevicesResponse : public ResponseBase
{
    AuxGetCaptureDevicesResponse(const string& a, const string& request_id, const string& return_code)
        : ResponseBase(a, request_id, return_code) {}

    string Device;
    string CurrentCaptureDevice;

	string ToString();
};


struct AccountLoginResponse : public ResponseBase
{
    AccountLoginResponse(const string& a, const string& request_id, const string& return_code)
        : ResponseBase(a, request_id, return_code) {}

    string AccountHandle;

	string ToString();
};


struct SessionCreateResponse : public ResponseBase
{
    SessionCreateResponse(const string& a, const string& request_id, const string& return_code)
        : ResponseBase(a, request_id, return_code) {}

    string SessionHandle;

	string ToString();
};

struct AuxGetRenderDevicesResponse : public ResponseBase
{
    AuxGetRenderDevicesResponse(const string& a, const string& request_id, const string& return_code)
        : ResponseBase(a, request_id, return_code) {}

    string Device;
    string CurrentRenderDevice;

	string ToString();
};


struct ConnectorInitiateShutdownResponse : public ResponseBase
{
    ConnectorInitiateShutdownResponse(const string& a, const string& request_id, const string& return_code)
        : ResponseBase(a, request_id, return_code) {}
};

struct ConnectorMuteLocalMicResponse : public ResponseBase
{
    ConnectorMuteLocalMicResponse(const string& a, const string& request_id, const string& return_code)
        : ResponseBase(a, request_id, return_code) {}
};

struct ConnectorMuteLocalSpeakerResponse : public ResponseBase
{
    ConnectorMuteLocalSpeakerResponse(const string& a, const string& request_id, const string& return_code)
        : ResponseBase(a, request_id, return_code) {}
};

struct ConnectorSetLocalMicVolumeResponse : public ResponseBase
{
    ConnectorSetLocalMicVolumeResponse(const string& a, const string& request_id, const string& return_code)
        : ResponseBase(a, request_id, return_code) {}
};

struct ConnectorSetLocalSpeakerVolumeResponse : public ResponseBase
{
    ConnectorSetLocalSpeakerVolumeResponse(const string& a, const string& request_id, const string& return_code)
        : ResponseBase(a, request_id, return_code) {}
};

struct AuxSetRenderDeviceResponse : public ResponseBase
{
    AuxSetRenderDeviceResponse(const string& a, const string& request_id, const string& return_code)
        : ResponseBase(a, request_id, return_code) {}
};

struct AuxSetCaptureDeviceResponse : public ResponseBase
{
    AuxSetCaptureDeviceResponse(const string& a, const string& request_id, const string& return_code)
        : ResponseBase(a, request_id, return_code) {}
};

struct SessionRenderAudioStartResponse : public ResponseBase
{
    SessionRenderAudioStartResponse(const string& a, const string& request_id, const string& return_code)
        : ResponseBase(a, request_id, return_code) {}
};

struct SessionRenderAudioStopResponse : public ResponseBase
{
    SessionRenderAudioStopResponse(const string& a, const string& request_id, const string& return_code)
        : ResponseBase(a, request_id, return_code) {}
};

struct AuxCaptureAudioStartResponse : public ResponseBase
{
    AuxCaptureAudioStartResponse(const string& a, const string& request_id, const string& return_code)
        : ResponseBase(a, request_id, return_code) {}
};

struct AuxCaptureAudioStopResponse : public ResponseBase
{
    AuxCaptureAudioStopResponse(const string& a, const string& request_id, const string& return_code)
        : ResponseBase(a, request_id, return_code) {}
};

struct AuxSetMicLevelResponse : public ResponseBase
{
    AuxSetMicLevelResponse(const string& a, const string& request_id, const string& return_code)
        : ResponseBase(a, request_id, return_code) {}
};

struct AuxSetSpeakerLevelResponse : public ResponseBase
{
    AuxSetSpeakerLevelResponse(const string& a, const string& request_id, const string& return_code)
        : ResponseBase(a, request_id, return_code) {}
};

struct AccountLogoutResponse : public ResponseBase
{
    AccountLogoutResponse(const string& a, const string& request_id, const string& return_code)
        : ResponseBase(a, request_id, return_code) {}
};

struct SessionTerminateResponse : public ResponseBase
{
    SessionTerminateResponse(const string& a, const string& request_id, const string& return_code)
        : ResponseBase(a, request_id, return_code) {}
};

struct SessionSetParticipantVolumeForMeResponse : public ResponseBase
{
    SessionSetParticipantVolumeForMeResponse(const string& a, const string& request_id, const string& return_code)
        : ResponseBase(a, request_id, return_code) {}
};

struct SessionConnectResponse : public ResponseBase
{
    SessionConnectResponse(const string& a, const string& request_id, const string& return_code)
        : ResponseBase(a, request_id, return_code) {}
};


//=============================================================================
// Request Objects

struct Request 
{
    Request(const ActionType& t, const string& request_id, const string& action) 
        : Type(t), RequestId(request_id), Action(action) {}

    ActionType Type; 

	string RequestId;
	string Action;

    virtual void SetState(Account& state) const;
    virtual void SetState(Connection& state) const;
    virtual void SetState(Session& state) const;
    virtual void SetState(Audio& state) const;
    virtual void SetState(Orientation& state) const;

	virtual ResponseBase* CreateResponse(const string& return_code);
};

struct AccountLoginRequest : public Request
{
    AccountLoginRequest(const string& request_id)
        : Request(AccountLogin1, request_id, AccountLogin1String) {}

    string ConnectorHandle;
    string AccountName;
    string AccountPassword;
    string AudioSessionAnswerMode;
    string AccountURI;
    string ParticipantPropertyFrequency;
    string EnableBuddiesAndPresence;
        
    void SetState(Account& state) const;

	AccountLoginResponse* CreateResponse(const string& return_code);
};

struct AccountLogoutRequest : public Request 
{
    AccountLogoutRequest(const string& request_id)
        : Request(AccountLogout1, request_id, AccountLogout1String) {}
        
    string AccountHandle;

	AccountLogoutResponse* CreateResponse(const string& return_code);
};

struct AuxCaptureAudioStartRequest : public Request 
{
    AuxCaptureAudioStartRequest(const string& request_id)
        : Request(AuxCaptureAudioStart1, request_id, AuxCaptureAudioStart1String) {}

    string Duration;

	AuxCaptureAudioStartResponse* CreateResponse(const string& return_code);
};

struct AuxCaptureAudioStopRequest : public Request 
{
    AuxCaptureAudioStopRequest(const string& request_id)
        : Request(AuxCaptureAudioStop1, request_id, AuxCaptureAudioStop1String) {}

    // No data

	AuxCaptureAudioStopResponse* CreateResponse(const string& return_code);
};

struct AuxGetCaptureDevicesRequest : public Request 
{
    AuxGetCaptureDevicesRequest(const string& request_id)
        : Request(AuxGetCaptureDevices1, request_id, AuxGetCaptureDevices1String) {}

    // No data

	AuxGetCaptureDevicesResponse* CreateResponse(const string& return_code);
};

struct AuxGetRenderDevicesRequest : public Request 
{
    AuxGetRenderDevicesRequest(const string& request_id)
        : Request(AuxGetRenderDevices1, request_id, AuxGetRenderDevices1String) {}

    // No data

	AuxGetRenderDevicesResponse* CreateResponse(const string& return_code);
};

struct AuxSetCaptureDeviceRequest : public Request 
{
    AuxSetCaptureDeviceRequest(const string& request_id)
        : Request(AuxSetCaptureDevice1, request_id, AuxSetCaptureDevice1String) {}

    // TODO: a list of Cap devs

	AuxSetCaptureDeviceResponse* CreateResponse(const string& return_code);
};

struct AuxSetMicLevelRequest : public Request 
{
    AuxSetMicLevelRequest(const string& request_id)
        : Request(AuxSetMicLevel1, request_id, AuxSetMicLevel1String) {}

    string Level;

	AuxSetMicLevelResponse* CreateResponse(const string& return_code);
};

struct AuxSetRenderDeviceRequest : public Request 
{
    AuxSetRenderDeviceRequest(const string& request_id)
        : Request(AuxSetRenderDevice1, request_id, AuxSetRenderDevice1String) {}

    // TODO: a list of Rend devs

	AuxSetRenderDeviceResponse* CreateResponse(const string& return_code);
};

struct AuxSetSpeakerLevelRequest : public Request 
{
    AuxSetSpeakerLevelRequest(const string& request_id)
        : Request(AuxSetSpeakerLevel1, request_id, AuxSetSpeakerLevel1String) {}

    string Level;

	AuxSetSpeakerLevelResponse* CreateResponse(const string& return_code);
};

struct ConnectorCreateRequest : public Request
{
    ConnectorCreateRequest(const string& request_id)
        : Request(ConnectorCreate1, request_id, ConnectorCreate1String) {}

    string ClientName;
    string AttemptStun;
    string AccountManagementServer;
    string MinimumPort;
    string MaximumPort;

	ConnectorCreateResponse* CreateResponse(const string& return_code);
};

struct ConnectorInitiateShutdownRequest : public Request 
{
    ConnectorInitiateShutdownRequest(const string& request_id)
        : Request(ConnectorInitiateShutdown1, request_id, ConnectorInitiateShutdown1String) {}

    string ConnectorHandle;

	ConnectorInitiateShutdownResponse* CreateResponse(const string& return_code);
};

struct ConnectorMuteLocalMicRequest : public Request 
{
    ConnectorMuteLocalMicRequest(const string& request_id)
        : Request(ConnectorMuteLocalMic1, request_id, ConnectorMuteLocalMic1String) {}

    string ConnectorHandle;
    string Value;
        
    void SetState (Audio& state) const;

	ConnectorMuteLocalMicResponse* CreateResponse(const string& return_code);
};

struct ConnectorMuteLocalSpeakerRequest : public Request 
{
    ConnectorMuteLocalSpeakerRequest(const string& request_id)
        : Request(ConnectorMuteLocalSpeaker1, request_id, ConnectorMuteLocalSpeaker1String) {}

    string ConnectorHandle;
    string Value;
        
    void SetState (Audio& state) const;

	ConnectorMuteLocalSpeakerResponse* CreateResponse(const string& return_code);
};

struct ConnectorSetLocalMicVolumeRequest : public Request 
{
    ConnectorSetLocalMicVolumeRequest(const string& request_id)
       : Request(ConnectorSetLocalMicVolume1, request_id, ConnectorSetLocalMicVolume1String) {}

    string ConnectorHandle;
    string Value;
        
    void SetState (Audio& state) const;

	ConnectorSetLocalMicVolumeResponse* CreateResponse(const string& return_code);
};

struct ConnectorSetLocalSpeakerVolumeRequest : public Request 
{
    ConnectorSetLocalSpeakerVolumeRequest(const string& request_id)
       : Request(ConnectorSetLocalSpeakerVolume1, request_id, ConnectorSetLocalSpeakerVolume1String) {}

    string ConnectorHandle;
    string Value;
        
    void SetState (Audio& state) const;

	ConnectorSetLocalSpeakerVolumeResponse* CreateResponse(const string& return_code);
};

struct SessionCreateRequest : public Request
{
    SessionCreateRequest(const string& request_id)
       : Request(SessionCreate1, request_id, SessionCreate1String) {}

    string AccountHandle;
    string URI;
    string Name;
    string Password;
    string JoinAudio;
    string JoinText;
    string PasswordHashAlgorithm;
        
    void SetState (Session& state) const;

	SessionCreateResponse* CreateResponse(const string& return_code);
};

struct SessionSet3DPositionRequest : public Request
{
    SessionSet3DPositionRequest(const string& request_id)
       : Request(SessionSet3DPosition1, request_id, SessionSet3DPosition1String) {}

    string SessionHandle;
    Orientation speaker;
    Orientation listener;
        
    void SetState (Orientation& state) const;

// TODO : unnecessary?
//	SessionSet3DPositionResponse* CreateResponse();
};

struct SessionSetParticipantMuteForMeRequest : public Request
{
    SessionSetParticipantMuteForMeRequest(const string& request_id)
       : Request(SessionSetParticipantMuteForMe1, request_id, SessionSetParticipantMuteForMe1String) {}

    string SessionHandle;
    string ParticipantURI;
    string Mute;

// TODO : unnecessary?
//	SessionSetParticipantMuteForMeResponse* CreateResponse();
};

struct SessionSetParticipantVolumeForMeRequest : public Request
{
    SessionSetParticipantVolumeForMeRequest(const string& request_id)
       : Request(SessionSetParticipantVolumeForMe1, request_id, SessionSetParticipantVolumeForMe1String) {}

    string SessionHandle;
    string ParticipantURI;
    string Volume;

	SessionSetParticipantVolumeForMeResponse* CreateResponse(const string& return_code);
};

struct SessionTerminateRequest : public Request
{
    SessionTerminateRequest(const string& request_id)
       : Request(SessionTerminate1, request_id, SessionTerminate1String) {}

    string SessionHandle;

	SessionTerminateResponse* CreateResponse(const string& return_code);
};

struct SessionConnectRequest : public Request
{
    SessionConnectRequest(const string& request_id)
       : Request(SessionConnect1, request_id, SessionConnect1String) {}

    string SessionHandle;
    string AudioMedia;

	SessionConnectResponse* CreateResponse(const string& return_code);
};

//typedef list <const Request *> RequestQueue;

//=============================================================================
// Event Objects

struct EventBase
{
    EventBase (const string& t) 
        : type (t) {}

    string type;

	//virtual string ToString() const;
};


struct LoginStateChangeEvent : public EventBase
{
    LoginStateChangeEvent (const string& t="LoginStateChangeEvent")
        : EventBase (t) {}

    string AccountHandle;
    string StatusCode;
    string StatusString;
    string State;

	string ToString();
};

struct SessionNewEvent : public EventBase
{
    SessionNewEvent (const string& t="SessionNewEvent")
        : EventBase (t) {}

    string AccountHandle;
    string SessionHandle;
	string State;	// is not written the document.
    string URI;
    string Name;
    string IsChannel;
    string AudioMedia;
	string HasText;		// is not written the document.
	string HasAudio;	// is not written the document.
	string HasVideo;	// is not written the document.

	string ToString();
};

struct SessionStateChangeEvent : public EventBase
{
    SessionStateChangeEvent (const string& t="SessionStateChangeEvent")
        : EventBase (t) {}

    string SessionHandle;
    string StatusCode;
    string StatusString;
    string State;
    string URI;
    string IsChannel;
    string ChannelName;

	string ToString();
};

struct ParticipantStateChangeEvent : public EventBase
{
    ParticipantStateChangeEvent (const string& t="ParticipantStateChangeEvent")
        : EventBase (t) {}

    string StatusCode;
    string StatusString;
    string State;
    string ParticipantURI;
    string AccountName;
    string DisplayName;
    string ParticipantType;

	string ToString();
};

struct ParticipantPropertiesEvent : public EventBase
{
    ParticipantPropertiesEvent (const string& t="ParticipantPropertiesEvent")
        : EventBase (t) {}

    string SessionHandle;
    string ParticipantURI;
    string IsLocallyMuted;
    string IsModeratorMuted;
    string Volume;
    string Energy;

	string ToString();
};

struct AuxAudioPropertiesEvent : public EventBase
{
    AuxAudioPropertiesEvent (const string& t="AuxAudioPropertiesEvent")
        : EventBase (t) {}

    string MicIsActive;
    string MicEnergy;
    string MicVolume;
    string SpeakerVolume;

	string ToString();
};


//=============================================================================
// Parse Objects
class RequestParser
{
    public:
        RequestParser (const char *message);
        auto_ptr <const Request> Parse ();

//		static void parse_VoiceInfoResponse(SIPServerInfo& sinfo);

    private:
        string get_request_id_ ();
        string get_action_ ();
        ActionType get_action_type_ ();

        const TiXmlElement* get_root_element_ (const string&);
        const TiXmlElement* get_element_ (const TiXmlElement*, const string&);
        string get_root_text_ (const string&);
        string get_text_ (const TiXmlElement*, const string&);

        void parse_vector_ (const TiXmlElement *vector, float buf [3]);
        Orientation parse_voice_orientation_ (const TiXmlElement *orient);

    private:
        auto_ptr <const Request> parse_AccountLogin_ ();
        auto_ptr <const Request> parse_AccountLogout_ ();
        auto_ptr <const Request> parse_AuxCaptureAudioStart_ ();
        auto_ptr <const Request> parse_AuxCaptureAudioStop_ ();
        auto_ptr <const Request> parse_AuxGetCaptureDevices_ ();
        auto_ptr <const Request> parse_AuxGetRenderDevices_ ();
        auto_ptr <const Request> parse_AuxSetCaptureDevice_ ();
        auto_ptr <const Request> parse_AuxSetMicLevel_ ();
        auto_ptr <const Request> parse_AuxSetRenderDevice_ ();
        auto_ptr <const Request> parse_AuxSetSpeakerLevel_ ();
        auto_ptr <const Request> parse_ConnectorCreate_ ();
        auto_ptr <const Request> parse_ConnectorInitiateShutdown_ ();
        auto_ptr <const Request> parse_ConnectorMuteLocalMic_ ();
        auto_ptr <const Request> parse_ConnectorMuteLocalSpeaker_ ();
        auto_ptr <const Request> parse_ConnectorSetLocalMicVolume_ ();
        auto_ptr <const Request> parse_ConnectorSetLocalSpeakerVolume_ ();
        auto_ptr <const Request> parse_SessionCreate_ ();
        auto_ptr <const Request> parse_SessionConnect_ ();
        auto_ptr <const Request> parse_SessionSet3DPosition_ ();
        auto_ptr <const Request> parse_SessionSetParticipantMuteForMe_ ();
        auto_ptr <const Request> parse_SessionSetParticipantVolumeForMe_ ();
        auto_ptr <const Request> parse_SessionTerminate_ ();

    private:
		string requestid_;
        ActionType type_;
        TiXmlDocument doc_;

    private:
        RequestParser ();
        RequestParser (const RequestParser&);
        void operator= (const RequestParser&);
};

//=============================================================================
// Functions

//string format_response (const ResponseMessage& resp);
//string format_response (const EventMessage& ev);

#endif //_MESSAGING_HPP_
