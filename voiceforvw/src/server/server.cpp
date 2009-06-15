/* server.cpp -- server module
 *
 *			Copyright 2008, 3di.jp Inc
 */

#include <curl/curl.h>

#include "main.h"
#include "server.hpp"

//=============================================================================
Server::Server (int port) : 
    sock_ (NULL),
    port_ (port), 
    bufsize_ (4096), 
    buf_ (NULL),
	userURI("")
{
	g_logger->Debug() << "entering Server()" << endl;

    try
    {
        buf_.reset (new char [bufsize_]);

        socketsInit (); // for winsock compat
        server_.listen (port_);
        sock_.reset (new TCPSocketWrapper (server_.accept ()));
    }
    catch (exception &e)
    {
        cerr << "unable to create server: " << e.what() << endl;
        server_.close();
        throw;
    }
}

//=============================================================================
Server::~Server () 
{ 
	g_logger->Debug() << "entering ~Server()" << endl;

    try 
    {
        if (sock_.get()) sock_->close();
        server_.close();
    }
    catch (...) {}

    socketsEnd (); // for winsock compat
}

//=============================================================================
void Server::Start ()
{
	char *pos;
	char *cur;

	if (!(sock_.get()))
        throw SocketLogicException ("server has no connection");

    char *buf (buf_.get());

    size_t nread (0);

    memset (buf, 0, bufsize_);

    for (;;)
    {
        try { nread = sock_->read (buf, bufsize_); }
        catch (SocketRunTimeException& e) 
        { 
            // TODO: viewer can occasionally quit uncleanly
            // can we do better than ignoring the exception and quitting?
            return; 
        }

        if (nread <= 0) 
            return;

		*(buf + nread) = 0x00;

		pos = NULL;
		cur = buf;

		while (NULL != (pos = strstr(cur, VFVW_XMLMSG_DELIM))) {

			*pos = 0x00;

			g_logger->Debug() << "received " << cur << endl;

			process_request_queue_(cur);

			nread -= strlen(cur);

			cur = pos + VFVW_XMLMSG_DELIM_LEN;
		}
    }
}

//=============================================================================
void Server::Send (const string& m) 
{ 
    if (!(sock_.get()))
        throw SocketLogicException ("server has no connection");

	g_logger->Debug() << m << endl;

	try
	{
		sock_->write (m.c_str(), m.size()); 
	}
	catch(exception e)
	{
		g_logger->Error() << "Error in Server::Send " << e.what() << endl;
	}
}

//=============================================================================
void Server::process_request_queue_(const char* mesg)
{
	Event *ev = NULL;

	Request *request = NULL;
	ResponseBase *response = NULL;

	// parsing a request message
    RequestParser parser(mesg);
    auto_ptr<const Request> req(parser.Parse());
	request = (Request *)req.release();

	const string result_code = "1";
	response = request->CreateResponse(result_code);
	if (response)
        response->InputXml = string(mesg);

    switch (request->Type)
    {
		// Connector Events
        case ConnectorCreate1:
			ev = new InitializeEvent();
			break;

        case ConnectorInitiateShutdown1:
			ev = new ShutdownEvent();
            break;

		case ConnectorMuteLocalMic1:
        case ConnectorMuteLocalSpeaker1:
        case ConnectorSetLocalMicVolume1:
        case ConnectorSetLocalSpeakerVolume1:
        case AuxGetCaptureDevices1:
        case AuxGetRenderDevices1:
        case AuxCaptureAudioStart1:
        case AuxCaptureAudioStop1:
        case AuxSetCaptureDevice1:
        case AuxSetRenderDevice1:
			ev = new AudioEvent();
            break;

		// Account Events
        case AccountLogin1:
			ev = new AccountLoginEvent();
            break;

		case AccountLogout1:
			ev = new AccountLogoutEvent();
			((AccountEvent*)ev)->account_handle 
				= ((AccountLogoutRequest *)request)->AccountHandle;
            break;

		// Session Events
        case SessionCreate1:
			ev = new SessionCreateEvent();
			((SessionEvent*)ev)->account_handle 
				= ((SessionCreateRequest *)request)->AccountHandle;
            break;

        case SessionSet3DPosition1:
			ev = new PositionEvent();
			((SessionEvent*)ev)->session_handle 
				= ((SessionSet3DPositionRequest *)request)->SessionHandle;
            break;

		case SessionTerminate1:
			ev = new SessionTerminateEvent();
			((SessionEvent*)ev)->session_handle 
				= ((SessionTerminateRequest *)request)->SessionHandle;
            break;

        case SessionConnect1:
			ev = new SessionConnectEvent();
			((SessionEvent*)ev)->session_handle 
				= ((SessionConnectRequest *)request)->SessionHandle;
            break;

		// v1.22
		case SessionMediaDisconnect1:
			ev = new SessionMediaDisconnectEvent();
			((SessionEvent*)ev)->session_handle = ((SessionMediaDisconnectRequest *)request)->SessionHandle;
			break;

        default:
			g_logger->Warn() << "Unknown request " << request->Action << endl;
            break;
    }

	if (ev != NULL) {
		ev->message = request;
		ev->result = response;
		g_eventManager.blockQueue.enqueue(ev);
	}
}

