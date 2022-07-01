#pragma once


#include <string>
#include <memory>

template <class>
class SubstateMachine;

/// <summary>
/// A substate of UI and input logic. These are made into classes
/// so their logic can be cohesively grouped together.
/// </summary>
template <class ty>
class Substate
{
public:

	// The state functions parameters will pretty much be a 
	// combination of 
	// - custom data:
	//		Which is just extra data tied to the state machine
	//		for convenience. This will usually be extra state 
	//		information or an important object of focus.
	// - The parent state machine:
	//		The state machine that invokes the callback. This is
	//		provided so the state handlers can access it to request
	//		a state change as a response to the event.
	//	

	/// <summary>
	/// Callback when the left button is pressed.
	/// </summary>
	/// <param name="targ">The custom user data</param>
	/// <param name="ssm">The parent state machine.</param>
	virtual void OnLeftDown( ty& targ, SubstateMachine<ty>& ssm);

	/// <summary>
	/// Callback when the left button is released.
	/// </summary>
	/// <param name="targ">The custom user data</param>
	/// <param name="ssm">The parent state machine.</param>
	virtual void OnLeftUp( ty& targ, SubstateMachine<ty>& ssm);

	/// <summary>
	/// Callback when the middle button is pressed.
	/// </summary>
	/// <param name="targ">The custom user data</param>
	/// <param name="ssm">The parent state machine.</param>
	virtual void OnMiddleDown( ty& targ, SubstateMachine<ty>& ssm);

	/// <summary>
	/// Callback when the middle button is released.
	/// </summary>
	/// <param name="targ">The custom user data</param>
	/// <param name="ssm">The parent state machine.</param>
	virtual void OnMiddleUp( ty& targ, SubstateMachine<ty>& ssm);

	/// <summary>
	/// Callback when the middle button is held down past the
	/// release time, and then released.
	/// </summary>
	/// <param name="targ">The custom user data</param>
	/// <param name="ssm">The parent state machine.</param>
	virtual void OnMiddleUpHold( ty& targ, SubstateMachine<ty>& ssm);

	/// <summary>
	/// Callback when the right button is pressed.
	/// </summary>
	/// <param name="targ">The custom user data</param>
	/// <param name="ssm">The parent state machine.</param>
	virtual void OnRightDown( ty& targ, SubstateMachine<ty>& ssm);

	/// <summary>
	/// Callback when the right button is released.
	/// </summary>
	/// <param name="targ">The custom user data</param>
	/// <param name="ssm">The parent state machine.</param>
	virtual void OnRightUp(	ty& targ, SubstateMachine<ty>& ssm);

	/// <summary>
	/// Callback for when the substate is activated.
	/// </summary>
	/// <param name="targ">The custom user data</param>
	/// <param name="ssm">The parent state machine.</param>
	virtual void OnEnterContext( ty& targ, SubstateMachine<ty>& ssm);

	/// <summary>
	/// Callback for when the substate is deactivated.
	/// </summary>
	/// <param name="targ">The custom user data</param>
	/// <param name="ssm">The parent state machine.</param>
	virtual void OnExitContext( ty& targ, SubstateMachine<ty>& ssm);

	/// <summary>
	/// Provides a human-readable name of the class for debug purposes.
	/// </summary>
	virtual std::string GetStateName() const = 0;

	virtual ~Substate();

public:
	typedef std::shared_ptr<Substate<ty>> Ptr;
};
