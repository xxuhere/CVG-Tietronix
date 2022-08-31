#include "cvgCoroutine.h"
#include "cvgAssert.h"

/// <summary>
/// An internal reference to the coroutine, held by the actual cvgCoroutineHandle 
/// class. This extra level of indirection allows the cvgCoroutineHandle to switch
/// between references while still automatically calling Coroutine::ImplEnd and
/// Coroutine::ImplDispose when the coroutine is removed from memory.
/// allows
/// </summary>
class _cvgCoroutineHandle
{
	friend cvgCoroutineHandle;
	cvgCoroutine* coroutine;

public:
	_cvgCoroutineHandle(cvgCoroutine* coroutine);
	~_cvgCoroutineHandle();
};


cvgCoroutine::cvgCoroutine(const std::string& tyname, const std::string& longhelp)
{
	this->tyname = tyname;
	this->longHelp = longhelp;
}

bool cvgCoroutine::Finish()
{
	if(this->finished)
		return false;

	this->finished = true;
	this->ImplEnd();
}

bool cvgCoroutine::Initialize()
{
	cvgAssert(!this->initialized, "Can only initialize coroutine once");
	cvgAssert(!this->finished, "Initializing a finished coroutine");

	if(this->initialized)
		return false;

	this->ImplStart();
	this->initialized = true;
}

bool cvgCoroutine::Reset()
{
	if(!this->finished)
	{
		if(!this->ImplEnd())
			return false;
	}

	this->finished = false;

	if(!this->ImplStart())
	{
		// If ImplStart fails, we call take-backsies on setting the
		// finished state to false. At this point the coroutine is in
		// a bad state, and the only thing left to do is disable it.
		this->finished = true;
		return false;
	}

	return true;
}

bool cvgCoroutine::MoveNext()
{
	if(this->finished)
		return false;

	bool canContinue = this->ImplStep();
	if(!canContinue)
	{ 
		this->Finish();
		return false;
	}

	return true;
}

_cvgCoroutineHandle::_cvgCoroutineHandle(cvgCoroutine* coroutine)
{
	this->coroutine = coroutine;
	this->coroutine->Initialize();
}

_cvgCoroutineHandle::~_cvgCoroutineHandle()
{
	this->coroutine->Finish();
	this->coroutine->ImplDispose();
}

cvgCoroutineHandle::cvgCoroutineHandle()
{}

cvgCoroutineHandle::cvgCoroutineHandle(cvgCoroutine* coroutine)
{
	this->reset( new _cvgCoroutineHandle(coroutine));
}

bool cvgCoroutineHandle::IsFinished()
{
	if(this->get() == nullptr)
		return true;

	return this->get()->coroutine->IsFinished();
}

bool cvgCoroutineHandle::MoveNext()
{
	if(this->get() == nullptr)
		return false;

	return this->get()->coroutine->MoveNext();
}
