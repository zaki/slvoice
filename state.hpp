/* state.hpp -- state definition
 *
 *			Ryan McDougall -- 2008
 */

#ifndef _STATE_HPP_
#define _STATE_HPP_

#include <boost/mpl/list.hpp>
#include <boost/statechart/event.hpp>
#include <boost/statechart/custom_reaction.hpp>
#include <boost/statechart/state_machine.hpp>
#include <boost/statechart/state.hpp>

using namespace boost::statechart;
using namespace std;

//=============================================================================
// State machine

class Server;
struct StateMachine;
struct StartState;
struct ConnectorState;
struct AccountState;
struct SessionState;
struct StopState;

struct ViewerEvent { RequestQueue messages; };

struct StartEvent : public ViewerEvent, event <StartEvent> {};
struct AccountEvent : public ViewerEvent, event <AccountEvent> {};
struct ConnectionEvent : public ViewerEvent, event <ConnectionEvent> {};
struct SessionEvent : public ViewerEvent, event <SessionEvent> {};
struct PositionEvent : public ViewerEvent, event <PositionEvent> {};
struct StopEvent : public ViewerEvent, event <StopEvent> {};

struct StateMachine : state_machine <StateMachine, StartState> 
{
    StateMachine (Server *s) : server (s), bridge (NULL) {}
    
    VoiceState voice; // the current voice state
    Server *server; // for sending messages across the network
    auto_ptr <SIPConference> bridge; // for creating a conference bridge
};

struct StartState : state <StartState, StateMachine> 
{
    typedef custom_reaction <ConnectionEvent> reactions;

    StartState (my_context ctx);
    ~StartState ();

    result react (const ConnectionEvent& ev);

    StateMachine& machine;
};

struct ConnectorState : state <ConnectorState, StateMachine> 
{
    typedef custom_reaction <AccountEvent> reactions;

    ConnectorState (my_context ctx);
    ~ConnectorState ();

    result react (const AccountEvent& ev);

    StateMachine& machine;
};

struct AccountState : state <AccountState, StateMachine> 
{
    typedef custom_reaction <SessionEvent> reactions;

    AccountState (my_context ctx);
    ~AccountState ();

    result react (const SessionEvent& ev);

    StateMachine& machine;
};

struct SessionState : state <SessionState, StateMachine> 
{
    typedef boost::mpl::list 
        <custom_reaction <StopEvent>, 
        custom_reaction <PositionEvent> > reactions;

    SessionState (my_context ctx);
    ~SessionState (); 

    result react (const PositionEvent& ev);
    result react (const StopEvent& ev);

    StateMachine& machine;
};

struct StopState : state <StopState, StateMachine> 
{
    StopState (my_context ctx);
    ~StopState ();

    StateMachine& machine;
};

#endif //_STATE_HPP_
