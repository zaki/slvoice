/* session_state.cpp -- session state module
 *
 *			Copyright 2008, 3di.jp Inc
 */

#include <main.h>
#include <state.hpp>
#include <boost/lexical_cast.hpp>

//=============================================================================
// Session Idle
//=============================================================================
SessionIdleState::SessionIdleState(my_context ctx) :
        my_base(ctx), // required because we call context() from a constructor
        machine(context<SessionMachine>()) {
    VFVW_LOG("SessionIdle entered");
}

SessionIdleState::~SessionIdleState() {
    VFVW_LOG("SessionIdle exited");
}

result SessionIdleState::react(const SessionCreateEvent& ev) {

	SIPUserInfo sinfo;
	stringstream ss;

	VFVW_LOG("SessionIdle react (SessionCreateEvent)");

    ev.message->SetState(machine.info->session); // this should have done the parsing

    try {
		VFVW_LOG("conference uri = %s", machine.info->session.uri.c_str());
		VFVW_LOG("account id = %d", machine.info->account->id);
		
		ConnectorInfo *con = glb_server->getConnector();

        if (con->sipconf != NULL) {

			ss.str(machine.info->session.uri);
			ss >> sinfo;

			// connect to conference
            con->sipconf->Join(
				sinfo, machine.info->account->id, 
				&machine.info->id);

			con->session.registId(machine.info->id, machine.info->handle);
            ((SessionCreateResponse *)ev.result)->SessionHandle = machine.info->handle;
        }
    } catch (runtime_error& e) {
        cerr << e.what() << endl;
        post_event(SessionTerminateEvent());
    }

    return transit<SessionCallingState>();
}

result SessionIdleState::react(const DialIncomingEvent& ev) {
    VFVW_LOG("SessionIdle react (DialIncomingEvent)");
    return transit<SessionIncomingState>();
}

//=============================================================================
// Session Terminated
//=============================================================================
SessionTerminatedState::SessionTerminatedState(my_context ctx) :
        my_base(ctx), // required because we call context() from a constructor
        machine(context<SessionMachine>()) {
    VFVW_LOG("SessionTerminated entered");

	SessionStateChangeEvent sessionStateEvent;
    sessionStateEvent.SessionHandle = machine.info->handle;
    //sessionStateEvent.StatusCode = "";
    //sessionStateEvent.StatusString = "";
    sessionStateEvent.State = "5";
    //sessionStateEvent.URI = "";
    //sessionStateEvent.IsChannel = "";
    //sessionStateEvent.ChannelName = "";

    glb_server-> Send (sessionStateEvent.ToString());

	// enqueue the session remove event
	SessionRemoveEvent *removeEvent = new SessionRemoveEvent();
	removeEvent->call_id = machine.info->id;
	removeEvent->session_handle = machine.info->handle;

	g_eventManager.blockQueue.enqueue(removeEvent);
}

SessionTerminatedState::~SessionTerminatedState() {
    VFVW_LOG("SessionTerminated exited");
}

//=============================================================================
// Session Calling
//=============================================================================
SessionCallingState::SessionCallingState(my_context ctx) :
        my_base(ctx), // required because we call context() from a constructor
        machine(context<SessionMachine>()) {
    VFVW_LOG("SessionCalling entered");
}

SessionCallingState::~SessionCallingState() {
    VFVW_LOG("SessionCalling exited");
}

result SessionCallingState::react(const SessionTerminateEvent& ev) {
    VFVW_LOG("SessionCalling react (SessionTerminateEvent)");
    return transit<SessionTerminatedState>();
}

result SessionCallingState::react(const DialEarlyEvent& ev) {
    VFVW_LOG("SessionCalling react (DialEarlyEvent)");
    return transit<SessionEarlyState>();
}

result SessionCallingState::react(const DialConnectingEvent& ev) {
    VFVW_LOG("SessionCalling react (DialConnectingEvent)");
    return transit<SessionConnectingState>();
}

result SessionCallingState::react(const DialSucceedEvent& ev) {
    VFVW_LOG("SessionCalling react (DialSucceedEvent)");
    return transit<SessionConfirmedState>();
}

result SessionCallingState::react(const DialDisconnectedEvent& ev) {
    VFVW_LOG("SessionCalling react (DialDisconnectedEvent)");
    return transit<SessionTerminatedState>();
}

