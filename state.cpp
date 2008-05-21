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
	VFVW_LOG("start entered");
}

StartState::~StartState () 
{ 
	VFVW_LOG("start exited");
}

result StartState::react (const ConnectionEvent& ev) 
{ 
	VFVW_LOG("start state react");
    return transit <ConnectionState> ();
}

//=============================================================================
ConnectionState::ConnectionState (my_context ctx) : 
    my_base (ctx), // required because we call context() from a constructor
    machine (context <StateMachine>())
{ 
	VFVW_LOG("connector entered");

	ostringstream mesg;

    ResponseMessage getcap (AuxGetCaptureDevices1String), 
                    getrend (AuxGetRenderDevices1String), 
                    conncreate (ConnectorCreate1String);

    conncreate.handle = VFVW_HANDLE;

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
	VFVW_LOG("connector exited");
}

result ConnectionState::react (const AccountEvent& ev) 
{
	VFVW_LOG("connector state react (AccountEvent)");

	ev.messages.back()-> SetState (machine.account);

    return transit <AccountState> ();
}

result ConnectionState::react (const StopEvent& ev) 
{ 
	VFVW_LOG("connector state react (StopEvent)");
    return transit <StopState> (); 
}

//=============================================================================
AccountState::AccountState (my_context ctx) : 
    my_base (ctx), // required because we call context() from a constructor
    machine (context <StateMachine>())
{ 
	VFVW_LOG("account entered");

	ostringstream mesg;

    ResponseMessage login (AccountLogin1String);
    EventMessage loginstate (LoginStateChangeEventString);

    login.handle = VFVW_HANDLE;
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
	VFVW_LOG("account exited");
}

result AccountState::react (const SessionEvent& ev) 
{ 
	VFVW_LOG("account state react (SessionEvent)");

    ev.messages.back()-> SetState (machine.session); // this should have done the parsing

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
	VFVW_LOG("dialing entered");

    // connect to conference
    try
    {
        glb_server-> Conference (machine.account, machine.session);
    }
    catch (runtime_error& e)
    {
        cerr << e.what() << endl;
        post_event (StopEvent ());
    }
}

DialingState::~DialingState () 
{ 
	VFVW_LOG("dialing exited");
}

result DialingState::react (const StopEvent& ev)
{
	VFVW_LOG("dialing state react (StopEvent)");
    return transit <StopState> ();
}

result DialingState::react (const DialSucceedEvent& ev) 
{ 
	VFVW_LOG("dialing state react (DialSucceedEvent)");
    return transit <SessionState> (); 
}

result DialingState::react (const DialFailedEvent& ev) 
{ 
	VFVW_LOG("dialing state react (DialFailedEvent)");
    return transit <StopState> (); 
}

//=============================================================================
SessionState::SessionState (my_context ctx) : 
    my_base (ctx), // required because we call context() from a constructor
    machine (context <StateMachine>())
{ 
	VFVW_LOG("session entered");

	ostringstream mesg;

    ResponseMessage sessioncreate (SessionCreate1String);
    EventMessage sessionstate (SessionStateChangeEventString),
                 partstate (ParticipantStateChangeEventString),
                 partprop (ParticipantPropertiesEventString);

    string handle (VFVW_HANDLE);
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

	VFVW_LOG("session exited");
}

result SessionState::react (const PositionEvent& ev) 
{ 
	VFVW_LOG("session state react (PositionEvent)");
    //ev.messages.back()-> SetState (machine.);
    return discard_event (); 
}

result SessionState::react (const AudioEvent& ev) 
{ 
	VFVW_LOG("session state react (AudioEvent)");
    ev.messages.back()-> SetState (machine.audio);

	glb_server->AudioControl(machine.session, machine.audio);

    return discard_event (); 
}

result SessionState::react (const StopEvent& ev) 
{ 
	VFVW_LOG("session state react (StopEvent)");
    return transit <StopState> (); 
}

//=============================================================================
StopState::StopState (my_context ctx) : 
    my_base (ctx), // required because we call context() from a constructor
    machine (context <StateMachine>())
{ 
	VFVW_LOG("stop entered");

	ostringstream mesg;

	// disconnect
    glb_server->Disconnect();

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
}

StopState::~StopState () 
{ 
	VFVW_LOG("stop exited");
}
