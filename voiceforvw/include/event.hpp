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
	Event() : message(NULL), result(NULL), type(EventType_None) {};
	virtual ~Event() {}

	Request *message;
	ResponseBase *result;
	EventType type;
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
	};
};

struct ShutdownEvent : public ConnectorEvent, event<ShutdownEvent> {
	ShutdownEvent() {
		type = EventType_Shutdown;
	};
};

struct AudioEvent : public ConnectorEvent, event <AudioEvent> {
	AudioEvent() {
		type = EventType_Audio;
	};
};

struct AccountLoginEvent : public AccountEvent, event<AccountLoginEvent> {
	AccountLoginEvent() {
		type = EventType_AccountLogin;
	};
};

struct AccountLogoutEvent : public AccountEvent, event<AccountLogoutEvent> {
	AccountLogoutEvent() {
		type = EventType_AccountLogout;
	};
};

struct RegSucceedEvent : public AccountEvent, event<RegSucceedEvent> {
	RegSucceedEvent() {
		type = EventType_RegSucceed;
	};
};

struct RegFailedEvent : public AccountEvent, event<RegFailedEvent> {
	RegFailedEvent() {
		type = EventType_RegFailed;
	};
};

struct AccountRemoveEvent : public SessionEvent, event <AccountRemoveEvent> {
	AccountRemoveEvent() {
		type = EventType_AccountRemove;
	};
};

struct SessionCreateEvent : public SessionEvent, event<SessionCreateEvent> {
	SessionCreateEvent() {
		type = EventType_SessionCreate;
	};
};

struct SessionTerminateEvent : public SessionEvent, event<SessionTerminateEvent> {
	SessionTerminateEvent() {
		type = EventType_SessionTerminate;
	};
};

struct SessionMediaDisconnectEvent : public SessionEvent, event<SessionMediaDisconnectEvent> {
	SessionMediaDisconnectEvent() {
		type = EventType_SessionMediaDisconnect;
	};
};

struct SessionConnectEvent : public SessionEvent, event<SessionConnectEvent> {
	SessionConnectEvent() {
		type = EventType_SessionConnect;
	};
};

struct DialIncomingEvent : public SessionEvent, event<DialIncomingEvent> {
	DialIncomingEvent() {
		type = EventType_DialIncoming;
	};
};

struct DialEarlyEvent : public SessionEvent, event<DialEarlyEvent> {
	DialEarlyEvent() {
		type = EventType_DialEarly;
	};
};

struct DialConnectingEvent : public SessionEvent, event<DialConnectingEvent> {
	DialConnectingEvent() {
		type = EventType_DialConnecting;
	};
};

struct DialSucceedEvent : public SessionEvent, event<DialSucceedEvent> {
	DialSucceedEvent() {
		type = EventType_DialSucceed;
	};
};

struct DialDisconnectedEvent : public SessionEvent, event<DialDisconnectedEvent> {
	DialDisconnectedEvent() {
		type = EventType_DialDisconnected;
	};
};

struct PositionEvent : public SessionEvent, event <PositionEvent> {
	PositionEvent() {
		type = EventType_Position;
	};
};

struct SessionRemoveEvent : public SessionEvent, event <SessionRemoveEvent> {
	SessionRemoveEvent() {
		type = EventType_SessionRemove;
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