//=============================================================================
// Session Incoming
//=============================================================================
SessionIncomingState::SessionIncomingState(my_context ctx) :
        my_base(ctx), // required because we call context() from a constructor
        machine(context<SessionMachine>()) {
    VFVW_LOG("SessionIncoming entered");

	SIPUserInfo uinfo;
	stringstream ss(machine.info->incoming_uri);

	ss >> uinfo;

	SessionNewEvent sessionNewEvent;
    sessionNewEvent.AccountHandle = machine.info->account->handle;
    sessionNewEvent.SessionHandle = machine.info->handle;
    sessionNewEvent.State = "0";
	sessionNewEvent.URI = "sip:" + uinfo.name + "@" + uinfo.domain;
	sessionNewEvent.Name = uinfo.name;
    sessionNewEvent.IsChannel = "true";
//    sessionNewEvent.AudioMedia = "default";
    sessionNewEvent.HasText = "false";
    sessionNewEvent.HasAudio = "true";
    sessionNewEvent.HasVideo = "false";

    glb_server->Send(sessionNewEvent.ToString());

	ConnectorInfo *con = glb_server->getConnector();

    if (con->sipconf != NULL) {
		// answer to incoming request
        con->sipconf->Answer(machine.info->id, 180);
	}
}

SessionIncomingState::~SessionIncomingState() {
    VFVW_LOG("SessionIncoming exited");
}

result SessionIncomingState::react(const SessionTerminateEvent& ev) {
    VFVW_LOG("SessionIncoming react (SessionTerminateEvent)");
    return transit<SessionTerminatedState>();
}

result SessionIncomingState::react(const SessionConnectEvent& ev) {
    VFVW_LOG("SessionIncoming react (SessionConnectEvent)");

	ConnectorInfo *con = glb_server->getConnector();

    if (con->sipconf != NULL) {
		// answer to incoming request
        con->sipconf->Answer(machine.info->id, 200);
	}

    return transit<SessionConnectingState>();
}

result SessionIncomingState::react(const DialEarlyEvent& ev) {
    VFVW_LOG("SessionIncoming react (DialEarlyEvent)");
    return transit<SessionEarlyState>();
}

result SessionIncomingState::react(const DialConnectingEvent& ev) {
    VFVW_LOG("SessionIncoming react (DialConnectingEvent)");
    return transit<SessionConnectingState>();
}

result SessionIncomingState::react(const DialSucceedEvent& ev) {
    VFVW_LOG("SessionIncoming react (DialSucceedEvent)");
    return transit<SessionConfirmedState>();
}

result SessionIncomingState::react(const DialDisconnectedEvent& ev) {
    VFVW_LOG("SessionIncoming react (DialDisconnectedEvent)");
    return transit<SessionTerminatedState>();
}

//=============================================================================
// Session Early
//=============================================================================
SessionEarlyState::SessionEarlyState(my_context ctx) :
        my_base(ctx), // required because we call context() from a constructor
        machine(context<SessionMachine>()) {
    VFVW_LOG("SessionEarly entered");
}

SessionEarlyState::~SessionEarlyState() {
    VFVW_LOG("SessionEarly exited");
}

result SessionEarlyState::react(const SessionTerminateEvent& ev) {
    VFVW_LOG("SessionEarly react (SessionTerminateEvent)");
    return transit<SessionTerminatedState>();
}

result SessionEarlyState::react(const SessionConnectEvent& ev) {
    VFVW_LOG("SessionEarly react (SessionConnectEvent)");

	ConnectorInfo *con = glb_server->getConnector();

    if (con->sipconf != NULL) {
		// answer to incoming request
        con->sipconf->Answer(machine.info->id, 200);
	}

    return transit<SessionConnectingState>();
}

result SessionEarlyState::react(const DialConnectingEvent& ev) {
    VFVW_LOG("SessionEarly react (DialConnectingEvent)");
    return transit<SessionConnectingState>();
}

result SessionEarlyState::react(const DialSucceedEvent& ev) {
    VFVW_LOG("SessionEarly react (DialSucceedEvent)");
    return transit<SessionConfirmedState>();
}

result SessionEarlyState::react(const DialDisconnectedEvent& ev) {
    VFVW_LOG("SessionEarly react (DialDisconnectedEvent)");
    return transit<SessionTerminatedState>();
}

//=============================================================================
// Session Connecting
//=============================================================================
SessionConnectingState::SessionConnectingState(my_context ctx) :
        my_base(ctx), // required because we call context() from a constructor
        machine(context<SessionMachine>()) {
    VFVW_LOG("SessionConnecting entered");
}

