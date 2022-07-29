#pragma once

#include <map>
#include <vector>
#include <memory>

template <class ty>
class Substate;

// NOTE: This class is called SubstateMachine because it's a state machine
// that's designed to be a state machine inside a state machine (See
// BaseState). Therefore, when referring to SubstateMachine, describing
// it as a state machine and sub-state machine will be interchanged and
// considered synonymous.

/// <summary>
/// A state machine for mouse click contexts, designed to be used
/// inside of BaseState instances.
/// </summary>
/// <typeparam name="ty">The datatype of the callback data.</typeparam>
template <typename ty>
class SubstateMachine
{
	// Originally this was specifically for the various input states 
	// of StateHMDOp, but the pattern has been generalized as a 
	// templated class for reuse - including leaving open the possibility
	// of reusing for other (future) BaseState subclasses.

private:
	/// <summary>
	/// The storage of cached states. Cached states are states that
	/// are persistent for the lifetime of the SubstateMachine, and
	/// can be referenced with integer IDs. What integers map to
	/// what states is up to the user.
	/// </summary>
	std::map<int, std::shared_ptr<Substate<ty>>> stateCache;

	/// <summary>
	/// The Substate statemachine task. The back of the vector is
	/// considered the current state. States can either be entered
	/// by pushing and poping to reserve and return to stored states, 
	/// or the back element can be changed directly. </summary>
	std::vector< std::shared_ptr<Substate<ty>>> stateStack;
	//
	// The terminology in the comments may be confusing, as the current
	// state is the "back of the vector" (i.e. stateStack.back()), but
	// when thinking of stateStack as a stack datastructure, that's also
	// the 'top' of the stack. But in an abstract way these terms (out of
	// context) can be seen as antonyms of each other. For now the terms
	// are used in the comments freely, whatever felt natural as the author
	// was commenting (wleu 07/01/2022)

	/// <summary>
	/// A cache of the value to send in to Substate callbacks for
	/// their targ parameter.
	/// </summary>
	ty* data;

public:

	/// <summary>
	/// Constructor.
	/// </summary>
	/// <param name="data">The data to submit to substate callbacks.</param>
	SubstateMachine(ty* data);

	/// <summary>
	/// Register a substate as a cached state that can be reused by
	/// referencing it with an ID.
	/// </summary>
	/// <param name="type">The id to map to the substate.</param>
	/// <param name="newSubstate">The substate to cache.</param>
	/// <returns>
	/// True if the substate was successfully registered.
	/// Else, the substate request was ignored.
	/// </returns>
	bool CacheSubstate(int type, std::shared_ptr<Substate<ty>> newSubstate);

	/// <summary>
	/// Change the current substate via cache id.
	/// </summary>
	/// <param name="type">The id of the substate to change to.</param>
	/// <param name="force">
	/// If false, the request will be ignored if the top state is already the
	/// requested state. If true and the top state is already active, it will
	/// be forced to exit and reenter.
	/// </param>
	/// <returns>True if a substate change happened. Else, false.</returns>
	bool ChangeCachedSubstate(int type, bool force = false);

	/// <summary>
	/// Change the current substate.
	/// </summary>
	/// <param name="newSubstate">The substate to change to.</param>
	/// <param name="force">
	/// If false, the request will be ignored if the top state is already the
	/// requested state. If true and the top state is already active, it will
	/// be forced to exit and reenter.
	/// </param>
	/// <returns>True if a substate change happened. Else, false.</returns>
	bool ChangeSubstate( std::shared_ptr<Substate<ty>> newSubstate, bool force = false);

	/// <summary>
	/// Push a substate as the current.
	/// </summary>
	/// <param name="type">The id of the substate to push.</param>
	/// <param name="force">
	/// If false, the request will be ignored if the top state is already the
	/// requested state. If true and the top state is already active, it will
	/// be forced to exit and reenter - as well as being push on top of itself.
	/// </param>
	/// <returns>True if the substate was pushed. Else, false.</returns>
	bool PushCachedSubstate(int type, bool force = false);

	/// <summary>
	/// Push a substate as the current.
	/// </summary>
	/// <param name="newSubstate">The substate to add.</param>
	/// <param name="force">
	/// If false, the request will be ignored if the top state is already the
	/// requested state. If true and the top state is already active, it will
	/// be forced to exit and reenter - as well as being push on top of itself.
	/// </param>
	/// <returns>True if the substate was pushed. Else, false.</returns>
	bool PushSubstate( std::shared_ptr<Substate<ty>> newSubstate, bool force = false);

	/// <summary>
	/// Push a substate as the current.
	/// 
	/// This convenience function overload also handles wrapping the raw substate
	/// into the required shared pointer. Note that after being added by this function,
	/// nothing else should claim ownership of the input parameter.
	/// </summary>
	/// <param name="newSubstate">The substate to push.</param>
	/// <param name="force">
	/// If false, the request will be ignored if the top state is already the
	/// requested state. If true and the top state is already active, it will
	/// be forced to exit and reenter - as well as being push on top of itself.
	/// </param>
	/// <returns>
	/// True if the substate was pushed. Else, false.
	/// 
	/// Under nominal cases, this function should not return false, but if it
	/// does, make note that the caller should still disown the raw pointer.
	/// </returns>
	bool PushSubstate( Substate<ty>* newSubstate, bool force = false);

	/// <summary>
	/// Pop the active substate.
	/// </summary>
	/// <param name="allowEmpty">
	/// If false, the request to PopSubstate will be ignored if there is only
	/// 1 substate on the stack. This will prevent the SubstateMachine's stack
	/// from being completly empty.
	/// </param>
	/// <returns>
	/// True if the stack was popped. Else, false.
	/// </returns>
	bool PopSubstate(bool allowEmpty = false);

	/// <summary>
	/// Clears all substates. The state machine will be completly empty
	/// </summary>
	/// <returns>Returns false if the state machine was already empty.</returns>
	bool ForceExitSubstate();

	/// <summary>
	/// Get the current substate in the state machine.
	/// </summary>
	/// <returns>The current substate, or null if the state machine is empty.</returns>
	inline typename Substate<ty>::Ptr GetCurSubtate()
	{ return this->stateStack.empty() ? nullptr : this->stateStack.back(); }

	/// <summary>
	/// Get the depth of the stack for the state machine. 
	/// 
	/// Only intended for debug/diagnostics.
	/// </summary>
	/// <returns>The stack size of the state machine.</returns>
	int GetStackDepth()
	{ return this->stateStack.size(); }

	/// <summary>
	/// Get the name of the substate at a specific depth.
	/// 
	/// Only intended for debug/diagnostics.
	/// </summary>
	/// <param name="depth">
	/// The depth to retrieve, where the current depth is GetStackDepth() - 1.
	/// </param>
	/// <returns>
	/// The name of the substate at the specified depth, or an empty string
	/// if there was nothing to return.
	/// </returns>
	std::string PeekNameOfDepth(int depth);

	~SubstateMachine()
	{ }
};
