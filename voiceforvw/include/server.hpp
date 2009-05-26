/* server.hpp -- server definition
 *
 *			Copyright 2008, 3di.jp Inc
 */

#ifndef _SERVER_HPP_
#define _SERVER_HPP_

#include <sockets/Sockets.h>

#include "state.hpp"

const int glb_default_port (44125);

#define	VFVW_XMLMSG_DELIM		"\n\n\n"
#define	VFVW_XMLMSG_DELIM_LEN	strlen("\n\n\n")

class ServerInfo {
	public:
		string sipsrvip;
};

//=============================================================================
// Server class

class Server
{
    public:
        Server (int port = glb_default_port);
        ~Server ();
        
        void Start ();
        void Send (const string&);

		ConnectorInfo* getConnector() { return &connector; }

		string userURI;

    private:
        //void enqueue_request_ (char* mesg);
        void process_request_queue_(const char* mesg);
        //void flush_messages_on_event_ (Event& ev);

    private:
        const int port_;
        const size_t bufsize_;
        auto_ptr <char> buf_;

        TCPSocketWrapper server_;
        auto_ptr <TCPSocketWrapper> sock_;

		//Request *request;
		//ResponseBase *response;

	private:
        ConnectorInfo connector;

	private:
        Server (const Server&);
        void operator= (const Server&);
};

#endif //_SERVER_HPP_