SessionConnectingState::~SessionConnectingState() {
    VFVW_LOG("SessionConnecting exited");
}

result SessionConnectingState::react(const SessionTerminateEvent& ev) {
    VFVW_LOG("SessionConnecting react (SessionTerminateEvent)");
    return transit<SessionTerminatedState>();
}

result SessionConnectingState::react(const DialSucceedEvent& ev) {
    VFVW_LOG("SessionConnecting react (DialSucceedEvent)");
    return transit<SessionConfirmedState>();
}

result SessionConnectingState::react(const DialDisconnectedEvent& ev) {
    VFVW_LOG("SessionConnecting react (DialDisconnectedEvent)");
    return transit<SessionTerminatedState>();
}

//=============================================================================
// Session Confirmed
//=============================================================================
SessionConfirmedState::SessionConfirmedState(my_context ctx) :
        my_base(ctx), // required because we call context() from a constructor
        machine(context<SessionMachine>()) {
    VFVW_LOG("SessionConfirmed entered");

	SessionStateChangeEvent sessionStateEvent;
    sessionStateEvent.SessionHandle = machine.info->handle;
    //sessionStateEvent.StatusCode = "";
    //sessionStateEvent.StatusString = "";
    sessionStateEvent.State = "4";
    //sessionStateEvent.URI = "";
    //sessionStateEvent.IsChannel = "";
    //sessionStateEvent.ChannelName = "";
    glb_server-> Send (sessionStateEvent.ToString());

    ParticipantStateChangeEvent partStateEvent;
    //sessionStateEvent.StatusCode = "";
    //sessionStateEvent.StatusString = "";
    partStateEvent.State = "7";
    //sessionStateEvent.ParticipantURI = "";
    //sessionStateEvent.AccountName = "";
    //sessionStateEvent.DisplayName = "";
    //sessionStateEvent.ParticipantType = "";
    glb_server-> Send (partStateEvent.ToString());

    ParticipantPropertiesEvent partPropEvent;
    sessionStateEvent.SessionHandle = machine.info->handle;
    //partPropEvent.ParticipantURI = "";
    //partPropEvent.IsLocallyMuted = "";
    //partPropEvent.IsModeratorMuted = "";
    //partPropEvent.Volume = "";
    //partPropEvent.Energy = "";
    glb_server-> Send (partPropEvent.ToString());

}

SessionConfirmedState::~SessionConfirmedState() {
    VFVW_LOG("SessionConfirmed exited");
}

result SessionConfirmedState::react(const SessionTerminateEvent& ev) {
    VFVW_LOG("SessionConfirmed react (SessionTerminateEvent)");

	ConnectorInfo *con = glb_server->getConnector();

    if (con->sipconf != NULL) {
        // disconnect
        con->sipconf->Leave(machine.info->id);
    }

    return transit<SessionTerminatedState>();
}

result SessionConfirmedState::react(const DialDisconnectedEvent& ev) {
    VFVW_LOG("SessionConfirmed react (DialDisconnectedEvent)");
    return transit<SessionTerminatedState>();
}


result SessionConfirmedState::react(const AudioEvent& ev) {
    VFVW_LOG("SessionConfirmed react (AudioEvent)");

    float mic_volume = 0.0f;
    float spk_volume = 0.0f;

	ConnectorInfo *con = glb_server->getConnector();

    // adjust mic volume
    if (!con->audio.mic_mute) {
        mic_volume = con->audio.mic_volume;
        // adjust between SL and PJSIP
        mic_volume = (mic_volume - VFVW_SL_VOLUME_MIN)
                     * VFVW_PJ_VOLUME_RANGE / VFVW_SL_VOLUME_RANGE;
    }

    // adjust speaker volume
    if (!con->audio.speaker_mute) {
        spk_volume = con->audio.speaker_volume;
        // adjust between SL and PJSIP
        spk_volume = (spk_volume - VFVW_SL_VOLUME_MIN)
                     * VFVW_PJ_VOLUME_RANGE / VFVW_SL_VOLUME_RANGE;
    }

    if (con->sipconf != NULL) {
        con->sipconf->AdjustTranVolume(machine.info->id, mic_volume);
		con->sipconf->AdjustRecvVolume(machine.info->id, spk_volume);
    }

    return discard_event();
}

result SessionConfirmedState::react(const PositionEvent& ev) {
    VFVW_LOG("SessionConfirmed react (PositionEvent)");

    // TODO

    return discard_event();
}

