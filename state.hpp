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

//=============================================================================
// State machine

class Server;
struct StateMachine;
struct StartState;
struct ConnectorState;
struct AccountState;
struct SessionState;
struct DialingState;
struct StopState;

struct ViewerEvent { RequestQueue messages; };

struct StartEvent : public ViewerEvent, event <StartEvent> {};
struct AccountEvent : public ViewerEvent, event <AccountEvent> {};
struct ConnectionEvent : public ViewerEvent, event <ConnectionEvent> {};
struct SessionEvent : public ViewerEvent, event <SessionEvent> {};
struct PositionEvent : public ViewerEvent, event <PositionEvent> {};
struct HardwareSetEvent : public ViewerEvent, event <HardwareSetEvent> {};
struct DialFailedEvent : public ViewerEvent, event <DialFailedEvent> {};
struct DialSucceedEvent : public ViewerEvent, event <DialSucceedEvent> {};
struct StopEvent : public ViewerEvent, event <StopEvent> {};

struct StateMachine : state_machine <StateMachine, StartState> 
{
    Voice voice; // the current voice state
    Account account; // the current account state
    Connection connection; // the current connector state
    Session session; // the current session state
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
    typedef boost::mpl::list 
        <custom_reaction <StopEvent>, 
        custom_reaction <AccountEvent> > reactions;

    ConnectorState (my_context ctx);
    ~ConnectorState ();

    result react (const StopEvent& ev);
    result react (const AccountEvent& ev);

    StateMachine& machine;
};

struct AccountState : state <AccountState, StateMachine> 
{
    typedef boost::mpl::list 
        <custom_reaction <StopEvent>, 
        custom_reaction <SessionEvent> > reactions;

    AccountState (my_context ctx);
    ~AccountState ();

    result react (const StopEvent& ev);
    result react (const SessionEvent& ev);

    StateMachine& machine;
};

struct SessionState : state <SessionState, StateMachine> 
{
    typedef boost::mpl::list 
        <custom_reaction <StopEvent>, 
        custom_reaction <HardwareSetEvent>, 
        custom_reaction <PositionEvent> > reactions;

    SessionState (my_context ctx);
    ~SessionState (); 

    result react (const StopEvent& ev);
    result react (const HardwareSetEvent& ev);
    result react (const PositionEvent& ev);

    StateMachine& machine;
};

struct DialingState : state <DialingState, StateMachine> 
{
    typedef boost::mpl::list 
        <custom_reaction <DialSucceedEvent>, 
        custom_reaction <DialFailedEvent> > reactions;

    DialingState (my_context ctx);
    ~DialingState (); 

    result react (const DialSucceedEvent& ev);
    result react (const DialFailedEvent& ev);

    StateMachine& machine;
};


struct StopState : state <StopState, StateMachine> 
{
    StopState (my_context ctx);
    ~StopState ();

    StateMachine& machine;
};

#endif //_STATE_HPP_
