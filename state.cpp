/* state.cpp -- state module
 *
 *			Ryan McDougall -- 2008
 */

#include <main.h>
#include <state.hpp>
#include <boost/lexical_cast.hpp>

//=============================================================================
StartState::StartState (my_context ctx) : 
    my_base (ctx), // required because we call context() from a constructor
    machine (context <StateMachine>())
{ 
    cout << "start entered" << endl; 
}

StartState::~StartState () 
{ 
    cout << "start exited" << endl; 
}

result StartState::react (const ConnectionEvent& ev) 
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

    glb_server-> Send (mesg.str());

    return transit <ConnectorState> ();
}

//=============================================================================
ConnectorState::ConnectorState (my_context ctx) : 
    my_base (ctx), // required because we call context() from a constructor
    machine (context <StateMachine>())
{ 
    cout << "connector entered" << endl; 
}

ConnectorState::~ConnectorState () 
{ 
    cout << "connector exited" << endl; 
}

result ConnectorState::react (const AccountEvent& ev) 
{
    cout << "connector state react" << endl;
    ostringstream mesg;

    ResponseMessage login (AccountLogin1String);
    EventMessage loginstate (LoginStateChangeEventString);

    login.handle = "xxxx";
    loginstate.state = 1;
    loginstate.status_code = 200;

    mesg << format_response (login);
    mesg << endmesg;
    mesg << format_response (loginstate);
    mesg << endmesg;

    glb_server-> Send (mesg.str());

    return transit <AccountState> ();
}

result ConnectorState::react (const StopEvent& ev) 
{ 
    return transit <StopState> (); 
}

//=============================================================================
AccountState::AccountState (my_context ctx) : 
    my_base (ctx), // required because we call context() from a constructor
    machine (context <StateMachine>())
{ 
    cout << "account entered" << endl; 
}

AccountState::~AccountState () 
{ 
    cout << "account exited" << endl; 
}

result AccountState::react (const SessionEvent& ev) 
{ 
    cout << "account state react" << endl;
    ostringstream mesg;

    ResponseMessage sessioncreate (SessionCreate1String);
    EventMessage sessionstate (SessionStateChangeEventString),
                 partstate (ParticipantStateChangeEventString),
                 partprop (ParticipantPropertiesEventString);

    string handle ("xxxx");
    sessioncreate.handle = handle;

    sessionstate.state = 4;
    sessionstate.params.push_back (make_pair ("SessionHandle", handle));

    partstate.state = 7;
    partstate.params.push_back (make_pair ("SessionHandle", handle));

    mesg << format_response (sessioncreate);
    mesg << endmesg;
    mesg << format_response (sessionstate);
    mesg << endmesg;
    mesg << format_response (partstate);
    mesg << endmesg;
    mesg << format_response (partprop);
    mesg << endmesg;

    glb_server-> Send (mesg.str());

    return transit <DialingState> ();
}

result AccountState::react (const StopEvent& ev) 
{ 
    return transit <StopState> (); 
}

//=============================================================================
DialingState::DialingState (my_context ctx) : 
    my_base (ctx), // required because we call context() from a constructor
    machine (context <StateMachine>())
{ 
    cout << "dialing entered" << endl; 

    // connect to conference
    try
    {
        glb_server-> Conference ("sip.conf");
    }
    catch (runtime_error& e)
    {
        cerr << e.what() << endl;
    }
}

DialingState::~DialingState () 
{ 
    cout << "dialing exited" << endl; 
}

result DialingState::react (const DialSucceedEvent& ev) 
{ 
    return transit <SessionState> (); 
}

result DialingState::react (const DialFailedEvent& ev) 
{ 
    return transit <StopState> (); 
}

//=============================================================================
SessionState::SessionState (my_context ctx) : 
    my_base (ctx), // required because we call context() from a constructor
    machine (context <StateMachine>())
{ 
    cout << "session entered" << endl; 
}

SessionState::~SessionState () 
{ 
    ostringstream mesg;

    ResponseMessage sessionterm (SessionTerminate1String);
    EventMessage sessionstate (SessionStateChangeEventString);

    mesg << format_response (sessionterm);
    mesg << endmesg;
    mesg << format_response (sessionstate);
    mesg << endmesg;

    cout << mesg.str() << endl;

    glb_server-> Send (mesg.str());

    cout << "session exited" << endl; 
}

result SessionState::react (const PositionEvent& ev) 
{ 
    const Request *req (ev.messages.back()); // last msg triggered the event
    if (req->type == SessionSet3DPosition1)
    {
        const PositionSetRequest *pos 
            (static_cast <const PositionSetRequest*> (req));

        machine.voice.speaker = pos->speaker;
        machine.voice.listener = pos->listener;
    }

    return discard_event (); 
}

result SessionState::react (const HardwareSetEvent& ev) 
{ 
    const Request *req (ev.messages.back()); // last msg triggered the event
    const HardwareSetRequest *set 
        (static_cast <const HardwareSetRequest*> (req));

    try
    {
        stringstream ss;
        ss << boolalpha;

        if ((req->type == ConnectorMuteLocalMic1) 
                && set->micmute.size())
        {
            ss.str (set->micmute);
            ss >> machine.voice.mic_mute;
        }

        
        if ((req->type == ConnectorSetLocalMicVolume1) 
                && set->micvol.size())
        {
            ss.str (set->micvol);
            ss >> machine.voice.mic_volume;
        }
        
        if ((req->type == ConnectorMuteLocalSpeaker1) 
                && set->speakermute.size())
        {
            ss.str (set->speakermute);
            ss >> machine.voice.speaker_mute;
        }
        
        if ((req->type == ConnectorSetLocalSpeakerVolume1) 
                && set->speakervol.size())
        {
            ss.str (set->speakervol);
            ss >> machine.voice.speaker_volume;
        }
    }
    
    catch (boost::bad_lexical_cast& e) 
    {
        // we're assuming that failure to parse one legit 
        // field is a sign that something serious is broken so just discard
        cout << "unable to parse hardware setting " << e.what() << endl;
    }

    return discard_event (); 
}

result SessionState::react (const StopEvent& ev) 
{ 
    return transit <StopState> (); 
}

//=============================================================================
StopState::StopState (my_context ctx) : 
    my_base (ctx), // required because we call context() from a constructor
    machine (context <StateMachine>())
{ 
    ostringstream mesg;

    EventMessage loginstate (LoginStateChangeEventString);
    ResponseMessage accountlogout (AccountLogout1String),
                    connshutdown (ConnectorInitiateShutdown1String);

    mesg << format_response (accountlogout);
    mesg << endmesg;
    mesg << format_response (loginstate);
    mesg << endmesg;
    mesg << format_response (connshutdown);
    mesg << endmesg;

    cout << mesg.str() << endl;

    glb_server-> Send (mesg.str());

    cout << "stopped entered" << endl; 
}

StopState::~StopState () 
{ 
    cout << "stopped exited" << endl; 
}
