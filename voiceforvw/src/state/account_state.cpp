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
        machine(context<AccountMachine>()) 
{
    g_logger->Debug("STATE") << "AccountLogout entered" << endl;
}

AccountLogoutState::~AccountLogoutState() 
{
    g_logger->Debug("STATE") << "AccountLogout exited" << endl;
}

result AccountLogoutState::react(const AccountLoginEvent& ev) 
{
    g_logger->Debug("STATE") << "AccountLogout react (AccountLoginEvent)" << endl;

    ev.message->SetState(machine.info->account);

	ConnectorInfo *con = glb_server->getConnector();

    SIPUserInfo uinfo;
	SIPServerInfo sipinfo;
	string url;

#ifndef _3DI
	// access to the voip frontend
	ServerUtil::getServerInfo(con->voiceserver_url + machine.info->account.name, sipinfo);

    machine.info->sipconf = new SIPConference(sipinfo);

    uinfo.name = machine.info->account.name;
    uinfo.password = machine.info->account.password;
	uinfo.sipuri = sipinfo.sipuri;

	g_logger->Info("AccountLogoutState") << "sipuri   : "  << sipinfo.sipuri << endl;
	g_logger->Info("AccountLogoutState") << "proxyuri : "  << sipinfo.proxyuri << endl;
	g_logger->Info("AccountLogoutState") << "reguri   : "  << sipinfo.reguri << endl;
	g_logger->Info("AccountLogoutState") << "domain   : " << uinfo.domain << endl;
#else
    sipinfo.sipuri = machine.info->account.uri;
    sipinfo.proxyuri = "";

    string domain (take_after_ ("@", sipinfo.sipuri));

    sipinfo.reguri = "sip:" + domain;  

    machine.info->sipconf = new SIPConference(sipinfo);

    uinfo.name = machine.info->account.name;
    uinfo.password = machine.info->account.password;
    uinfo.sipuri = machine.info->account.uri;

    g_logger->Info("AccountLogoutState") << "sipuri   : "  << uinfo.sipuri << endl;
    g_logger->Info("AccountLogoutState") << "proxyuri : CANNOT USE" << endl;
    g_logger->Info("AccountLogoutState") << "reguri   : CANNOT USE" << endl;
    g_logger->Info("AccountLogoutState") << "domain   : " << uinfo.domain << endl;
#endif

	// sending REGISTER
    machine.info->sipconf->Register(uinfo, &machine.info->id);

	g_logger->Info("AccountLogoutState") << "Account ID = " << machine.info->id << " Handle = " << machine.info->handle << endl;

	con->account.registId(machine.info->id, machine.info->handle);
    ((AccountLoginResponse *)ev.result)->AccountHandle = machine.info->handle;

	glb_server->userURI = uinfo.sipuri;
	glb_server->participantURI = uinfo.name;

    return transit<AccountRegisteringState>();
}

//=============================================================================
// Account Registering
//=============================================================================
AccountRegisteringState::AccountRegisteringState(my_context ctx) :
        my_base(ctx), // required because we call context() from a constructor
        machine(context<AccountMachine>()) 
{
	g_logger->Debug("STATE") << "AccountRegistering entered" << endl;
}

AccountRegisteringState::~AccountRegisteringState() 
{
    g_logger->Debug("STATE") << "AccountRegistering exited" << endl;
}

result AccountRegisteringState::react(const RegSucceedEvent& ev) 
{
    g_logger->Debug("STATE") << "AccountRegistering react (RegSucceededEvent)" << endl;
    return transit<AccountLoginState>();
}

result AccountRegisteringState::react(const RegFailedEvent& ev) 
{
	g_logger->Debug("STATE") << "AccountRegistering react (RegFailedEvent)" << endl;
    return transit<AccountLogoutState>();
}


//=============================================================================
// Account Login
//=============================================================================
AccountLoginState::AccountLoginState(my_context ctx) :
        my_base(ctx), // required because we call context() from a constructor
        machine(context<AccountMachine>()) 
{
	g_logger->Debug("STATE") << "AccountLogin entered" << endl;

	// Send LoginStateChangeEvent
    LoginStateChangeEvent loginStateEvent;
    loginStateEvent.AccountHandle = machine.info->handle;
	loginStateEvent.StatusCode = loginStateEvent.OKCode; // "200";	// v1.22
	loginStateEvent.StatusString = loginStateEvent.OKString; // "OK";
    loginStateEvent.State = "1";

    glb_server->Send(loginStateEvent.ToString());
}

AccountLoginState::~AccountLoginState() 
{
    g_logger->Debug("STATE") << "AccountLogin exited" << endl;
}

result AccountLoginState::react(const AccountLogoutEvent& ev) 
{
	g_logger->Debug("STATE") << "AccountLogin react (AccountLogoutEvent)" << endl;

    if (machine.info->sipconf != NULL) {
		// sending unREG (Expires=0)
        machine.info->sipconf->UnRegister(machine.info->id);
    }

    return transit<AccountUnregisteringState>();
}

result AccountLoginState::react(const RegFailedEvent& ev) 
{
	g_logger->Debug("STATE") << "AccountLogin react (RegFailedEvent)" << endl;
    return transit<AccountLogoutState>();
}

//=============================================================================
// Account Unregistering
//=============================================================================
AccountUnregisteringState::AccountUnregisteringState(my_context ctx) :
        my_base(ctx), // required because we call context() from a constructor
        machine(context<AccountMachine>()) 
{
	g_logger->Debug("STATE") << "AccountUnregistering entered" << endl;

    // Send LoginStateChangeEvent
    LoginStateChangeEvent loginStateEvent;
    loginStateEvent.AccountHandle = machine.info->handle;
	loginStateEvent.StatusCode = loginStateEvent.OKCode; //"200"; // v1.22
	loginStateEvent.StatusString = loginStateEvent.OKString; //"OK";
    loginStateEvent.State = "0";

    glb_server->Send(loginStateEvent.ToString());
}

AccountUnregisteringState::~AccountUnregisteringState() 
{
	g_logger->Debug("STATE") << "AccountUnregistering exited" << endl;
}

result AccountUnregisteringState::react(const RegSucceedEvent& ev) 
{
	g_logger->Debug("STATE") << "AccountUnregistering react (RegSucceedEvent)" << endl;
	
	// enqueue the account remove event
	AccountRemoveEvent *removeEvent = new AccountRemoveEvent();
	removeEvent->acc_id = machine.info->id;
	removeEvent->account_handle = machine.info->handle;

    delete machine.info->sipconf;
    machine.info->sipconf = NULL;

	g_eventManager.blockQueue.enqueue(removeEvent);

    return transit<AccountLogoutState>();
}

result AccountUnregisteringState::react(const RegFailedEvent& ev)
{
	g_logger->Debug("STATE") << "AccountUnregistering react (RegFailedEvent)" << endl;

	// enqueue the account remove event
	AccountRemoveEvent *removeEvent = new AccountRemoveEvent();
	removeEvent->acc_id = machine.info->id;
	removeEvent->account_handle = machine.info->handle;

    delete machine.info->sipconf;
    machine.info->sipconf = NULL;

	g_eventManager.blockQueue.enqueue(removeEvent);
	
	return transit<AccountLogoutState>();
}

