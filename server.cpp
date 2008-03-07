/* server.cpp -- server module
 *
 *			Ryan McDougall -- 2008
 */

#include <main.h>
#include <server.hpp>

//=============================================================================
Server::Server (int port) : 
    state_ (this), 
    port_ (port), 
    bufsize_ (4096), 
    buf_ (new char [bufsize_])
{
    server_.listen (port_);
    sock_.reset (new TCPSocketWrapper (server_.accept ()));
    state_.initiate ();
}

//=============================================================================
Server::~Server () 
{ 
    server_.close();
    sock_->close();

    RequestQueue::iterator i = queue_.begin();
    while (i != queue_.end())
        delete *i++;
}

//=============================================================================
void Server::Start ()
{
    char *buf (buf_.get());

    state_.process_event (StartEvent());

    size_t nread (0);
    for (;;)
    {
        ActionType type;
        TiXmlDocument doc;

        memset (buf, 0, bufsize_);
        nread = sock_->read (buf, bufsize_);
        if (nread <= 0) break;
        cout << "received: " << buf << endl;

        doc.Parse (buf);
        type = get_action_type (doc);

        queue_.push_back (new RequestItem (parse_request (doc), type)); 

        issue_events_ (type);
    }
}

//=============================================================================
void Server::Send (const string& m) 
{ 
    sock_->write (m.c_str(), m.size()); 
}

//=============================================================================
void Server::pop_messages_on_event_ (ViewerEvent& ev)
{
    ev.messages.splice 
        (ev.messages.end(), 
         queue_, queue_.begin(), queue_.end());
}

//=============================================================================
void Server::issue_events_ (ActionType type)
{
    switch (type)
    {
        case AccountLogin1:
            {
                AccountEvent ev;
                pop_messages_on_event_ (ev);
                state_.process_event (ev);
            }
            break;

        case ConnectorCreate1:
            {
                ConnectionEvent ev;
                pop_messages_on_event_ (ev);
                state_.process_event (ev);
            }
            break;

        case SessionCreate1:
            {
                SessionEvent ev;
                pop_messages_on_event_ (ev);
                state_.process_event (ev);
            }
            break;

        case SessionTerminate1:
            {
                StopEvent ev;
                pop_messages_on_event_ (ev);
                state_.process_event (ev);
            }
            break;

        default:
            {
                //MessageReceivedEvent ev;
                //state_.process_event (ev);
            }
            break;
    }
}

