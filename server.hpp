/* server.hpp -- server definition
 *
 *			Ryan McDougall -- 2008
 */

#ifndef _SERVER_HPP_
#define _SERVER_HPP_

#include <sockets/Sockets.h>
using namespace std;

const int glb_default_port (44124);

//=============================================================================
// Server class

class Server
{
    public:
        Server (int port = glb_default_port);
        ~Server ();
        
        void Start ();
        void Send (const string& m);

    private:
        void enqueue_request_ (char* mesg);
        void pop_messages_on_event_ (ViewerEvent& ev);
        void process_request_queue_ ();

    private:
        RequestQueue queue_;
        StateMachine state_;

        TCPSocketWrapper server_;
        auto_ptr <TCPSocketWrapper> sock_;

    private:
        const int port_;
        const size_t bufsize_;
        auto_ptr <char> buf_;

    private:
        Server (const Server&);
        void operator= (const Server&);
};

#endif //_SERVER_HPP_
