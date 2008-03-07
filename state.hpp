/* state.hpp -- state definition
 *
 *			Ryan McDougall -- 2008
 */

#ifndef _STATE_HPP_
#define _STATE_HPP_

//=============================================================================
// State machine

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
struct StopEvent : public ViewerEvent, event <StopEvent> {};

struct StateMachine : state_machine <StateMachine, StartState> 
{
    StateMachine (Server *s) : server (s), bridge (NULL) {}

    Server *server;
    auto_ptr <SIPConference> bridge;
};

struct StartState : simple_state <StartState, StateMachine> 
{
    typedef custom_reaction <ConnectionEvent> reactions;

    StartState ();
    ~StartState ();

    result react (const ConnectionEvent& ev);
};

struct ConnectorState : simple_state <ConnectorState, StateMachine> 
{
    typedef custom_reaction <AccountEvent> reactions;

    ConnectorState ();
    ~ConnectorState ();

    result react (const AccountEvent& ev);
};

struct AccountState : simple_state <AccountState, StateMachine> 
{
    typedef custom_reaction <SessionEvent> reactions;

    AccountState ();
    ~AccountState ();

    result react (const SessionEvent& ev) 
};

struct SessionState : simple_state <SessionState, StateMachine> 
{
    typedef custom_reaction <StopEvent> reactions;

    SessionState ();
    ~SessionState (); 

    result react (const StopEvent& ev);
};

struct StopState : simple_state <StopState, StateMachine> 
{
    StopState ();
    ~StopState ();
};

#endif //_STATE_HPP_
