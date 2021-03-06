/* session_state.cpp -- session state module
 *
 *			Copyright 2008, 3di.jp Inc
 */

#include <boost/lexical_cast.hpp>

#include "main.h"
#include "state.hpp"
#include "server_util.hpp"

//=============================================================================
// Session Idle
//=============================================================================
SessionIdleState::SessionIdleState(my_context ctx) :
        my_base(ctx), // required because we call context() from a constructor
        machine(context<SessionMachine>()) 
{
	g_logger->Debug("STATE") << "SessionIdle entered" << endl;
}

SessionIdleState::~SessionIdleState() 
{
    g_logger->Debug("STATE") << "SessionIdle exited" << endl;
}

result SessionIdleState::react(const SessionCreateEvent& ev) {

	SIPUserInfo sinfo;
	stringstream ss;

	g_logger->Debug("STATE") << "SessionIdle react (SessionCreateEvent)" << endl;

    ev.message->SetState(machine.info->session); // this should have done the parsing

	try 
	{
		g_logger->Info("SESSION") << "Conference URI = " << machine.info->session.uri << endl;
		g_logger->Info("SESSION") << "Account ID = " << machine.info->account->id << endl;
		
		ConnectorInfo *con = glb_server->getConnector();
		SIPConference *psc = machine.info->account->sipconf;

        if (psc != NULL) {

			ss.str(machine.info->session.uri);
			ss >> sinfo;

			SIPServerInfo sipinfo;

#ifndef _3DI
			// access to the voip frontend
			ServerUtil::getServerInfo(con->voiceserver_url + sinfo.name, sipinfo);

			// connect to conference
            psc->Join(
				sipinfo.sipuri, machine.info->account->id, 
                &machine.info->id,
                machine.info->session.connectedType
                );
#else
            // connect to conference
            // Swap the domain from what is coming in session.uri to
            // what we saved earlier in Account.Login
            string domain (take_after_ ("@", machine.info->account->account.uri));
            string invite (take_before_ ("@", machine.info->session.uri));
			
            psc->Join(
                invite+"@"+domain,		// Saved PBX URI from Account.Login
                machine.info->account->id, 
                &machine.info->id,
                machine.info->session.connectedType
                );
#endif
			con->session.registId(machine.info->id, machine.info->handle);
            ((SessionCreateResponse *)ev.result)->SessionHandle = machine.info->handle;
        }
    } catch (runtime_error& e) {
        cerr << e.what() << endl;
        post_event(SessionTerminateEvent());
    }

    return transit<SessionCallingState>();
}

result SessionIdleState::react(const DialIncomingEvent& ev) 
{
	g_logger->Debug("STATE") << "SessionIdle react (DialIncomingEvent)" << endl;
    return transit<SessionIncomingState>();
}

