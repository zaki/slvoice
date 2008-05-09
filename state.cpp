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
    return transit <ConnectionState> ();
}

//=============================================================================
ConnectionState::ConnectionState (my_context ctx) : 
    my_base (ctx), // required because we call context() from a constructor
    machine (context <StateMachine>())
{ 
    cout << "connector entered" << endl; 
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
}

ConnectionState::~ConnectionState () 
{ 
    cout << "connector exited" << endl; 
}

result ConnectionState::react (const AccountEvent& ev) 
{
    cout << "connector state react" << endl;
    return transit <AccountState> ();
}

result ConnectionState::react (const StopEvent& ev) 
{ 
    return transit <StopState> (); 
}

//=============================================================================
AccountState::AccountState (my_context ctx) : 
    my_base (ctx), // required because we call context() from a constructor
    machine (context <StateMachine>())
{ 
    cout << "account entered" << endl; 
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
}

AccountState::~AccountState () 
{ 
    cout << "account exited" << endl; 
}

result AccountState::react (const SessionEvent& ev) 
{ 
    cout << "account state react" << endl;

    ev.messages.back()-> SetState (machine.session);
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
        post_event (StopEvent ());
    }
}

DialingState::~DialingState () 
{ 
    cout << "dialing exited" << endl; 
}

result DialingState::react (const StopEvent& ev)
{
    return transit <StopState> ();
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
    //ev.messages.back()-> SetState (machine.);
    return discard_event (); 
}

result SessionState::react (const AudioEvent& ev) 
{ 
    ev.messages.back()-> SetState (machine.audio);
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
