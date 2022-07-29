#include "Substate.h"

// The definitions here are split out from the *.h file because
// we need to be able to separate brining in declarations from
// definitions - because Subtate and SubstateMachine reference
// each other.

template <typename ty>
void Substate<ty>::OnLeftDown( ty& targ, SubstateMachine<ty>& ssm)
{}

template <typename ty>
void Substate<ty>::OnLeftUp( ty& targ, SubstateMachine<ty>& ssm)
{}

template <typename ty>
void Substate<ty>::OnMiddleDown( ty& targ, SubstateMachine<ty>& ssm)
{}

template <typename ty>
void Substate<ty>::OnMiddleUp( ty& targ, SubstateMachine<ty>& ssm)
{}

template <typename ty>
void Substate<ty>::OnMiddleUpHold( ty& targ, SubstateMachine<ty>& ssm)
{}

template <typename ty>
void Substate<ty>::OnRightDown( ty& targ, SubstateMachine<ty>& ssm)
{}

template <typename ty>
void Substate<ty>::OnRightUp( ty& targ, SubstateMachine<ty>& ssm)
{}

template <typename ty>
void Substate<ty>::OnEnterContext( ty& targ, SubstateMachine<ty>& ssm)
{}

template <typename ty>
void Substate<ty>::OnExitContext( ty& targ, SubstateMachine<ty>& ssm)
{}

template <typename ty>
Substate<ty>::~Substate()
{}