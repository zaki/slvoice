/* event_manager.cpp -- event management module
 *
 *			Copyright 2008, 3di.jp Inc
 */

#include <main.h>

void BlockingQueue::enqueue(void *data) {
//	VFVW_LOG("entering enqueue() [%x]", data);
	_lock lk(_mutex);
	_queue.push(data);
	_cond.notify_all();
//	VFVW_LOG("exiting enqueue()");
}

void* BlockingQueue::dequeue() {

	void *ret = NULL;
//	VFVW_LOG("entering dequeue()");

	_lock lk(_mutex);

	while (_queue.size() == 0) {
//		VFVW_LOG("waiting for enqueued");
		_cond.wait(lk);
	}

	ret = _queue.front();
	_queue.pop();

//	VFVW_LOG("exiting dequeue() [%x]", ret);

	return ret;
}

void EventManager::operator()() {

	VFVW_LOG("entering EventManager::operator()()");

	Event *item = NULL;

	while ((item = (Event *)blockQueue.dequeue()) != NULL)
	{
		eventProc(item);
	}

	VFVW_LOG("exiting EventManager::operator()()");
}

void EventManager::processConnector(ConnectorEvent *ev) {

	VFVW_LOG("entering processConnector()");

	ConnectorInfo* con = glb_server->getConnector();

	// Connector Events
    switch (ev->type)
    {
        case EventType_Initialize:
			VFVW_LOG("EventType_Initialize");
			con->machine.process_event(*(InitializeEvent*)ev);
            break;

        case EventType_Shutdown:
			VFVW_LOG("EventType_Shutdown");
            con->machine.process_event(*(ShutdownEvent*)ev);
            break;

		case EventType_Audio:
			VFVW_LOG("EventType_Audio");
            con->machine.process_event(*(AudioEvent*)ev);
            break;
		default:
			// logic error route
			VFVW_LOG("unknown event (logic error)");
			break;
	}

	VFVW_LOG("exiting processConnector()");
}

void EventManager::processAccount(AccountEvent *ev) {

	VFVW_LOG("entering processAccount()");

	ConnectorInfo* con = glb_server->getConnector();

	if (ev->type == EventType_AccountLogin) {
		// create new account
		ev->account_handle = con->account.create();
	}

	// finding the account info
	if (ev->account_handle == "") {
		ev->account_handle = con->account.convertId(ev->acc_id);
	}
	VFVW_LOG("account handle = %s", ev->account_handle.c_str());

	AccountInfo *info = con->account.find(ev->account_handle);

	if (info == NULL) {
        VFVW_LOG("Account info is not found.");
		return;
	}

	// Account Events
    switch (ev->type)
    {
        case EventType_AccountLogin:
			VFVW_LOG("EventType_AccountLogin");
			info->machine.process_event(*(AccountLoginEvent*)ev);
            break;

		case EventType_AccountLogout:
			VFVW_LOG("EventType_AccountLogout");
			info->machine.process_event(*(AccountLogoutEvent*)ev);
            break;

        case EventType_RegSucceed:
			VFVW_LOG("EventType_RegSucceed");
			info->machine.process_event(*(RegSucceedEvent*)ev);
            break;

		case EventType_RegFailed:
			VFVW_LOG("EventType_RegFailed");
			info->machine.process_event(*(RegFailedEvent*)ev);
            break;

		case EventType_AccountRemove:
			VFVW_LOG("EventType_AccountRemove");
			con->account.remove(ev->account_handle);
            break;

		default:
			// logic error route
			VFVW_LOG("unknown event (logic error)");
			break;
	}

	VFVW_LOG("exiting processAccount()");
}

