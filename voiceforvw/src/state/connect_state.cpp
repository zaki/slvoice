/* connect_state.cpp -- connector state module
 *
 *			Copyright 2008, 3di.jp Inc
 */

#include "main.h"
#include "state.hpp"
#include "server_util.hpp"

#include <boost/lexical_cast.hpp>

//=============================================================================
// Connector Idle
//=============================================================================
ConnectorIdleState::ConnectorIdleState(my_context ctx) :
        my_base(ctx), // required because we call context() from a constructor
        machine(context<ConnectorMachine>()) {
    VFVW_LOG("ConnectorIdle entered");
}

ConnectorIdleState::~ConnectorIdleState() {
    VFVW_LOG("ConnectorIdle exited");
}

result ConnectorIdleState::react(const InitializeEvent& ev) {
    VFVW_LOG("ConnectorIdle react (InitializeEvent)");

	machine.info->voiceserver_url = g_config->VoiceServerURI;

	VFVW_LOG("voip frontend url = %s", machine.info->voiceserver_url.c_str());	

    machine.info->handle = VFVW_CONNECTOR_HANDLE;

    ((ConnectorCreateResponse *)ev.result)->ConnectorHandle = machine.info->handle;
    //((ConnectorCreateResponse *)ev.result)->VersionID = "";	// TODO

    return transit<ConnectorActiveState>();
}

//=============================================================================
// Connector Active
//=============================================================================
ConnectorActiveState::ConnectorActiveState(my_context ctx) :
        my_base(ctx), // required because we call context() from a constructor
        machine(context<ConnectorMachine>()) {
    VFVW_LOG("ConnectorActive entered");
}

ConnectorActiveState::~ConnectorActiveState() {
    VFVW_LOG("ConnectorActive exited");
}

result ConnectorActiveState::react(const ShutdownEvent& ev) {
    VFVW_LOG("ConnectorActive react (ShutdownEvent)");

    machine.info->handle = "";

    return transit<ConnectorIdleState>();
}

result ConnectorActiveState::react(const AudioEvent& ev) {
    VFVW_LOG("ConnectorActive react (AudioEvent)");

    ev.message->SetState(machine.info->audio);

	// control all sessions
	machine.info->session.controlAudioLevel();

    return discard_event();
}
