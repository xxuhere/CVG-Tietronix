#pragma once

// The definitions here are split out from the *.h file because
// we need to be able to separate brining in declarations from
// definitions - because Subtate and SubstateMachine reference
// each other.

#include "SubstateMachine.h"
#include "../Utils/cvgAssert.h"

template <typename ty>
SubstateMachine<ty>::SubstateMachine(ty* data)
{
	this->data = data;
}

template <typename ty>
bool SubstateMachine<ty>::CacheSubstate(int type, std::shared_ptr<Substate<ty>> newSubstate)
{
	this->stateCache[type] = newSubstate;

	// We may want to reject requests if the key is already taken, in which
	// case we would return false.
	return true;
}

template <typename ty>
bool SubstateMachine<ty>::ChangeCachedSubstate(int type, bool force)
{
	auto itFind = this->stateCache.find(type);
	if(itFind == this->stateCache.end())
		return false;

	typename Substate<ty>::Ptr newSubstate = itFind->second;
	cvgAssert(newSubstate.get() != nullptr, "Detected illegal null HMDOp substate");


	return ChangeSubstate(newSubstate, force);
}

template <typename ty>
bool SubstateMachine<ty>::ChangeSubstate(std::shared_ptr<Substate<ty>> newSubstate, bool force)
{
	if(this->stateStack.empty())
		return this->PushSubstate(newSubstate, force);

	if(!force && this->stateStack.back() == newSubstate)
		return false;

	this->stateStack.back()->OnExitContext(*this->data, *this);
	this->stateStack[this->stateStack.size() - 1] = newSubstate;
	newSubstate->OnEnterContext(*this->data, *this);

	return true;
}

template <typename ty>
bool SubstateMachine<ty>::PushCachedSubstate(int type, bool force)
{
	auto itFind = this->stateCache.find(type);
	if(itFind == this->stateCache.end())
		return false;

	std::shared_ptr<Substate<ty>> newSubstate = itFind->second;
	cvgAssert(newSubstate.get() != nullptr, "Detected illegal null HMDOp substate");

	return this->PushSubstate(newSubstate, force);
}

template <typename ty>
bool SubstateMachine<ty>::PushSubstate(std::shared_ptr<Substate<ty>> newSubstate, bool force)
{
	if(this->stateStack.empty())
	{
		this->stateStack.push_back(newSubstate);
		newSubstate->OnEnterContext(*this->data, *this);
		return true;
	}

	if(!force && this->stateStack.back() == newSubstate)
		return false;

	this->stateStack.back()->OnExitContext(*this->data, *this);
	this->stateStack.push_back(newSubstate);
	newSubstate->OnEnterContext(*this->data, *this);
	return true;
}

template <typename ty>
bool SubstateMachine<ty>::PushSubstate( Substate<ty>* newSubstate, bool force)
{
	return this->PushSubstate(std::shared_ptr<Substate<ty>>(newSubstate), force);
}

template <typename ty>
bool SubstateMachine<ty>::PopSubstate(bool allowEmpty)
{
	if(this->stateStack.empty())
		return false;

	if(this->stateStack.size() == 1 && allowEmpty == false)
		return false;

	std::shared_ptr<Substate<ty>> top = this->stateStack.back();
	this->stateStack.pop_back();
	top->OnExitContext(*this->data, *this);

	if(!this->stateStack.empty())
		this->stateStack.back()->OnEnterContext(*this->data, *this);

	return true;
}

template <typename ty>
bool SubstateMachine<ty>::ForceExitSubstate()
{
	if(this->stateStack.empty())
		return false;

	// We could just clear the stateStack and be done with it without doing
	// formal pops, but until there's a reason to, we're going to remove
	// items by-the-numbers.
	while(this->PopSubstate(true)){}
	return true;
}

template <typename ty>
std::string SubstateMachine<ty>::PeekNameOfDepth(int depth)
{
	if(depth < 0 || depth >= this->stateStack.size())
		return "";

	return this->stateStack[depth]->GetStateName();
}