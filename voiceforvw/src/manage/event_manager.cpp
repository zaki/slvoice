/* event_manager.cpp -- event management module
 *
 *			Copyright 2008, 3di.jp Inc
 */

#include <main.h>

void BlockingQueue::enqueue(void *data) {
	_lock lk(_mutex);
	_queue.push(data);
	_cond.notify_all();
}

void* BlockingQueue::dequeue() {
	void *ret = NULL;

	_lock lk(_mutex);

	while (_queue.size() == 0) {
		_cond.wait(lk);
	}

	ret = _queue.front();
	_queue.pop();

	return ret;
}

void EventManager::operator()() 
{
	pj_thread_init();
	if (!pj_thread_is_registered())
	{
		pj_thread_t *thread;
		pj_status_t status = pj_thread_register("", desc, &thread);

		if (status != PJ_SUCCESS)
		{
			g_logger->Error("EVENTMANAGER") << "Could not register audio device thread. ERROR: {" << status << "}" << endl;
		}
	}
	else
	{
		g_logger->Warn() << "Thread already registered" << endl;
	}

	pj_status_t status;
	pj_caching_pool cp;
	pj_caching_pool_init(&cp, &pj_pool_factory_default_policy, 0);
	pjmedia_aud_subsys_init(&cp.factory);

	unsigned int devcount = pjmedia_aud_dev_count();
	pjmedia_aud_dev_info info;

	g_logger->Info() << "Audio device count " << devcount << endl;
	
	for(unsigned int i = 0; i < devcount; i++)
	{
		status = pjmedia_aud_dev_get_info((pjmedia_aud_dev_index)i, &info);
		if (status != PJ_SUCCESS)
		{
			g_logger->Warn() << "Could not retreive audio device information. {ERROR:" << status << "}" << endl;
		}
		else
		{
			string devicename = string(info.name);
			g_logger->Info() << "Audio device " << i << " name=" << string(info.name) << endl;
			g_logger->Debug() << "Properties: CAPS=" << info.caps << " Samplerate=" << info.default_samples_per_sec << " Driver=" << string(info.driver) << endl;
			g_logger->Debug() << "Formats=" << info.ext_fmt_cnt << " Inputs=" << info.input_count << " Outputs=" << info.output_count << endl;
			g_logger->Debug() << "Routes=" << info.routes << endl;
			for(int j = 0; j < info.ext_fmt_cnt; j++)
			{
				g_logger->Debug() << "Format(" << j << ")=" << info.ext_fmt[j].id << endl;
			}
			g_logger->Debug() << endl;

			if (info.input_count > 0)
			{
				bool supported = false;
				for (int k = 0; k < info.ext_fmt_cnt; k++)
				{
					if (info.ext_fmt[k].id == PJMEDIA_FORMAT_ULAW || info.ext_fmt[k].id == PJMEDIA_FORMAT_ILBC)
						supported = true;
				}
				if (supported)
				{
					g_eventManager.CaptureDevices += "<CaptureDevice><Device>" + devicename + "</Device></CaptureDevice>";
					g_logger->Debug() << "Supported capture device: " << devicename << endl;
				}
			}
			if (info.output_count > 0)
			{
				bool supported = false;
				for (int l = 0; l < info.ext_fmt_cnt; l++)
				{
					if (info.ext_fmt[l].id == PJMEDIA_FORMAT_ULAW || info.ext_fmt[l].id == PJMEDIA_FORMAT_ILBC)
						supported = true;
				}
				if (supported)
				{
					g_eventManager.RenderDevices += "<RenderDevice><Device>" + devicename + "</Device></RenderDevice>";
					g_logger->Debug() << "Supported render device: " << devicename << endl;
				}
			}
		}
	}

	g_eventManager.CurrentCaptureDevice = "default";
	g_eventManager.CurrentRenderDevice = "default";

	pjmedia_aud_subsys_shutdown();

	g_logger->Debug() << "entering EventManager::operator()()" << endl;

	Event *item = NULL;

	while ((item = (Event *)blockQueue.dequeue()) != NULL)
	{
		eventProc(item);
	}

	g_logger->Debug() << "exiting EventManager::operator()()" << endl;
}

