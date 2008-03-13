/* state.cpp -- state module
 *
 *			Ryan McDougall -- 2008
 */

#include <main.h>
#include <state.hpp>

//=============================================================================
StartState::StartState () { cout << "start entered" << endl; }
StartState::~StartState () { cout << "start exited" << endl; }

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

    context <StateMachine>().server-> Send (mesg.str());

    return transit <ConnectorState> ();
}

//=============================================================================
ConnectorState::ConnectorState () { cout << "connector entered" << endl; }
ConnectorState::~ConnectorState () { cout << "connector exited" << endl; }

result ConnectorState::react (const AccountEvent& ev) 
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

    context <StateMachine>().server-> Send (mesg.str());

    return transit <AccountState> ();
}

//=============================================================================
AccountState::AccountState () { cout << "account entered" << endl; }
AccountState::~AccountState () { cout << "account exited" << endl; }

result AccountState::react (const SessionEvent& ev) 
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

    context <StateMachine>().server-> Send (mesg.str());

    return transit <SessionState> ();
}

//=============================================================================
SessionState::SessionState (my_context ctx)
: my_base (ctx) // required because we call context() from a constructor
{ 
    // TODO: this function should echo the current session info
    cout << "session entered" << endl; 

    // connect to conference
    SIPServerInfo sinfo ("conference", "10.8.1.149");
    SIPUserInfo uinfo ("test0", "example.com");
    SIPConference *bridge;

    context <StateMachine>().bridge.reset 
        (bridge = new SIPConference (sinfo));

    bridge-> Register (uinfo);
    bridge-> Join ();
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

    context <StateMachine>().server-> Send (mesg.str());

    cout << "session exited" << endl; 
}

result SessionState::react (const StopEvent& ev) { return transit <StopState> (); }

//=============================================================================
StopState::StopState () 
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

    //context <StateMachine>().server-> Send (mesg.str());

    cout << "stopped entered" << endl; 
}

StopState::~StopState () { cout << "stopped exited" << endl; }
