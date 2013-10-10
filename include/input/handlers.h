/*! \file handlers.h
	\author HiddenSeeker

	This file contains implementations for window input handlers, which can be used as 
	callbacks to handle window input.
 */
#pragma once
#include "input/events.h"

namespace sad
{

namespace input
{

/*! A basic calback for handling window input
 */
template<
typename _EventType
>
class AbstractHandler
{
public:
	/*! Invokes a callback. Used by sad::Input
		\param[in] e event type
	 */
	virtual void invoke(const _EventType & e) = 0;
	/*! Kept for inheritance compliance
	 */
	virtual ~AbstractHandler() {}
};

/*! A callback, which calls a simple function, which is specified on creation.
 */
template<
typename _EventType
>
class FreeFunctionHandler: public sad::input::AbstractHandler<_EventType>
{
private:
	void (*m_f)(const _EventType &); //!< Called function
public:
	/*! Constructs new handler
		\param[in] f callback
	 */
	FreeFunctionHandler(void (*f)(const _EventType &)) : m_f(f)
	{
	}

	/*! Invokes a callback function
		\param[in] e event type
	 */
	virtual void invoke(const _EventType & e)
	{
		m_f(e);
	}
};

/*! A callback, which calls a simple function, which is specified on creation.
	The function does not care about event.
 */
template<
typename _EventType
>
class VoidFreeFunctionHandler: public sad::input::AbstractHandler<_EventType>
{
private:
	void (*m_f)(); //!< Called function
public:
	/*! Constructs new handler
		\param[in] f callback
	 */
	VoidFreeFunctionHandler(void (*f)()) : m_f(f)
	{
	}

	/*! Invokes a callback function
		\param[in] e event type
	 */
	virtual void invoke(const _EventType & e)
	{
		m_f();
	}
};

/*! A callback, which calls a method on specified object. Both method an object, 
	are specified on creation
 */
template<
typename _EventType,
typename _ObjectClass,
typename _MethodCallback
>
class MethodHandler: public sad::input::AbstractHandler<_EventType>
{
private:
	/*! Inner object, which callback will be called on
	 */
	_ObjectClass * m_o;                             
	/*! Method, which specifies callback
	 */
	_MethodCallback m_f;      
public:
	/*! Constructs new handler
		\param[in] o object
		\param[in] f callback, which will be called
	 */
	MethodHandler(
		_ObjectClass * o,
		_MethodCallback f
	) : m_o(o), m_f(f)
	{
	}

	/*! Invokes a callback function
		\param[in] e event type
	 */
	virtual void invoke(const _EventType & e)
	{
		(m_o->*m_f)(e);
	}
};

/*! A callback, which calls a method on specified object. Both method an object, 
	are specified on creation.
	A callback does not care for a data
 */
template<
typename _EventType,
typename _ObjectClass,
typename _MethodCallback
>
class VoidMethodHandler: public sad::input::AbstractHandler<_EventType>
{
private:
	/*! Inner object, which callback will be called on
	 */
	_ObjectClass * m_o;                             
	/*! Method, which specifies callback
	 */
	_MethodCallback * m_f;      
public:
	/*! Constructs new handler
		\param[in] o object
		\param[in] f callback, which will be called
	 */
	VoidMethodHandler(
		_ObjectClass * o,
		_MethodCallback * f
	) : m_o(o), m_f(f)
	{
	}

	/*! Invokes a callback function
		\param[in] e event type
	 */
	virtual void invoke(const _EventType & e)
	{
		(m_o->*m_f)();
	}
};

/*! A callback, which composes two method calls on specified objects. All parameters, are
	specified, when creating objects
 */
template<
typename _EventType,
typename _ObjectClass,
typename _FCallback,
typename _GCallback
>
class CompositeHandler: public sad::input::AbstractHandler<_EventType>
{
private:
	/*! Inner object, which callback will be called on
	 */
	_ObjectClass * m_o;                             
	/*! Method, which returns object for callback
	 */
	_FCallback m_f;   
	/*! Method, which is called, when event is raised
	 */
	_GCallback m_g;
public:
	/*! Constructs new handler
		\param[in] o object
		\param[in] f callback, which returns intermediate object
		\param[in] g callback, which is called on event
	 */
	CompositeHandler(
		_ObjectClass * o,
		_FCallback f,
		_GCallback g
	) : m_o(o), m_f(f), m_g(g)
	{
	}

	/*! Invokes a callback function
		\param[in] e event type
	 */
	virtual void invoke(const _EventType & e)
	{
		(((m_o->*m_f)()) ->* m_g) (e);
	}
};

/*! A callback, which composes two method calls on specified objects. All parameters, are
	specified, when creating objects
	
	An ending callback does not care for event
 */
template<
typename _EventType,
typename _ObjectClass,
typename _FCallback,
typename _GCallback
>
class VoidCompositeHandler: public sad::input::AbstractHandler<_EventType>
{
private:
	/*! Inner object, which callback will be called on
	 */
	_ObjectClass * m_o;                             
	/*! Method, which returns object for callback
	 */
	_FCallback m_f;   
	/*! Method, which is called, when event is raised
	 */
	_GCallback m_g;
public:
	/*! Constructs new handler
		\param[in] o object
		\param[in] f callback, which returns intermediate object
		\param[in] g callback, which is called on event
	 */
	VoidCompositeHandler(
		_ObjectClass * o,
		_FCallback f,
		_GCallback g
	) : m_o(o), m_f(f), m_g(g)
	{
	}

	/*! Invokes a callback function
		\param[in] e event type
	 */
	virtual void invoke(const _EventType & e)
	{
		(((m_o->*m_f)()) ->* m_g) ();
	}
};

}

}