void EventManager::processConnector(ConnectorEvent *ev) 
{
	g_logger->Debug() << "entering processConnector()" << endl;

	ConnectorInfo* con = glb_server->getConnector();

	// Connector Events
    switch (ev->type)
    {
        case EventType_Initialize:
			g_logger->Debug() << "EventType_Initialize" << endl;
			con->machine.process_event(*(InitializeEvent*)ev);
            break;

        case EventType_Shutdown:
			g_logger->Debug() << "EventType_ShutdownEvent" << endl;
            con->machine.process_event(*(ShutdownEvent*)ev);
            break;

		case EventType_Audio:
			g_logger->Debug() << "EventType_AudioEvent" << endl;
            con->machine.process_event(*(AudioEvent*)ev);
            break;
		default:
			// logic error route
			g_logger->Warn() << "unknown event (logic error)" << endl;
			break;
	}

	g_logger->Debug() << "exiting processConnector()" << endl;
}

void EventManager::processAccount(AccountEvent *ev) 
{
	g_logger->Debug() << "entering processAccount()" << endl;

	ConnectorInfo* con = glb_server->getConnector();

	if (ev->type == EventType_AccountLogin) {
		// create new account
		ev->account_handle = con->account.create();
	}

	// finding the account info
	if (ev->account_handle == "") {
		ev->account_handle = con->account.convertId(ev->acc_id);
	}
	
	g_logger->Info() << " Account handle = " << ev->account_handle << endl;

	AccountInfo *info = con->account.find(ev->account_handle);

	if (info == NULL) {
		g_logger->Warn() << "Account info is not found" << endl;
		return;
	}

	// Account Events
    switch (ev->type)
    {
        case EventType_AccountLogin:
			g_logger->Debug() << "EventType_AccountLogin" << endl;
			info->machine.process_event(*(AccountLoginEvent*)ev);
            break;

		case EventType_AccountLogout:
			g_logger->Debug() << "EventType_AccountLogout" << endl;
			info->machine.process_event(*(AccountLogoutEvent*)ev);
            break;

        case EventType_RegSucceed:
			g_logger->Debug() << "EventType_RegSucceed" << endl;
			info->machine.process_event(*(RegSucceedEvent*)ev);
            break;

		case EventType_RegFailed:
			g_logger->Debug() << "EventType_RegFailed" << endl;
			info->machine.process_event(*(RegFailedEvent*)ev);
            break;

		case EventType_AccountRemove:
			g_logger->Debug() << "EventType_AccountRemove" << endl;
			con->account.remove(ev->account_handle);
            break;

		default:
			// logic error route
			g_logger->Warn() << "unknown event (logic error)" << endl;
			break;
	}

	g_logger->Debug() << "exiting processAccount()" << endl;
}

