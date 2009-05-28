/* state.hpp -- state definition
 *
 *			Copyright 2008, 3di.jp Inc
 */

#ifndef _STATE_HPP_
#define _STATE_HPP_

#include <main.h>

#include <boost/mpl/list.hpp>
#include <boost/statechart/event.hpp>
#include <boost/statechart/custom_reaction.hpp>
#include <boost/statechart/state_machine.hpp>
#include <boost/statechart/state.hpp>
#include <boost/ref.hpp>
#include <boost/thread.hpp>
#include <boost/thread/condition.hpp>

using namespace boost::statechart;

// ------------------------------ Session ------------------------------
struct SessionIdleState;
class SessionInfo;

struct SessionMachine : state_machine <SessionMachine, SessionIdleState> 
{
	SessionInfo* info;
};

struct SessionIdleState : state <SessionIdleState, SessionMachine> 
{
    typedef boost::mpl::list<
		custom_reaction<SessionCreateEvent>, 
		custom_reaction<DialIncomingEvent>> reactions;

    SessionIdleState(my_context ctx);
    ~SessionIdleState();

    result react(const SessionCreateEvent& ev);
    result react(const DialIncomingEvent& ev);

    SessionMachine& machine;
};

struct SessionTerminatedState : state <SessionTerminatedState, SessionMachine> 
{
    SessionTerminatedState(my_context ctx);
    ~SessionTerminatedState();

    SessionMachine& machine;
};

struct SessionCallingState : state <SessionCallingState, SessionMachine> 
{
    typedef boost::mpl::list<
		custom_reaction<SessionTerminateEvent>, 
		custom_reaction<DialEarlyEvent>, 
		custom_reaction<DialConnectingEvent>, 
		custom_reaction<DialSucceedEvent>, 
		custom_reaction<DialDisconnectedEvent>> reactions;

    SessionCallingState(my_context ctx);
    ~SessionCallingState();

    result react(const SessionTerminateEvent& ev);
    result react(const DialEarlyEvent& ev);
    result react(const DialConnectingEvent& ev);
    result react(const DialSucceedEvent& ev);
    result react(const DialDisconnectedEvent& ev);

    SessionMachine& machine;
};

struct SessionIncomingState : state <SessionIncomingState, SessionMachine> 
{
    typedef boost::mpl::list<
		custom_reaction<SessionTerminateEvent>, 
		custom_reaction<SessionConnectEvent>, 
		custom_reaction<DialEarlyEvent>, 
		custom_reaction<DialConnectingEvent>, 
		custom_reaction<DialSucceedEvent>, 
		custom_reaction<DialDisconnectedEvent>> reactions;

    SessionIncomingState(my_context ctx);
    ~SessionIncomingState();

    result react(const SessionTerminateEvent& ev);
    result react(const SessionConnectEvent& ev);
    result react(const DialEarlyEvent& ev);
    result react(const DialConnectingEvent& ev);
    result react(const DialSucceedEvent& ev);
    result react(const DialDisconnectedEvent& ev);

    SessionMachine& machine;
};

struct SessionEarlyState : state <SessionEarlyState, SessionMachine> 
{
    typedef boost::mpl::list<
		custom_reaction<SessionTerminateEvent>, 
		custom_reaction<SessionConnectEvent>, 
		custom_reaction<DialConnectingEvent>, 
		custom_reaction<DialSucceedEvent>, 
		custom_reaction<DialDisconnectedEvent>> reactions;

    SessionEarlyState(my_context ctx);
    ~SessionEarlyState();

    result react(const SessionTerminateEvent& ev);
    result react(const SessionConnectEvent& ev);
    result react(const DialConnectingEvent& ev);
    result react(const DialSucceedEvent& ev);
    result react(const DialDisconnectedEvent& ev);

    SessionMachine& machine;
};

struct SessionConnectingState : state <SessionConnectingState, SessionMachine> 
{
    typedef boost::mpl::list<
		custom_reaction<SessionTerminateEvent>, 
		custom_reaction<DialSucceedEvent>, 
		custom_reaction<DialDisconnectedEvent>> reactions;

    SessionConnectingState(my_context ctx);
    ~SessionConnectingState();

    result react(const SessionTerminateEvent& ev);
    result react(const DialSucceedEvent& ev);
    result react(const DialDisconnectedEvent& ev);

    SessionMachine& machine;
};

struct VolumeCheckingThread
{
	VolumeCheckingThread() :call_id(-1), handle(""), stopped(true) {};
	VolumeCheckingThread(int cid, string hdl) : call_id(cid), handle(hdl), stopped(false) {};
	void operator()();
	int call_id;
	string handle;
	bool stopped;
};

struct SessionConfirmedState : state <SessionConfirmedState, SessionMachine> 
{
    typedef boost::mpl::list<
		custom_reaction<SessionTerminateEvent>, 
		custom_reaction<SessionMediaDisconnectEvent>,		// v1.22
		custom_reaction<AudioEvent>, 
		custom_reaction<PositionEvent>, 
		custom_reaction<DialDisconnectedEvent>> reactions;

    SessionConfirmedState(my_context ctx);
    ~SessionConfirmedState();