void EventManager::processSession(SessionEvent *ev) {

	VFVW_LOG("entering processSession()");

    ConnectorInfo* con = glb_server->getConnector();

	if (ev->type == EventType_SessionCreate 
	 || ev->type == EventType_DialIncoming) {

		if (ev->account_handle == "") {
			ev->account_handle = con->account.convertId(ev->acc_id);
		}
		VFVW_LOG("AccountHandle = %s", ev->account_handle.c_str());

		AccountInfo *accinfo = con->account.find(ev->account_handle);

		if (accinfo != NULL) {

			// create new session
			ev->session_handle = con->session.create(accinfo);
			VFVW_LOG("SessionHandle = %s", ev->session_handle.c_str());

			SessionInfo *sinfo = con->session.find(ev->session_handle);

			// set call-id
			sinfo->id = ev->call_id;
			con->session.registId(sinfo->id, sinfo->handle);

			// set incoming user's uri
			sinfo->incoming_uri = ev->uri;
		}
		else {
			VFVW_LOG("this handle is not registered");
		}

		// create new session
		ev->account_handle = con->account.create();
	}

	if (ev->session_handle == "") {
		ev->session_handle = con->session.convertId(ev->call_id);
	}

	// finding the session info
	SessionInfo *info = con->session.find(ev->session_handle);

	if (info == NULL) {
        VFVW_LOG("Session info is not found.");
		return;
	}

    switch (ev->type)
    {
		// Session Events
		case EventType_SessionCreate:
			VFVW_LOG("EventType_SessionCreate");
			info->machine.process_event(*(SessionCreateEvent*)ev);
            break;

        case EventType_Position:
			VFVW_LOG("EventType_Position");
			info->machine.process_event(*(PositionEvent*)ev);
            break;

        case EventType_SessionTerminate:
			VFVW_LOG("EventType_SessionTerminate");
			info->machine.process_event(*(SessionTerminateEvent*)ev);
            break;

        case EventType_SessionConnect:
			VFVW_LOG("EventType_SessionConnect");
			info->machine.process_event(*(SessionConnectEvent*)ev);
            break;

        case EventType_DialIncoming:
			VFVW_LOG("EventType_DialIncoming");
			info->machine.process_event(*(DialIncomingEvent*)ev);
            break;

        case EventType_DialEarly:
			VFVW_LOG("EventType_DialEarly");
			info->machine.process_event(*(DialEarlyEvent*)ev);
            break;

        case EventType_DialConnecting:
			VFVW_LOG("EventType_DialConnecting");
			info->machine.process_event(*(DialConnectingEvent*)ev);
            break;

        case EventType_DialSucceed:
			VFVW_LOG("EventType_DialSucceed");
			info->machine.process_event(*(DialSucceedEvent*)ev);
            break;

        case EventType_DialDisconnected:
			VFVW_LOG("EventType_DialDisconnected");
			info->machine.process_event(*(DialDisconnectedEvent*)ev);
            break;

        case EventType_SessionRemove:
			VFVW_LOG("EventType_SessionRemove");
			con->session.remove(ev->session_handle);
            break;

		default:
			// logic error route
			VFVW_LOG("unknown event (logic error)");
			break;
	}

	VFVW_LOG("exiting processSession()");
}

void EventManager::eventProc(Event *ev) {

	VFVW_LOG("entering eventProc()");

    switch (ev->type)
    {
		// Connector Events
        case EventType_Initialize:
        case EventType_Shutdown:
		case EventType_Audio:
            processConnector((ConnectorEvent*)ev);
            break;

		// Account Events
        case EventType_AccountLogin:
		case EventType_AccountLogout:
        case EventType_RegSucceed:
        case EventType_RegFailed:
		case EventType_AccountRemove:
            processAccount((AccountEvent*)ev);
            break;

		// Session Events
		case EventType_SessionCreate:
        case EventType_Position:
        case EventType_SessionTerminate:
        case EventType_SessionConnect:
        case EventType_DialIncoming:
        case EventType_DialEarly:
        case EventType_DialConnecting:
        case EventType_DialSucceed:
        case EventType_DialDisconnected:
		case EventType_SessionRemove:
            processSession((SessionEvent*)ev);
			break;

        default:
			VFVW_LOG("unknown event");
            break;
    }

	if (ev->result != NULL) {

		ev->result->ReturnCode = "0";

		string respStr = ev->result->ToString();

		VFVW_LOG("deleting response message [%x]", ev->result);
		delete ev->result;
		ev->result = NULL;

		try {
			glb_server->Send(respStr);
			VFVW_LOG("sent a response message");
		}
        catch (SocketRunTimeException& e) 
        { 
            // ignore
        }
	}

	if (ev->message != NULL) {
		VFVW_LOG("deleting request message [%x]", ev->message);
		delete ev->message;
		ev->message = NULL;
	}

	if (ev != NULL) {
		VFVW_LOG("deleting event [%x]", ev);
		delete ev;
	}

	VFVW_LOG("exiting eventProc()");
}
