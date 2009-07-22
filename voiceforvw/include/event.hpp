/* event.hpp -- event definition
 *
 *			Copyright 2008, 3di.jp Inc
 */

#ifndef _EVENT_HPP_
#define _EVENT_HPP_

#include <main.h>

//#include <boost/mpl/list.hpp>
#include <boost/statechart/event.hpp>
//#include <boost/statechart/custom_reaction.hpp>
//#include <boost/statechart/state_machine.hpp>
//#include <boost/statechart/state.hpp>

#include <queue>
#include <boost/ref.hpp>
#include <boost/thread.hpp>
#include <boost/thread/condition.hpp>

using namespace boost::statechart;

//=============================================================================
// Event Types
enum EventType
{
    EventType_None,
    EventType_Initialize,
    EventType_Shutdown,
    EventType_Audio,

    EventType_AccountLogin,
    EventType_AccountLogout,
    EventType_RegSucceed,
    EventType_RegFailed,
	EventType_AccountRemove,

    EventType_SessionCreate,
    EventType_SessionTerminate,
    EventType_SessionConnect,
	EventType_SessionMediaDisconnect,		// v1.22
    EventType_DialIncoming,
    EventType_DialEarly,
    EventType_DialConnecting,
    EventType_DialSucceed,
    EventType_DialDisconnected,
    EventType_Position,
	EventType_SessionRemove,
};

struct Event
{
	Event() : message(NULL), result(NULL), type(EventType_None), event_name("Unknown") {};
	virtual ~Event() {}

	Request *message;
	ResponseBase *result;
	EventType type;
	string event_name;
};

struct ConnectorEvent : public Event
{
};

struct AccountEvent : public ConnectorEvent
{
	int acc_id;
	string account_handle;
};

struct SessionEvent : public AccountEvent
{
	string uri;
	int call_id;

	string session_handle;
};

//----------------------------------------------------------------------
struct InitializeEvent : public ConnectorEvent, event<InitializeEvent> {
	InitializeEvent() {
		type = EventType_Initialize;
		event_name = string("InitializeEvent");
	};
};

struct ShutdownEvent : public ConnectorEvent, event<ShutdownEvent> {
	ShutdownEvent() {
		type = EventType_Shutdown;
		event_name = string("ConnectorEvent");
	};
};

struct AudioEvent : public ConnectorEvent, event <AudioEvent> {
	AudioEvent() {
		type = EventType_Audio;
		event_name = string("AudioEvent");
	};
};

struct AccountLoginEvent : public AccountEvent, event<AccountLoginEvent> 
{
	AccountLoginEvent() 
	{
		type = EventType_AccountLogin;
		event_name = string("AccountLoginEvent");
		accountURI = string("");
	};

	string accountURI;
};

struct AccountLogoutEvent : public AccountEvent, event<AccountLogoutEvent> {
	AccountLogoutEvent() {
		type = EventType_AccountLogout;
		event_name = string("AccountLogoutEvent");
	};
};

struct RegSucceedEvent : public AccountEvent, event<RegSucceedEvent> {
	RegSucceedEvent() {
		type = EventType_RegSucceed;
		event_name = string("RegSucceededEvent");
	};
};

struct RegFailedEvent : public AccountEvent, event<RegFailedEvent> {
	RegFailedEvent() {
		type = EventType_RegFailed;
		event_name = string("RegFailedEvent");
	};
};

struct AccountRemoveEvent : public SessionEvent, event <AccountRemoveEvent> {
	AccountRemoveEvent() {
		type = EventType_AccountRemove;
		event_name = string("AccountRemoveEvent");
	};
};

struct SessionCreateEvent : public SessionEvent, event<SessionCreateEvent> {
	SessionCreateEvent() {
		type = EventType_SessionCreate;
		event_name = string("SessionCreateEvent");
	};
};

struct SessionTerminateEvent : public SessionEvent, event<SessionTerminateEvent> {
	SessionTerminateEvent() {
		type = EventType_SessionTerminate;
		event_name = string("SessionTerminateEvent");
	};
};

struct SessionMediaDisconnectEvent : public SessionEvent, event<SessionMediaDisconnectEvent> {
	SessionMediaDisconnectEvent() {
		type = EventType_SessionMediaDisconnect;
		event_name = string("SessionMediaDisconnectEvent");
	};
};

struct SessionConnectEvent : public SessionEvent, event<SessionConnectEvent> {
	SessionConnectEvent() {
		type = EventType_SessionConnect;
		event_name = string("SessionConnectEvent");
	};
};

struct DialIncomingEvent : public SessionEvent, event<DialIncomingEvent> {
	DialIncomingEvent() {
		type = EventType_DialIncoming;
		event_name = string("DialIncomingEvent");
	};
};

struct DialEarlyEvent : public SessionEvent, event<DialEarlyEvent> {
	DialEarlyEvent() {
		type = EventType_DialEarly;
		event_name = string("DialEarlyEvent");
	};
};

struct DialConnectingEvent : public SessionEvent, event<DialConnectingEvent> {
	DialConnectingEvent() {
		type = EventType_DialConnecting;
		event_name = string("DialConnectingEvent");
	};
};

struct DialSucceedEvent : public SessionEvent, event<DialSucceedEvent> {
	DialSucceedEvent() {
		type = EventType_DialSucceed;
		event_name = string("DialSucceedEvent");
	};
};

struct DialDisconnectedEvent : public SessionEvent, event<DialDisconnectedEvent> {
	DialDisconnectedEvent() {
		type = EventType_DialDisconnected;
		event_name = string("DialDisconnectedEvent");
	};
};

struct PositionEvent : public SessionEvent, event <PositionEvent> {
	PositionEvent() {
		type = EventType_Position;
		event_name = string("PositionEvent");
	};
};

struct SessionRemoveEvent : public SessionEvent, event <SessionRemoveEvent> {
	SessionRemoveEvent() {
		type = EventType_SessionRemove;
		event_name = string("SessionRemoveEvent");
	};
};

//=============================================================================
// BlockingQueue class
class BlockingQueue
{
    public:
		BlockingQueue() { };
		~BlockingQueue() { };
        
		void enqueue(void *data);
		void* dequeue();

    private:
		typedef boost::mutex::scoped_lock _lock;
		boost::condition _cond;
		boost::mutex	_mutex;
		queue<void*>	_queue;
};

class EventManager
{
	public:
		EventManager() {};

		BlockingQueue blockQueue;

		void operator()();
		pj_thread_desc desc;

		string CaptureDevices;
		string RenderDevices;
		string CurrentCaptureDevice;
		string CurrentRenderDevice;

	private:
		void eventProc(Event*);
		void processConnector(ConnectorEvent*);
		void processAccount(AccountEvent*);
		void processSession(SessionEvent*);
};


#endif //_EVENT_HPP_