    result react(const SessionTerminateEvent& ev);
	result react(const SessionMediaDisconnectEvent& ev);
    result react(const AudioEvent& ev);
    result react(const PositionEvent& ev);
    result react(const DialDisconnectedEvent& ev);

    SessionMachine& machine;
	boost::thread thr;
	VolumeCheckingThread volumeCheckingThread;
};

// ------------------------------ Account ------------------------------
struct AccountLogoutState;
class AccountInfo;

struct AccountMachine : state_machine<AccountMachine, AccountLogoutState> 
{
	AccountInfo* info;
};

struct AccountLogoutState : state <AccountLogoutState, AccountMachine> 
{
	typedef boost::mpl::list<
		custom_reaction<AccountLoginEvent>> reactions;

    AccountLogoutState(my_context ctx);
    ~AccountLogoutState();

    result react(const AccountLoginEvent& ev);

    AccountMachine& machine;
};

struct AccountRegisteringState : state <AccountRegisteringState, AccountMachine> 
{
	typedef boost::mpl::list<
		custom_reaction<RegSucceedEvent>, 
		custom_reaction<RegFailedEvent>> reactions;

    AccountRegisteringState(my_context ctx);
    ~AccountRegisteringState();

    result react(const RegSucceedEvent& ev);
    result react(const RegFailedEvent& ev);

    AccountMachine& machine;
};

struct AccountLoginState : state <AccountLoginState, AccountMachine> 
{
    typedef boost::mpl::list<
		custom_reaction<AccountLogoutEvent>, 
		custom_reaction<RegFailedEvent>> reactions;

    AccountLoginState(my_context ctx);
    ~AccountLoginState();

    result react(const AccountLogoutEvent& ev);
    result react(const RegFailedEvent& ev);

    AccountMachine& machine;
};

struct AccountUnregisteringState : state <AccountUnregisteringState, AccountMachine> 
{
	typedef boost::mpl::list<
		custom_reaction<RegSucceedEvent>, 
		custom_reaction<RegFailedEvent>> reactions;

    AccountUnregisteringState(my_context ctx);
    ~AccountUnregisteringState();

    result react(const RegSucceedEvent& ev);
    result react(const RegFailedEvent& ev);

    AccountMachine& machine;
};

// ------------------------------ Connector ------------------------------
struct ConnectorIdleState;
class ConnectorInfo;

struct ConnectorMachine : state_machine <ConnectorMachine, ConnectorIdleState> 
{
	ConnectorInfo* info;
};

struct ConnectorIdleState : state <ConnectorIdleState, ConnectorMachine> 
{
    typedef custom_reaction<InitializeEvent> reactions;

    ConnectorIdleState(my_context ctx);
    ~ConnectorIdleState();

    result react(const InitializeEvent& ev);

    ConnectorMachine& machine;
};

struct ConnectorActiveState : state <ConnectorActiveState, ConnectorMachine> 
{
    typedef boost::mpl::list<
		custom_reaction<ShutdownEvent>, 
		custom_reaction<AudioEvent>> reactions;

    ConnectorActiveState(my_context ctx);
    ~ConnectorActiveState();

    result react(const ShutdownEvent& ev);
    result react(const AudioEvent& ev);

    ConnectorMachine& machine;
};

class BaseInfo {
	public:
		string handle;
		int id;
};

class AccountInfo : public BaseInfo {
	
	public:
		AccountInfo() {
			machine.initiate();
			machine.info = this;
		};
		~AccountInfo() {
			machine.terminate();
		};

		AccountMachine machine;
	    Account account;

		SIPConference *sipconf;
};

class SessionInfo : public BaseInfo {
	
	public:
		SessionInfo(const AccountInfo* acc) {
			machine.initiate();
			machine.info = this;
			account = acc;
		};
		~SessionInfo() {
			machine.terminate();
		};

		SessionMachine machine;

		Session session; // the current session state
	    Orientation speaker; // the position of the speaking voice
	    Orientation listener; // the position of the listener to the speaker

		string incoming_uri;

		const AccountInfo* account;
};

class BaseManager {
	
	public:
		string registHandle(BaseInfo*);
		BaseInfo* findBase(const string&);
		void registId(const int, const string&);
		string convertId(const int);
		void remove(const string&);
	protected:
		map<string, BaseInfo*> infos;
		map<int, string> handles;
};

class SessionManager : public BaseManager {
	
	public:
		SessionManager() { };
		~SessionManager() { };
		
		string create(const AccountInfo*);
		SessionInfo* find(const string&);
		void controlAudioLevel();
};

class AccountManager : public BaseManager {
	
	public:
		AccountManager() { };
		~AccountManager() { };
		
		string create();
		AccountInfo* find(const string&);
};

class ConnectorInfo : public BaseInfo {
	
	public:
		ConnectorInfo() {
			machine.initiate();
			machine.info = this;
		};
		~ConnectorInfo() {
			machine.terminate();
		};

	    Audio audio; // the current audio state

		AccountManager account;
		SessionManager session;

		ConnectorMachine machine;

		string voiceserver_url;
};

#endif //_STATE_HPP_
