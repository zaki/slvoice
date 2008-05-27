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
	char *pos;
	char *cur;

	if (!(sock_.get()))
        throw SocketLogicException ("server has no connection");

    char *buf (buf_.get());

    state_.process_event (StartEvent());

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

//        cout << "received: " << buf << endl;
		pos = NULL;
		cur = buf;

		while (NULL != (pos = strstr(cur, VFVW_XMLMSG_DELIM))) {

			*pos = 0x00;

			VFVW_LOG("received: %s", cur);

			enqueue_request_ (cur);

			// TODO : multi threads
			process_request_queue_();

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

//    cout << m << endl;
	VFVW_LOG(m.c_str());
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
void Server::Conference (const Account& account, const Session& session)
{
    SIPServerInfo sinfo;
    SIPUserInfo uinfo;
    stringstream ss;

    ss.str (session.uri);
    ss >> sinfo;

	uinfo.name = account.name;
	uinfo.password = account.password;
	uinfo.domain = sinfo.domain;

	activeconference_.reset (new SIPConference (sinfo));
    activeconference_-> Register (uinfo);
    activeconference_-> Join ();
}

//=============================================================================
void Server::Disconnect()
{
    activeconference_->Leave();
}

//=============================================================================
void Server::AudioControl(const Session& session, const Audio& audio)
{
	float mic_volume = 0.0f;
	float spk_volume = 0.0f;

	// adjust mic volume
	if (!audio.mic_mute) {
		mic_volume = audio.mic_volume;
		// adjust between SL and PJSIP
		mic_volume = (mic_volume - VFVW_SL_VOLUME_MIN) 
						* VFVW_PJ_VOLUME_RANGE / VFVW_SL_VOLUME_RANGE;
	}
	activeconference_->AdjustTranVolume(session.call_id, mic_volume);

	// adjust speaker volume
	if (!audio.speaker_mute) {
		spk_volume = audio.speaker_volume;
		// adjust between SL and PJSIP
		spk_volume = (spk_volume - VFVW_SL_VOLUME_MIN) 
						* VFVW_PJ_VOLUME_RANGE / VFVW_SL_VOLUME_RANGE;
	}
	activeconference_->AdjustRecvVolume(session.call_id, spk_volume);
}


//=============================================================================
void Server::enqueue_request_ (char* mesg)
{
    RequestParser parser (mesg);
    auto_ptr <const Request> req (parser.Parse());

    queue_.push_back (req.release());
}

//=============================================================================
void Server::process_request_queue_ ()
{
    if (queue_.size() == 0)
        return;

    switch (queue_.back()->Type)
    {
        case AccountLogin1:
            {
                AccountEvent ev;
                flush_messages_on_event_ (ev);
                state_.process_event (ev);
            }
            break;

        case ConnectorCreate1:
            {
                ConnectionEvent ev;
                flush_messages_on_event_ (ev);
                state_.process_event (ev);
            }
            break;

        case SessionCreate1:
            {
                SessionEvent ev;
                flush_messages_on_event_ (ev);
                state_.process_event (ev);
            }
            break;

        case SessionSet3DPosition1:
            {
                PositionEvent ev;
                flush_messages_on_event_ (ev);
                state_.process_event (ev);
            }
            break;

        case AccountLogout1:
        case ConnectorInitiateShutdown1:
        case SessionTerminate1:
            {
                StopEvent ev;
                flush_messages_on_event_ (ev);
                state_.process_event (ev);
            }
            break;

        case ConnectorMuteLocalMic1:
        case ConnectorMuteLocalSpeaker1:
        case ConnectorSetLocalMicVolume1:
        case ConnectorSetLocalSpeakerVolume1:
            {
                AudioEvent ev;
                flush_messages_on_event_ (ev);
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

//=============================================================================
void Server::flush_messages_on_event_ (Event& ev)
{
    ev.messages.splice 
        (ev.messages.end(), 
         queue_, queue_.begin(), queue_.end());
}