void EventManager::processSession(SessionEvent *ev) 
{
	g_logger->Debug() << "entering processSession()" << endl;

    ConnectorInfo* con = glb_server->getConnector();

	if (ev->type == EventType_SessionCreate 
	 || ev->type == EventType_DialIncoming) {

		if (ev->account_handle == "") {
			ev->account_handle = con->account.convertId(ev->acc_id);
		}

		g_logger->Info() << "AccountHandle = " << ev->account_handle << endl;

		AccountInfo *accinfo = con->account.find(ev->account_handle);

		if (accinfo != NULL) {

			// create new session
			ev->session_handle = con->session.create(accinfo);
			g_logger->Info() << "AccountHandle = " << ev->session_handle << endl;

			SessionInfo *sinfo = con->session.find(ev->session_handle);

			// set call-id
			sinfo->id = ev->call_id;
			con->session.registId(sinfo->id, sinfo->handle);

			// set incoming user's uri
			sinfo->incoming_uri = ev->uri;
		}
		else 
		{
			g_logger->Warn() << "This handle is not registered" << endl;
		}

		// create new session
		ev->account_handle = con->account.create();
	}

	if (ev->session_handle == "") {
		ev->session_handle = con->session.convertId(ev->call_id);
	}

	// finding the session info
	SessionInfo *info = con->session.find(ev->session_handle);

	if (info == NULL) 
	{
		g_logger->Warn() << "Session info is not found" << endl;
		return;
	}

    switch (ev->type)
    {
		// Session Events
		case EventType_SessionCreate:
			g_logger->Debug() << "EventType_SessionCreate" << endl;
			info->machine.process_event(*(SessionCreateEvent*)ev);
            break;

        case EventType_Position:
			g_logger->Debug() << "EventType_Position" << endl;
			info->machine.process_event(*(PositionEvent*)ev);
            break;

        case EventType_SessionTerminate:
			g_logger->Debug() << "EventType_SessionTerminate" << endl;
			info->machine.process_event(*(SessionTerminateEvent*)ev);
            break;

		// v1.22
        case EventType_SessionMediaDisconnect:
			g_logger->Debug() << "EventType_SessionMediaDisconnect" << endl;
			info->machine.process_event(*(SessionMediaDisconnectEvent*)ev);
            break;

        case EventType_SessionConnect:
			g_logger->Debug() << "EventType_SessionConnect" << endl;
			info->machine.process_event(*(SessionConnectEvent*)ev);
            break;

        case EventType_DialIncoming:
			g_logger->Debug() << "EventType_DialIncoming" << endl;
			info->machine.process_event(*(DialIncomingEvent*)ev);
            break;

        case EventType_DialEarly:
			g_logger->Debug() << "EventType_DialEarly" << endl;
			info->machine.process_event(*(DialEarlyEvent*)ev);
            break;

        case EventType_DialConnecting:
			g_logger->Debug() << "EventType_DialConnecting" << endl;
			info->machine.process_event(*(DialConnectingEvent*)ev);
            break;

        case EventType_DialSucceed:
			g_logger->Debug() << "EventType_DialSucceed" << endl;
			info->machine.process_event(*(DialSucceedEvent*)ev);
            break;

        case EventType_DialDisconnected:
			g_logger->Debug() << "EventType_DialDisconnected" << endl;
			info->machine.process_event(*(DialDisconnectedEvent*)ev);
            break;

        case EventType_SessionRemove:
			g_logger->Debug() << "EventType_SessionRemove" << endl;
			con->session.remove(ev->session_handle);
            break;

		default:
			// logic error route
			g_logger->Warn() << "unknown event (logic error)" << endl;
			break;
	}

	g_logger->Debug() << "exiting processSession()" << endl;
}

void EventManager::eventProc(Event *ev) 
{
	g_logger->Debug() << "entering eventProc()" << endl;

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
		case EventType_SessionMediaDisconnect:		// v1.22
            processSession((SessionEvent*)ev);
			break;

        default:
			g_logger->Warn() << "unknown event " << ev->type << endl;
            break;
    }

	if (ev->result != NULL) {

		ev->result->ReturnCode = "0";

		string respStr = ev->result->ToString();

		g_logger->Debug() << "Deleting response message [" << ev->result << "]" << endl;
		delete ev->result;
		ev->result = NULL;

		try {
			glb_server->Send(respStr);
			g_logger->Debug() << "Sent a response message" << endl;
		}
        catch (SocketRunTimeException& e) 
        { 
            // ignore
        }
	}

	if (ev->message != NULL) 
	{
		g_logger->Debug() << "Deleting request message [" << ev->message << "]" << endl;
		delete ev->message;
		ev->message = NULL;
	}

	if (ev != NULL) 
	{
		g_logger->Debug() << "Deleting event [" << ev << "]" << endl;
		delete ev;
	}

	g_logger->Debug() << "exiting eventProc()" << endl;
}
