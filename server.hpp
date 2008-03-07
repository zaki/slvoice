/* server.hpp -- server definition
 *
 *			Ryan McDougall -- 2008
 */

#ifndef _SERVER_HPP_
#define _SERVER_HPP_

const int glb_default_port (44124);

//=============================================================================
// Server class

class Server
{
    public:
        Server (int port = glb_default_port) 
            : state_ (this) port_ (port), 
            bufsize_ (4096), buf_ (new char [bufsize_]); 

        ~Server ();
        
        void Start ();
        void Send (const string& m);

    private:
        void pop_messages_on_event_ (ViewerEvent& ev);
        void issue_events_ (ActionType type);

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
        Server ();
        Server (const Server&);
        void operator= (const Server&);
};

#endif //_SERVER_HPP_
