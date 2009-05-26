/* account_state.cpp -- account state module
 *
 *			Copyright 2008, 3di.jp Inc
 */

#include <boost/lexical_cast.hpp>

#include "main.h"
#include "state.hpp"
#include "server_util.hpp"

//=============================================================================
// Account Logout
//=============================================================================
AccountLogoutState::AccountLogoutState(my_context ctx) :
        my_base(ctx), // required because we call context() from a constructor
        machine(context<AccountMachine>()) {
    VFVW_LOG("AccountLogout entered");
}

AccountLogoutState::~AccountLogoutState() {
    VFVW_LOG("AccountLogout exited");
}

result AccountLogoutState::react(const AccountLoginEvent& ev) {
    VFVW_LOG("AccountLogout react (AccountLoginEvent)");

    ev.message->SetState(machine.info->account);

	ConnectorInfo *con = glb_server->getConnector();

    SIPUserInfo uinfo;
	SIPServerInfo sipinfo;
	string url;

	// access to the voip frontend
	ServerUtil::getServerInfo(con->voiceserver_url + machine.info->account.name, sipinfo);

	VFVW_LOG("sipuri   : %s", sipinfo.sipuri.c_str());
	VFVW_LOG("proxyuri : %s", sipinfo.proxyuri.c_str());
	VFVW_LOG("reguri   : %s", sipinfo.reguri.c_str());

    machine.info->sipconf = new SIPConference(sipinfo);

    uinfo.name = machine.info->account.name;
    uinfo.password = machine.info->account.password;
	uinfo.sipuri = sipinfo.sipuri;

	VFVW_LOG("domain : %s", uinfo.domain.c_str());

	// sending REGISTER
    machine.info->sipconf->Register(uinfo, &machine.info->id);

	VFVW_LOG("account id = %d, handle = %s", machine.info->id, machine.info->handle.c_str());
	con->account.registId(machine.info->id, machine.info->handle);
    ((AccountLoginResponse *)ev.result)->AccountHandle = machine.info->handle;

	glb_server->userURI = uinfo.sipuri;

    return transit<AccountRegisteringState>();
}

//=============================================================================
// Account Registering
//=============================================================================
AccountRegisteringState::AccountRegisteringState(my_context ctx) :
        my_base(ctx), // required because we call context() from a constructor
        machine(context<AccountMachine>()) {
    VFVW_LOG("AccountRegistering entered");
}

AccountRegisteringState::~AccountRegisteringState() {
    VFVW_LOG("AccountRegistering exited");
}

result AccountRegisteringState::react(const RegSucceedEvent& ev) {
    VFVW_LOG("AccountRegistering react (RegSucceedEvent)");
    return transit<AccountLoginState>();
}

result AccountRegisteringState::react(const RegFailedEvent& ev) {
    VFVW_LOG("AccountRegistering react (RegFailedEvent)");
    return transit<AccountLogoutState>();
}


//=============================================================================
// Account Login
//=============================================================================
AccountLoginState::AccountLoginState(my_context ctx) :
        my_base(ctx), // required because we call context() from a constructor
        machine(context<AccountMachine>()) {

	VFVW_LOG("AccountLogin entered");

	// Send LoginStateChangeEvent
    LoginStateChangeEvent loginStateEvent;
    loginStateEvent.AccountHandle = machine.info->handle;
	loginStateEvent.StatusCode = loginStateEvent.OKCode; // "200";	// v1.22
	loginStateEvent.StatusString = loginStateEvent.OKString; // "OK";
    loginStateEvent.State = "1";

    glb_server->Send(loginStateEvent.ToString());
}

AccountLoginState::~AccountLoginState() {
    VFVW_LOG("AccountLogin exited");
}

result AccountLoginState::react(const AccountLogoutEvent& ev) {
    VFVW_LOG("AccountLogin react (AccountLogoutEvent)");

    if (machine.info->sipconf != NULL) {
		// sending unREG (Expires=0)
        machine.info->sipconf->UnRegister(machine.info->id);
    }

    return transit<AccountUnregisteringState>();
}

result AccountLoginState::react(const RegFailedEvent& ev) {
    VFVW_LOG("AccountLogin react (RegFailedEvent)");
    return transit<AccountLogoutState>();
}

//=============================================================================
// Account Unregistering
//=============================================================================
AccountUnregisteringState::AccountUnregisteringState(my_context ctx) :
        my_base(ctx), // required because we call context() from a constructor
        machine(context<AccountMachine>()) {
    VFVW_LOG("AccountUnregistering entered");

    // Send LoginStateChangeEvent
    LoginStateChangeEvent loginStateEvent;
    loginStateEvent.AccountHandle = machine.info->handle;
	loginStateEvent.StatusCode = loginStateEvent.OKCode; //"200"; // v1.22
	loginStateEvent.StatusString = loginStateEvent.OKString; //"OK";
    loginStateEvent.State = "0";

    glb_server->Send(loginStateEvent.ToString());
}

AccountUnregisteringState::~AccountUnregisteringState() {
    VFVW_LOG("AccountUnregistering exited");
}

result AccountUnregisteringState::react(const RegSucceedEvent& ev) {
    VFVW_LOG("AccountUnregistering react (RegSucceedEvent)");

	// enqueue the account remove event
	AccountRemoveEvent *removeEvent = new AccountRemoveEvent();
	removeEvent->acc_id = machine.info->id;
	removeEvent->account_handle = machine.info->handle;

    delete machine.info->sipconf;
    machine.info->sipconf = NULL;

	g_eventManager.blockQueue.enqueue(removeEvent);

    return transit<AccountLogoutState>();
}

result AccountUnregisteringState::react(const RegFailedEvent& ev) {
    VFVW_LOG("AccountUnregistering react (RegFailedEvent)");

	// enqueue the account remove event
	AccountRemoveEvent *removeEvent = new AccountRemoveEvent();
	removeEvent->acc_id = machine.info->id;
	removeEvent->account_handle = machine.info->handle;

    delete machine.info->sipconf;
    machine.info->sipconf = NULL;

	g_eventManager.blockQueue.enqueue(removeEvent);
	
	return transit<AccountLogoutState>();
}

