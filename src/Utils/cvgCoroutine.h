#pragma once
#include <string>
#include <memory>

/// <summary>
/// Provides a coroutine(ish) functionality to cvg, allowing compact
/// state machine progressing logic to be executed as an object.
/// 
/// It may be possible to use C++ coroutine in the future, but this
/// class adds additional features and coroutines are a C++0x20 feature
/// which, by the 5 year rule of the repository, shouldn't be used in
/// the codebase until 2025.
/// 
/// Coroutines will be implemented by subclassing off the cvgCoroutine
/// class, but should ONLY be used through cvgCoroutineHandle as it
/// provides extra management that is required for coroutines to
/// run correctly.
/// </summary>
class cvgCoroutine
{
	friend class _cvgCoroutineHandle;

private:
	/// <summary>
	/// Tracks if the coroutine has already been initialized. This is
	/// mostly a proactive guard against multiple initialization.
	/// </summary>
	bool initialized = false;

	/// <summary>
	/// Tracks if the coroutine has been finished.
	/// </summary>
	bool finished = false;

	/// <summary>
	/// The human-readable description of the coroutine, in case things
	/// want to query more information about the coroutine. This is a 
	/// non-functional (descriptive) arbitrary value.
	/// </summary>
	std::string tyname;

	/// <summary>
	/// The human-readable description of the coroutine. This is similar
	/// to tyname, but served to provide a longer name.
	/// </summary>
	std::string longHelp;

protected:

	/// <summary>
	/// Subclasses should override this to provide the logic for what
	/// happens when the subclass starts.
	/// 
	/// Note that a coroutine may call ImplStart() multiple times if the
	/// coroutine is reset.
	/// </summary>
	/// <returns>Return true if there are no errors.</returns>
	virtual bool ImplStart() = 0;

	/// <summary>
	/// Subclasses should override this to provide the logic for what
	/// happens when the coroutine ends.
	/// </summary>
	/// <returns>Return false if there are no errors.</returns>
	virtual bool ImplEnd() = 0;

	/// <summary>
	/// Subclasses should override this to provide the logic for what
	/// happens when the coroutine advances a step.
	/// </summary>
	/// <returns>
	/// Return true if the coroutine can continue advancing afterwards.
	/// Else, if false is returned, the coroutine will be finished.
	/// </returns>
	virtual bool ImplStep() = 0;

	/// <summary>
	/// Subclasses should override this provide the logic for what
	/// happens when the coroutine is remove from memory.
	/// </summary>
	/// <returns>If true, successful.</returns>
	virtual bool ImplDispose() = 0;

public:

	/// <summary>
	/// Constructor.
	/// </summary>
	/// <param name="tyname">Short human-readable description of the coroutine.</param>
	/// <param name="longhelp">Optional longer human-readable description of the coroutine.</param>
	cvgCoroutine(const std::string& tyname, const std::string& longhelp);

	/// <summary>
	/// Initialize the coroutine. This will be set when placing the
	/// coroutine in cvgCoroutineHandle.
	/// </summary>
	/// <returns>Successful if true.</returns>
	bool Initialize();

	/// <summary>
	/// Resets the coroutine.
	/// </summary>
	/// <returns>Successful if true.</returns>
	bool Reset();

	/// <summary>
	/// Do another step in the coroutine.
	/// </summary>
	/// <returns>
	/// If true, the coroutine has been stepped, and can still step. Else
	/// there was either an error, or the coroutine has finished.
	/// 
	/// This allows the coroutine to be used in a while loop:
	///	e.g.,
	///		while(coroutine->MoveNext()){}
	/// </returns>
	bool MoveNext();

	/// <summary>
	/// Force the coroutine to end.
	/// </summary>
	/// <returns>If successful, true.</returns>
	bool Finish();

	/// <summary>
	/// Query if the coroutine is in a finished state.
	/// </summary>
	/// <returns>If true, the coroutine has finished.</returns>
	inline const bool IsFinished() const
	{ return this->finished; }

	inline std::string TypeName() const
	{ return this->tyname; }

	inline std::string LongHelp() const
	{ return this->longHelp; }
};

// private to the .h, 
// defined in the .cpp.
class _cvgCoroutineHandle;

/// <summary>
/// A wrapper for coroutine handles that provides extra mandatory
/// state-keeping utilities, as well as C++ shared pointer support.
/// </summary>
class cvgCoroutineHandle : 
	public std::shared_ptr<_cvgCoroutineHandle>
{
public:
	/// <summary>
	/// Constructor.
	/// </summary>
	/// <param name="coroutine">The coroutine manage.</param>
	cvgCoroutineHandle(cvgCoroutine* coroutine);

	cvgCoroutineHandle();

	/// <summary>
	/// Delegates an IsFinished() query to the wrapped coroutine.
	/// </summary>
	bool IsFinished();

	/// <summary>
	/// Delegates a MoveNext() operation to the wrapped coroutine.
	/// </summary>
	bool MoveNext();

	/// <summary>
	/// Checks if the handle has a coroutine assigned to it.
	/// </summary>
	bool IsValid()
	{ return this->get() != nullptr; }
};