//=============================================================================
// Session Terminated
//=============================================================================
SessionTerminatedState::SessionTerminatedState(my_context ctx) :
        my_base(ctx), // required because we call context() from a constructor
        machine(context<SessionMachine>()) 
{
	g_logger->Debug("STATE") << "SessionTerminated entered" << endl;

	SessionStateChangeEvent sessionStateEvent;
    sessionStateEvent.SessionHandle = machine.info->handle;
	sessionStateEvent.StatusCode = sessionStateEvent.OKCode;
	sessionStateEvent.StatusString = sessionStateEvent.OKString;
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

SessionTerminatedState::~SessionTerminatedState() 
{
	g_logger->Debug("STATE") << "SessionTerminated exited" << endl;
	g_logger->Terse("STATE") << "=======  SESSION  ======== Session Terminate Destructor" << endl;

}

//=============================================================================
// Session Calling
//=============================================================================
SessionCallingState::SessionCallingState(my_context ctx) :
        my_base(ctx), // required because we call context() from a constructor
        machine(context<SessionMachine>()) 
{
	g_logger->Debug("STATE") << "SessionCalling entered" << endl;
	g_logger->Terse("STATE") << "=======  SESSION  ======== CallingState Constructor" << endl;
}

SessionCallingState::~SessionCallingState() 
{
	g_logger->Debug("STATE") << "SessionCalling exited" << endl;
	g_logger->Terse("STATE") << "=======  SESSION  ======== CallingState Destructor" << endl;
}

result SessionCallingState::react(const SessionTerminateEvent& ev) 
{
	g_logger->Debug("STATE") << "SessionCalling react (SessionTerminateEvent)" << endl;

    g_logger->Terse("STATE") << "=======  SESSION  ======== CallingState Terminate" << endl;
    //Added July 7, 2009
    SIPConference *psc = machine.info->account->sipconf;	
    if (psc != NULL) 
    {
        psc->Leave(machine.info->id);
    }
    //Added July 7, 2009

    return transit<SessionTerminatedState>();
}

result SessionCallingState::react(const DialEarlyEvent& ev) 
{
	g_logger->Debug("STATE") << "SessionCalling react (DialEarlyEvent)" << endl;
    return transit<SessionEarlyState>();
}

result SessionCallingState::react(const DialConnectingEvent& ev) 
{
	g_logger->Debug("STATE") << "SessionCalling react (DialConnectingEvent)" << endl;
    return transit<SessionConnectingState>();
}

result SessionCallingState::react(const DialSucceedEvent& ev) 
{
	g_logger->Debug("STATE") << "SessionCalling react (DialSucceededEvent)" << endl;
    return transit<SessionConfirmedState>();
}

result SessionCallingState::react(const DialDisconnectedEvent& ev)
{
	g_logger->Debug("STATE") << "SessionCalling react (DialDisconnectedEvent)" << endl;
    return transit<SessionTerminatedState>();
}

//=============================================================================
// Session Incoming
//=============================================================================
SessionIncomingState::SessionIncomingState(my_context ctx) :
        my_base(ctx), // required because we call context() from a constructor
        machine(context<SessionMachine>()) 
{
	g_logger->Debug("STATE") << "SessionIncoming entered" << endl;	

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

	glb_server->userURI = "sip:" + uinfo.name + "@" + uinfo.domain;
	glb_server->participantURI = uinfo.name;
    glb_server->Send(sessionNewEvent.ToString());

	ConnectorInfo *con = glb_server->getConnector();

    if (machine.info->account->sipconf != NULL) {
		// answer to incoming request
        machine.info->account->sipconf->Answer(machine.info->id, 180);
	}
}

SessionIncomingState::~SessionIncomingState() 
{
	g_logger->Debug("STATE") << "SessionIncoming exited" << endl;	
}

result SessionIncomingState::react(const SessionTerminateEvent& ev) 
{
	g_logger->Debug("STATE") << "SessionIncoming react (SessionTerminateEvent)" << endl;	
    return transit<SessionTerminatedState>();
}

result SessionIncomingState::react(const SessionConnectEvent& ev) 
{
	g_logger->Debug("STATE") << "SessionIncoming react (SessionConnectEvent)" << endl;

	SIPConference *psc = machine.info->account->sipconf;

    if (psc != NULL) {
		// answer to incoming request
        psc->Answer(machine.info->id, 200);
	}

    return transit<SessionConnectingState>();
}

result SessionIncomingState::react(const DialEarlyEvent& ev) 
{
	g_logger->Debug("STATE") << "SessionIncoming react (DialEarlyEvent)" << endl;
    return transit<SessionEarlyState>();
}

result SessionIncomingState::react(const DialConnectingEvent& ev) 
{
    g_logger->Debug("STATE") << "SessionIncoming react (DialConnectingEvent)" << endl;
    return transit<SessionConnectingState>();
}

result SessionIncomingState::react(const DialSucceedEvent& ev) 
{
    g_logger->Debug("STATE") << "SessionIncoming react (DialSucceedEvent)" << endl;
    return transit<SessionConfirmedState>();
}

result SessionIncomingState::react(const DialDisconnectedEvent& ev) 
{
	g_logger->Debug("STATE") << "SessionIncoming react (DialDisconnectedEvent)" << endl;
    return transit<SessionTerminatedState>();
}

//=============================================================================
// Session Early
//=============================================================================
SessionEarlyState::SessionEarlyState(my_context ctx) :
        my_base(ctx), // required because we call context() from a constructor
        machine(context<SessionMachine>()) 
{
	g_logger->Debug("STATE") << "SessionEarly entered" << endl;
}

SessionEarlyState::~SessionEarlyState() 
{
	g_logger->Debug("STATE") << "SessionEarly exited" << endl;
}

result SessionEarlyState::react(const SessionTerminateEvent& ev) 
{
    g_logger->Debug("STATE") << "SessionEarly react (SessionTerminateEvent)" << endl;
 
    SIPConference *psc = machine.info->account->sipconf;
    if (psc != NULL) 
    {
        // disconnect
        psc->Leave(machine.info->id);
    }
    return transit<SessionTerminatedState>();
}

result SessionEarlyState::react(const SessionConnectEvent& ev) 
{
	g_logger->Debug("STATE") << "SessionEarly react (SessionConnectEvent)" << endl;

	SIPConference *psc = machine.info->account->sipconf;

    if (psc != NULL) {
		// answer to incoming request
        psc->Answer(machine.info->id, 200);
	}

    return transit<SessionConnectingState>();
}

result SessionEarlyState::react(const DialConnectingEvent& ev) 
{
	g_logger->Debug("STATE") << "SessionEarly react (DialConnectingEvent)" << endl;
    return transit<SessionConnectingState>();
}

result SessionEarlyState::react(const DialSucceedEvent& ev) 
{
	g_logger->Debug("STATE") << "SessionEarly react (DialSucceedEvent)" << endl;
    return transit<SessionConfirmedState>();
}

result SessionEarlyState::react(const DialDisconnectedEvent& ev) 
{
    g_logger->Debug("STATE") << "SessionEarly react (DialDisconnectedEvent)" << endl;
    return transit<SessionTerminatedState>();
}

//=============================================================================
// Session Connecting
//=============================================================================
SessionConnectingState::SessionConnectingState(my_context ctx) :
        my_base(ctx), // required because we call context() from a constructor
        machine(context<SessionMachine>()) 
{
	g_logger->Debug("STATE") << "SessionConnecting entered" << endl;
    g_logger->Terse("STATE") << "=======  SESSION  ======== ConnectingState Constructor" << endl;
}

SessionConnectingState::~SessionConnectingState() 
{
    g_logger->Debug("STATE") << "SessionConnecting exited" << endl;
    g_logger->Terse("STATE") << "=======  SESSION  ======== ConnectingState Destructor" << endl;
}

result SessionConnectingState::react(const SessionTerminateEvent& ev) 
{
    g_logger->Debug("STATE") << "SessionConnecting react (SessionTerminateEvent)" << endl;
    g_logger->Terse("STATE") << "=======  SESSION  ======== ConnectingState Terminate" << endl;
    return transit<SessionTerminatedState>();
}

result SessionConnectingState::react(const DialSucceedEvent& ev) 
{
    g_logger->Debug("STATE") << "SessionConnecting react (DialSucceedEvent)" << endl;
    return transit<SessionConfirmedState>();
}

result SessionConnectingState::react(const DialDisconnectedEvent& ev) 
{
    g_logger->Debug("STATE") << "SessionConnecting react (DialDisconnectedEvent)" << endl;
    return transit<SessionTerminatedState>();
}

//=============================================================================
// VolumeCheckingThread
//=============================================================================
void VolumeCheckingThread::operator()()
{
	unsigned int tx_level;
	unsigned int rx_level;

	float mic_volume = 0.0f;

	pj_thread_desc desc;
	pj_thread_t *thread;
	pj_thread_register("volume_polling", desc, &thread);

	while (!stopped)
	{
		pj_status_t status;
		pjsua_call_info ci;

		if (call_id >= 0)
		{
			status = pjsua_call_get_info((pjsua_call_id)call_id, &ci);
			if (status == PJ_SUCCESS)
			{
				status = pjsua_conf_get_signal_level(ci.conf_slot, &tx_level, &rx_level);
				if (status == PJ_SUCCESS)
				{
					ConnectorInfo *con = glb_server->getConnector();
					mic_volume = con->audio.mic_volume;

					mic_volume = ((mic_volume+100) / 200) * 100;

					ParticipantPropertiesEvent partPropEvent;
					partPropEvent.SessionHandle = handle;
					partPropEvent.ParticipantURI = glb_server->participantURI;
					partPropEvent.IsLocallyMuted = "false";
					partPropEvent.IsModeratorMuted = "false";
					char buf[255];
					sprintf(buf, "%d", (int)mic_volume);
					partPropEvent.Volume = buf;

					// tx_level is a value between 0 and 255
					// energy is between 0 and 1.0

					sprintf(buf, "%1.2f", (((float)tx_level) / 256.0));
					partPropEvent.Energy = buf;

					if (tx_level > 20)
					{
						partPropEvent.IsSpeaking = "true";
					}

					glb_server->Send (partPropEvent.ToString());
				}
			}
		}
		    
		pj_thread_sleep(200);
	}
}

//=============================================================================
// Session Confirmed
//=============================================================================
SessionConfirmedState::SessionConfirmedState(my_context ctx) :
        my_base(ctx), // required because we call context() from a constructor
        machine(context<SessionMachine>()) 
{
	g_logger->Debug("STATE") << "SessionConfirmed entered" << endl;

    // Mute mic on first connect
    ConnectorInfo *con = glb_server->getConnector();
    SIPConference *psc = machine.info->account->sipconf;

    if (psc != NULL && con->audio.mic_mute) 
    {
        psc->AdjustTranVolume(machine.info->id, 0.0f);
    }

	SessionStateChangeEvent sessionStateEvent;
    sessionStateEvent.SessionHandle = machine.info->handle;
	sessionStateEvent.StatusCode = sessionStateEvent.OKCode;
	sessionStateEvent.StatusString = sessionStateEvent.OKString;
    sessionStateEvent.State = "4";
    //sessionStateEvent.URI = "";
    //sessionStateEvent.IsChannel = "";
    //sessionStateEvent.ChannelName = "";
    glb_server-> Send (sessionStateEvent.ToString());

    ParticipantStateChangeEvent partStateEvent;
	partStateEvent.StatusCode = sessionStateEvent.OKCode;
	partStateEvent.StatusString = sessionStateEvent.OKString;
    partStateEvent.State = "7";
    partStateEvent.ParticipantURI = glb_server->participantURI;
    //partStateEvent.AccountName = "";
    partStateEvent.DisplayName = "";
    partStateEvent.ParticipantType = "0";
    glb_server-> Send (partStateEvent.ToString());

    ParticipantPropertiesEvent partPropEvent;
    partPropEvent.SessionHandle = machine.info->handle;
    //partPropEvent.ParticipantURI = "";
    //partPropEvent.IsLocallyMuted = "";
    //partPropEvent.IsModeratorMuted = "";
    //partPropEvent.Volume = "";
    //partPropEvent.Energy = "";
    glb_server-> Send (partPropEvent.ToString());

	if (g_config->Version < 122)
	{
	}
	else
	{
		MediaStreamUpdatedEvent mediaStreamUpdatedEvent;
		mediaStreamUpdatedEvent.StatusCode = "0";
		mediaStreamUpdatedEvent.StatusString = "OK";
		mediaStreamUpdatedEvent.SessionHandle = machine.info->handle;
		mediaStreamUpdatedEvent.SessionGroupHandle = "";
		mediaStreamUpdatedEvent.State = "2";

		glb_server->Send(mediaStreamUpdatedEvent.ToString());
	}

	// TODO: New thread for volume checking
	volumeCheckingThread.call_id = machine.info->id;
	volumeCheckingThread.handle = machine.info->handle;
	volumeCheckingThread.stopped = false;

	thr = boost::thread(boost::ref(volumeCheckingThread));
}

SessionConfirmedState::~SessionConfirmedState() 
{
    g_logger->Debug("STATE") << "SessionConfirmed exited" << endl;
}

result SessionConfirmedState::react(const SessionTerminateEvent& ev) 
{
	g_logger->Debug("STATE") << "SessionConfirmed react (SessionTerminateEvent)" << endl;

	SIPConference *psc = machine.info->account->sipconf;

	volumeCheckingThread.stopped = true;
	thr.join();

    if (psc != NULL) {
        // disconnect
        psc->Leave(machine.info->id);
    }

    return transit<SessionTerminatedState>();
}

result SessionConfirmedState::react(const DialDisconnectedEvent& ev) 
{
	g_logger->Debug("STATE") << "SessionConfirmed react (DialDisconnectedEvent)" << endl;
    return transit<SessionTerminatedState>();
}


result SessionConfirmedState::react(const AudioEvent& ev) 
{
	g_logger->Debug("STATE") << "SessionConfirmed react (AudioEvent)" << endl;

    float mic_volume = 0.0f;
    float spk_volume = 0.0f;

	ConnectorInfo *con = glb_server->getConnector();
	SIPConference *psc = machine.info->account->sipconf;

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

    if (psc != NULL) {
        psc->AdjustTranVolume(machine.info->id, mic_volume);
		psc->AdjustRecvVolume(machine.info->id, spk_volume);
    }

    return discard_event();
}

result SessionConfirmedState::react(const PositionEvent& ev) 
{
	g_logger->Debug("STATE") << "SessionConfirmed react (PositionEvent)" << endl;

    // TODO

    return discard_event();
}

// v1.22
result SessionConfirmedState::react(const SessionMediaDisconnectEvent& ev) 
{
	g_logger->Debug("STATE") << "SessionConfirmed react (SessionMediaDisconnectEvent)" << endl;

	volumeCheckingThread.stopped = true;

	// We are disconnecting from this session
	SessionStateChangeEvent sessionStateEvent;
    sessionStateEvent.SessionHandle = machine.info->handle;
	sessionStateEvent.StatusCode = sessionStateEvent.OKCode;
	sessionStateEvent.StatusString = sessionStateEvent.OKString;
    sessionStateEvent.State = "5";

    //sessionStateEvent.URI = "";
    //sessionStateEvent.IsChannel = "";
    //sessionStateEvent.ChannelName = "";

    glb_server-> Send (sessionStateEvent.ToString());

    return discard_event();
}