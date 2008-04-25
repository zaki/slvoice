/* server.cpp -- server module
 *
 *			Ryan McDougall -- 2008
 */

#include <main.h>
#include <server.hpp>

//=============================================================================
Server::Server (int port) : 
    sock_ (NULL),
    port_ (port), 
    bufsize_ (4096), 
    buf_ (NULL)
{
    try
    {
        buf_.reset (new char [bufsize_]);

        socketsInit (); // for winsock compat
        server_.listen (port_);
        sock_.reset (new TCPSocketWrapper (server_.accept ()));

        state_.initiate ();
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
    RequestQueue::iterator i = queue_.begin();
    while (i != queue_.end())
        delete *i++;

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
    if (!(sock_.get()))
        throw SocketLogicException ("server has no connection");

    char *buf (buf_.get());

    state_.process_event (StartEvent());

    size_t nread (0);
    for (;;)
    {
        memset (buf, 0, bufsize_);
        
        try { nread = sock_->read (buf, bufsize_); }
        catch (SocketRunTimeException& e) 
        { 
            // TODO: viewer can occasionally quit uncleanly
            // can we do better than ignoring the exception and quitting?
            return; 
        }

        if (nread <= 0) 
            return;

        cout << "received: " << buf << endl;
        enqueue_request_ (buf);
        process_request_queue_();
    }
}

//=============================================================================
void Server::Send (const string& m) 
{ 
    if (!(sock_.get()))
        throw SocketLogicException ("server has no connection");

    cout << m << endl;
    sock_->write (m.c_str(), m.size()); 
}

//=============================================================================
void Server::Conference (const string& filename)
{
    SIPServerInfo sinfo;
    SIPUserInfo uinfo;

    ifstream file (filename.c_str());
    if (!file) throw runtime_error ("unable to open sip.conf file");

    file >> sinfo;
    file >> uinfo;
    file.close();

    activeconference_.reset (new SIPConference (sinfo));
    activeconference_-> Register (uinfo);
    activeconference_-> Join ();
}

//=============================================================================
void Server::enqueue_request_ (char* mesg)
{
    TiXmlDocument doc;
    doc.Parse (mesg);

    auto_ptr <const Request> req (parse_request (doc));

    if (req.get()) // is null on parse failure
        queue_.push_back (req.release());
}

//=============================================================================
void Server::pop_messages_on_event_ (ViewerEvent& ev)
{
    ev.messages.splice 
        (ev.messages.end(), 
         queue_, queue_.begin(), queue_.end());
}

//=============================================================================
void Server::process_request_queue_ ()
{
    if (queue_.size() == 0)
        return;

    switch (queue_.back()->type)
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

        case SessionSet3DPosition1:
            {
                PositionEvent ev;
                pop_messages_on_event_ (ev);
                state_.process_event (ev);
            }
            break;

        case AccountLogout1:
        case ConnectorInitiateShutdown1:
        case SessionTerminate1:
            {
                StopEvent ev;
                pop_messages_on_event_ (ev);
                state_.process_event (ev);
            }
            break;

        case ConnectorMuteLocalMic1:
        case ConnectorMuteLocalSpeaker1:
        case ConnectorSetLocalMicVolume1:
        case ConnectorSetLocalSpeakerVolume1:
            {
                HardwareSetEvent ev;
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
