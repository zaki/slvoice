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
// Request Objects

struct Request 
{
        Request (ActionType t, int id = 0) 
            : type_ (t), sequenceid_ (id) {}

        int SequenceId;
        ActionType Type; 

        virtual void SetState (Account& state) const;
        virtual void SetState (Connector& state) const;
        virtual void SetState (Session& state) const;
        virtual void SetState (Audio& state) const;
        virtual void SetState (Orientation& state) const;
};

struct AccountLoginRequest : public Request
{
        AccountLoginRequest (int id = 0)
            : Request (AccountLogin1, id) {}

        string ConnectorHandle;
        string AccountName;
        string AccountPassword;
        string AudioSessionAnswerMode;
        string AccountURI;
        string ParticipantPropertyFrequency;
        string EnableBuddiesAndPresence;
        
        void SetState (Account& state) const;
};

struct AccountLogoutRequest : public Request 
{
        AccountLogoutRequest (int id = 0)
            : Request (AccountLogout1, id) {}
        
        string AccountHandle;
};

struct AuxCaptureAudioStartRequest : public Request 
{
        AuxCaptureAudioStartRequest (int id = 0)
            : Request (AuxCaptureAudioStart1, id) {}

        string Duration;
};

struct AuxCaptureAudioStopRequest : public Request 
{
        AuxCaptureAudioStopRequest (int id = 0)
            : Request (AuxCaptureAudioStop1, id) {}
        
        // No data
};

struct AuxGetCaptureDevicesRequest : public Request 
{
        AuxGetCaptureDevicesRequest (int id = 0)
            : Request (AuxGetCaptureDevices1, id) {}
       
        // No data
};

struct AuxGetRenderDevicesRequest : public Request 
{
        AuxGetRenderDevicesRequest (int id = 0)
            : Request (AuxGetRenderDevices1, id) {}
        
        // No data
};

struct AuxSetCaptureDeviceRequest : public Request 
{
        AuxSetCaptureDeviceRequest (int id = 0)
            : Request (AuxSetCaptureDevice1, id) {}

        // TODO: a list of Cap devs
};

struct AuxSetMicLevelRequest : public Request 
{
        AuxSetMicLevelRequest (int id = 0)
            : Request (AuxSetMicLevel1, id) {}

        string Level;
};

struct AuxSetRenderDeviceRequest : public Request 
{
        AuxSetRenderDeviceRequest (int id = 0)
            : Request (AuxSetRenderDevice1, id) {}

        // TODO: a list of Rend devs
};

struct AuxSetMicLevelRequest : public Request 
{
        AuxSetMicLevelRequest (int id = 0)
            : Request (AuxSetMicLevel1, id) {}

        string Level;
};

struct AuxSetSpeakerLevelRequest : public Request 
{
        AuxSetSpeakerLevelRequest (int id = 0)
            : Request (AuxSetSpeakerLevel1, id) {}

        string Level;
};

struct ConnectorCreateRequest : public Request
{
        ConnectorCreateRequest (int id = 0)
            : Request (ConnectorCreate1, id) {}

        string ClientName;
        string AttemptStun;
        string AccountManagementServer;
        string MinimumPort;
        string MaximumPort;
};

struct ConnectorInitiateShutdownRequest : public Request 
{
        ConnectorInitiateShutdownRequest (int id = 0)
            : Request (ConnectorInitiateShutdown1, id) {}

        string ConnectorHandle;
};

struct ConnectorMuteLocalMicRequest : public Request 
{
        ConnectorMuteLocalMicRequest (int id = 0)
            : Request (ConnectorMuteLocalMic1, id) {}

        string ConnectorHandle;
        string Value;
        
        void SetState (Audio& state) const;
};

struct ConnectorMuteLocalSpeakerRequest : public Request 
{
        ConnectorMuteLocalSpeakerRequest (int id = 0)
            : Request (ConnectorMuteLocalSpeaker1, id) {}

        string ConnectorHandle;
        string Value;
        
        void SetState (Audio& state) const;
};

struct ConnectorSetLocalMicVolumeRequest : public Request 
{
        ConnectorSetLocalMicVolumeRequest (int id = 0)
            : Request (ConnectorSetLocalMicVolume1, id) {}

        string ConnectorHandle;
        string Value;
        
        void SetState (Audio& state) const;
};

struct ConnectorSetLocalSpeakerVolumeRequest : public Request 
{
        ConnectorSetLocalSpeakerVolumeRequest (int id = 0)
            : Request (ConnectorSetLocalSpeakerVolume1, id) {}

        string ConnectorHandle;
        string Value;
        
        void SetState (Audio& state) const;
};

struct SessionCreateRequest : public Request
{
        SessionCreateRequest (int id = 0)
            : Request (SessionCreate1, id) {}

        string AccountHandle;
        string URI;
        string Name;
        string Password;
        string JoinAudio;
        string JoinText;
        string PasswordHashAlgorithm;
        
        void SetState (Session& state) const;
};

struct SessionSet3DPositionRequest : public Request
{
        SessionSet3DPositionRequest  (int id = 0)
            : Request (SessionSet3DPosition1, id) {}

        Orientation speaker;
        Orientation listener;
        
        void SetState (Orientation& state) const;
};

struct SessionSetParticipantMuteForMeRequest : public Request
{
        SessionSetParticipantMuteForMeRequest (int id = 0)
            : Request (SessionSetParticipantMuteForMe1, id) {}

        string SessionHandle;
        string ParticipantURI;
        string Mute;
};

struct SessionSetParticipantVolumeForMeRequest : public Request
{
        SessionSetParticipantVolumeForMeRequest (int id = 0)
            : Request (SessionSetParticipantVolumeForMe1, id) {}

        string SessionHandle;
        string ParticipantURI;
        string Volume;
};

struct SessionTerminateRequest : public Request
{
        SessionTerminateRequest (int id = 0)
            : Request (SessionTerminate1, id) {}

        string SessionHandle;
};

typedef list <const Request *> RequestQueue;


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
// Parse Objects
class RequestParser
{
    public:
        RequestParser (char *message);
        auto_ptr <const Request> Parse ();

    private:
        int get_sequence_id_ ();
        ActionType get_action_type_ ();

        TiXmlElement* get_root_element_ (const string&);
        TiXmlElement* get_element_ (const TiXmlElement*, const string&);
        string get_root_text_ (const string&);
        string get_text_ (const TiXmlElement*, const string&);

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
        auto_ptr <const Request> parse_SessionSet3DPosition_ ();
        auto_ptr <const Request> parse_SessionSetParticipantMuteForMe_ ();
        auto_ptr <const Request> parse_SessionSetParticipantVolumeForMe_ ();
        auto_ptr <const Request> parse_SessionTerminate_ ();

    private:
        int sequenceid_;
        ActionType type_;
        TiXmlDocument doc_;

    private:
        RequestParser ();
        RequestParser (const RequestParser&);
        void operation= (const RequestParser&);
};

//=============================================================================
// Functions

string format_response (const ResponseMessage& resp);
string format_response (const EventMessage& ev);

#endif //_MESSAGING_HPP_
