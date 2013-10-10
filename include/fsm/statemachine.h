/*! \file   statemachine.h
    \author HiddenSeeker

	File, containing a definition of statemachine and global game state
*/
#pragma once

#include "../types.h"
#include "../input.h"
#include "../sadhash.h"

#include <stdlib.h>


namespace sad
{

namespace fsm
{

/*! Defines a shared information between any states
 */
class Shared
{
 public:
	 template<typename T> inline T & as() { return static_cast<T&>(*this); }
	 template<typename T> inline const T & as() const{ return static_cast<const T&>(*this); }
	 virtual ~Shared();
};

class Machine;
/*! Defines a callback, which can be invoked in state
 */
class AbstractCallback
{
 private:
 public:
	/*! Clones a callback
	 */
	 virtual AbstractCallback * clone()=0;
	 /*! Invocation method, used when StateMachine invokes a callback
	  */
	 virtual void invoke()=0;
	 /*! Invocation method, used when Input invokes a callback
	  */
	 virtual void invoke(const sad::Event & o)=0;
	 /*! Destructor
	  */
	 virtual ~AbstractCallback();
};
/*! Defines a free function invocation method denoted with type  void (*)()
 */
class FreeFunction: public AbstractCallback
{
 private:
	 void (*m_function)(); //!< A called callback
 public:
	 inline FreeFunction(void (*function)()) : m_function(function) {}
 	/*! Clones a callback
	 */
	 virtual AbstractCallback * clone();
	 /*! Invocation method, used when StateMachine invokes a callback
	  */
	 virtual void invoke();
	 /*! Invocation method, used when Input invokes a callback
	  */
	 virtual void invoke(const sad::Event & o);
};

/*! Defines a free function invocation method denoted with type  void (*)(const sad::Event &)
 */
class FreeEventFunction: public AbstractCallback
{
 private:
	 void (*m_function)(const sad::Event &); //!< A called callback
 public:
	 inline FreeEventFunction(void (*function)(const sad::Event &)) : m_function(function) {}
	 /*! Not invoked, since this is saddy event callback
	  */
	 virtual void invoke();
	 /*! Clones a callback
	 */
	 virtual AbstractCallback * clone();
	 /*! Invocation method, used when Input invokes a callback
	  */
	 virtual void invoke(const sad::Event & o);
};

/*! Defines a class method function invocation as void(*)()
 */
template<
typename _Class
>
class MethodCallback: public AbstractCallback
{
 private:
	 _Class * m_o;
	 void (_Class::*m_function)();
 public:
	 inline MethodCallback(_Class * o, void (_Class::*function)() ) : m_o(o), m_function(function) 
	 {}
	 /*! Clones a callback
	 */
	 virtual AbstractCallback * clone() { return new MethodCallback<_Class>(*this); }
	 /*! Not invoked, since this is saddy event callback
	  */
	 virtual void invoke() { (m_o->*m_function)(); }
	 /*! Invocation method, used when Input invokes a callback
	  */
	 virtual void invoke(const sad::Event & o) { (m_o->*m_function)(); }
};

/*! Defines a class method function invocation as void(*)(const sad::Event &)
 */
template<
typename _Class
>
class MethodEventCallback: public AbstractCallback
{
 private:
	 _Class * m_o;
	 void (_Class::*m_function)(const sad::Event &);
 public:
	 inline MethodEventCallback(_Class * o, void (_Class::*function)(const sad::Event &) ) : 
	 m_o(o), m_function(function) 
	 {}
	 /*! Clones a callback
	 */
	 virtual AbstractCallback * clone() { return new MethodEventCallback<_Class>(*this); }
	 /*! Not invoked, since this is saddy event callback
	  */
	 virtual void invoke() { (m_o->*m_function)(sad::Event()); }
	 /*! Invocation method, used when Input invokes a callback
	  */
	 virtual void invoke(const sad::Event & o) { (m_o->*m_function)(o); }
};

/*! A callback with delegated method invocation
 */
template<
typename _DelegatingClass,
typename _InvokedClass
>
class DelegatedMethodCallback: public AbstractCallback
{
 private:
	 _DelegatingClass * m_o;
	 _InvokedClass * (_DelegatingClass::*m_getter)();
	 void (_InvokedClass::*m_function)();
 public:
	 inline DelegatedMethodCallback(
		 _DelegatingClass * o, 
		 _InvokedClass * (_DelegatingClass::*getter)(),
		 void (_InvokedClass::*function)()
		 ) : 
	 m_o(o), m_getter(getter), m_function(function) 
	 {}
	 /*! Clones a callback
	 */
	 virtual AbstractCallback * clone() { return new DelegatedMethodCallback<_DelegatingClass, _InvokedClass>(*this); }
	 /*! Not invoked, since this is saddy event callback
	  */
	 virtual void invoke() 
	 { 
		 _InvokedClass * p = (m_o->*m_getter)();
		 (p->*m_function)();
	 }
	 /*! Invocation method, used when Input invokes a callback
	  */
	 virtual void invoke(const sad::Event & o) 
	 { 
		 _InvokedClass * p = (m_o->*m_getter)();
		 (p->*m_function)();
	 }
};
/*! A callback with delegated method invocation
 */
template<
typename _DelegatingClass,
typename _InvokedClass
>
class DelegatedEventMethodCallback: public AbstractCallback
{
 private:
	 _DelegatingClass * m_o;
	 _InvokedClass * (_DelegatingClass::*m_getter)();
	 void (_InvokedClass::*m_function)(const sad::Event & o);
 public:
	 inline DelegatedEventMethodCallback(
		 _DelegatingClass * o, 
		 _InvokedClass * (_DelegatingClass::*getter)(),
		 void (_InvokedClass::*function)(const sad::Event & o)
		 ) : 
	 m_o(o), m_getter(getter), m_function(function) 
	 {}
	 /*! Clones a callback
	 */
	 virtual AbstractCallback * clone() { return new DelegatedEventMethodCallback<_DelegatingClass, _InvokedClass>(*this); }
	 /*! Not invoked, since this is saddy event callback
	  */
	 virtual void invoke() 
	 { 
		 _InvokedClass * p = (m_o->*m_getter)();
		 (p->*m_function)(sad::Event());
	 }
	 /*! Invocation method, used when Input invokes a callback
	  */
	 virtual void invoke(const sad::Event & o) 
	 { 
		 _InvokedClass * p = (m_o->*m_getter)();
		 (p->*m_function)(o);
	 }
};


class Callback: public AbstractCallback
{
 private:
	AbstractCallback * m_impl; //!< Inner implementation of callback
	/*! Copies another callback
	 */
	void copy(const fsm::Callback & other);
 public:
	inline Callback() { m_impl = NULL; }
	Callback(const Callback & o);
	Callback & operator=(const Callback &o);
	/*! Clones a callback
	 */
	virtual AbstractCallback * clone();
	/*! Not invoked, since this is saddy event callback
	 */
	virtual void invoke();
	/*! Invocation method, used when Input invokes a callback
	 */
	virtual void invoke(const sad::Event & o);
	inline ~Callback() { delete m_impl; }

	Callback( void (*f)() );
	Callback( void (*f)(const sad::Event &) );

	template<typename _Class>
	Callback( _Class * o, void (_Class::*f)() )  
	{ m_impl =  new fsm::MethodCallback<_Class>(o, f); }

	template<typename _Class>
	Callback( _Class * o, void (_Class::*f)(sad::Event & o) )  
	{ m_impl =  new fsm::MethodEventCallback<_Class>(o, f); }

	template<typename _DelegatingClass, typename _InvokingClass>
	Callback( 
		_DelegatingClass * o, 
		_InvokingClass * (_DelegatingClass::*f)(), 
		void (_InvokingClass::*g)() 
	)  
	{ m_impl =  new fsm::DelegatedMethodCallback<_DelegatingClass, _InvokingClass>(o, f, g); }

	template<typename _DelegatingClass, typename _InvokingClass>
	Callback( 
		_DelegatingClass * o, 
		_InvokingClass * (_DelegatingClass::*f)(), 
		void (_InvokingClass::*g)(const sad::Event & o) 
	)  
	{ m_impl =  new fsm::DelegatedEventMethodCallback<_DelegatingClass, _InvokingClass>(o, f, g); }

};

/*! A names for callbacks from State Lists
 */
class Names
{
public:
	/*! Enter state callback name
	 */
	static const sad::String ENTER;
	/*! Leave state callback name
	 */
	static const sad::String LEAVE;
	/*! A callback for mouse down event
	 */
	static const sad::String MOUSEDOWN;
	/*!  A callback for mouse up event
	 */
	static const sad::String MOUSEUP;
	/*!  A callback for mouse click event
	 */
	static const sad::String MOUSECLICK;
	/*!  A callback for mouse double click event
	 */
	static const sad::String MOUSEDBLCLICK;
	/*!  A callback for mouse move event
	 */
	static const sad::String MOUSEMOVE;
	/*!  A callback for mouse move event
	 */
	static const sad::String WHEEL;
	/*!  A callback for mouse move event
	 */
	static const sad::String KEYDOWN;
	/*!  A callback for mouse move event
	 */
	static const sad::String KEYUP;
	/*!  A callback for quit
	 */
	static const sad::String QUIT;
};

/*! An interface for handling state transitions
 */
class State
{
private:
	typedef sad::Hash<sad::String, AbstractCallback *> CallbackList;
	typedef sad::Hash<int, AbstractCallback *> CharCallbackList;

	/*! Callbacks for hashes data
	 */
	CallbackList m_callbacks;
	/*! Callbacks for key up list
	 */
	CharCallbackList m_keyup; 
	/*! Callbacks for key up list
	 */
	CharCallbackList m_keydown; 
	/*! Link to machine, where action is stored
	 */
	fsm::Machine * m_machine;
	/*! Erase a callback list
		\param[in] c list
	 */
	void eraseCallbackList(CallbackList & c);
		/*! Erase a callback list
		\param[in] c list
	 */
	void eraseCallbackList(CharCallbackList & c);
public:
	State();
	virtual void setMachine(fsm::Machine * machine);
	/*! Adds a new callback for event. Class takes ownership on callback 
		\param[in] eventType type of event for state
		\param[in] callback a callback data
	 */
    virtual void addCallback(const sad::String & eventType, fsm::AbstractCallback * callback);
	/*! Adds new callback on enter
		\param[in] f function
	 */
	inline void addEnterCallback(void (*f)())
	{ this->addCallback(fsm::Names::ENTER, new fsm::Callback(f)); }
	/*! Adds new callback on enter
		\param[in] o object
		\param[in] f function
	 */
	template<typename _Class>
	void addEnterCallback(_Class * o, void (_Class::*f)())
	{ this->addCallback(fsm::Names::ENTER, new fsm::Callback(o, f)); }
	/*! Adds new callback on leave
		\param[in] f function
	 */
	inline void addLeaveCallback(void (*f)())
	{ this->addCallback(fsm::Names::LEAVE, new fsm::Callback(f)); }
	/*! Adds new callback on leave
		\param[in] o object
		\param[in] f function
	 */
	template<typename _Class>
	void addLeaveCallback(_Class * o, void (_Class::*f)())
	{ this->addCallback(fsm::Names::LEAVE, new fsm::Callback(o, f)); }
	/*! Adds a new callback for event. Class takes ownership on callback 
		\param[in] eventType type of event for state
		\param[in] f a callback 
	 */
	inline void addEventCallback(const sad::String & eventType, void (*f)(const sad::Event &))
	{ this->addCallback(eventType, new fsm::Callback(f)); }
	/*! Adds a new callback for event. Class takes ownership on callback 
		\param[in] eventType type of event for state
		\param[in] o class object
		\param[in] f a callback 
	 */
	template<typename _Class>
	inline void addEventCallback(const sad::String & eventType, _Class * o,  void (_Class::*f)(const sad::Event &))
	{ this->addCallback(eventType, new fsm::Callback(o, f)); }
	/*! Adds a new callback for event. Class takes ownership on callback 
		\param[in] eventType type of event for state
		\param[in] o class object
		\param[in] f a function for getting second object
		\param[in] g a callback
	 */
	template<typename _DelegatingClass, typename _InvokedClass>
	inline void addEventCallback(
		const sad::String & eventType, 
		_DelegatingClass * o,  
		_InvokedClass * (_DelegatingClass::*f)(),
		void (_InvokedClass::*g)()
	)
	{ this->addCallback(eventType, new fsm::Callback(o, f, g)); }
	/*! Adds a new callback for event. 
		\param[in] eventType type of event for state
		\param[in] o class object
		\param[in] f getter callback
		\param[in] g invoked callback
	 */
	template<
		typename _DelegatingClass,
		typename _InvokedClass
	>
	inline void addEventCallback(
		const sad::String & eventType, 
		_DelegatingClass * o,  
		_InvokedClass * (_DelegatingClass::*f)(),
		void (_InvokedClass::*g)(const sad::Event &)
	)
	{ this->addCallback(eventType, new fsm::Callback(o, f, g)); }
	/*! Removes a callback for eventtype
		\param[in] eventType type of event
	*/
	virtual void removeCallback(const sad::String & eventType);
	/*! Adds a callback for key up event on specific key
		\param[in] key key which it will be bind to
		\param[in] c callback
	 */
	virtual void addKeyUpCallback(int key, fsm::AbstractCallback * c);
	/*! Adds a new callback for event. Class takes ownership on callback 
		\param[in] key a handled key
		\param[in] f callback 
	 */
	inline void addKeyUpCallback(int key, void (*f)())
	{ this->addKeyUpCallback(key, new fsm::Callback(f)); }
	/*! Adds a new callback for event. Class takes ownership on callback 
		\param[in] key a handled key
		\param[in] f callback 
	 */
	inline void addKeyUpCallback(int key, void (*f)(const sad::Event &))
	{ this->addKeyUpCallback(key, new fsm::Callback(f)); }
	/*! Adds a new callback for event. Class takes ownership on callback 
		\param[in] key a handled key
		\param[in] o class object
		\param[in] f a callback 
	 */
	template<typename _Class>
	inline void addKeyUpCallback(int key, _Class * o,  void (_Class::*f)(const sad::Event &))
	{ this->addKeyUpCallback(key, new fsm::Callback(o, f)); }
	/*! Adds a new callback for keyup event. 
		\param[in] key key data
		\param[in] o class object
		\param[in] f getter callback
		\param[in] g invoked callback
	 */
	template<
		typename _DelegatingClass,
		typename _InvokedClass
	>
	inline void addKeyUpCallback(
		int key, 
		_DelegatingClass * o,  
		_InvokedClass * (_DelegatingClass::*f)(),
		void (_InvokedClass::*g)()
	)
	{ this->addKeyUpCallback(key, new fsm::Callback(o, f, g)); }
	/*! Adds a new callback for keyup event. 
		\param[in] key key data
		\param[in] o class object
		\param[in] f getter callback
		\param[in] g invoked callback
	 */
	template<
		typename _DelegatingClass,
		typename _InvokedClass
	>
	inline void addKeyUpCallback(
		int key, 
		_DelegatingClass * o,  
		_InvokedClass * (_DelegatingClass::*f)(),
		void (_InvokedClass::*g)(const sad::Event &)
	)
	{ this->addKeyUpCallback(key, new fsm::Callback(o, f, g)); }
	/*! Adds a callback for key down event on specific key 
		\param[in] key key which it will be bind to
		\param[in] c callback
	 */
	virtual void addKeyDownCallback(int key, fsm::AbstractCallback * c);
	/*! Adds a new callback for event. Class takes ownership on callback 
		\param[in] key a handled key
		\param[in] f a callback
	 */
	inline void addKeyDownCallback(int key, void (*f)(const sad::Event &))
	{ this->addKeyDownCallback(key, new fsm::Callback(f)); }
	/*! Adds a new callback for event. Class takes ownership on callback 
		\param[in] key a handled key
		\param[in] o class object
		\param[in] f a callback data
	 */
	template<typename _Class>
	inline void addKeyDownCallback(int key, _Class * o,  void (_Class::*f)())
	{ this->addKeyDownCallback(key, new fsm::Callback(o, f)); }
	/*! Adds a new callback for event. Class takes ownership on callback 
		\param[in] key a handled key
		\param[in] o class object
		\param[in] f a callback 
	 */
	template<typename _Class>
	inline void addKeyDownCallback(int key, _Class * o,  void (_Class::*f)(const sad::Event &))
	{ this->addKeyDownCallback(key, new fsm::Callback(o, f)); }
	/*! Adds a new callback for keydown event. 
		\param[in] key key data
		\param[in] o class object
		\param[in] f getter callback
		\param[in] g invoked callback
	 */
	template<
		typename _DelegatingClass,
		typename _InvokedClass
	>
	inline void addKeyDownCallback(
		int key, 
		_DelegatingClass * o,  
		_InvokedClass * (_DelegatingClass::*f)(),
		void (_InvokedClass::*g)()
	)
	{ this->addKeyDownCallback(key, new fsm::Callback(o, f, g)); }
	/*! Adds a new callback for keydown event. 
		\param[in] key key data
		\param[in] o class object
		\param[in] f getter callback
		\param[in] g invoked callback
	 */
	template<
		typename _DelegatingClass,
		typename _InvokedClass
	>
	inline void addKeyDownCallback(
		int key, 
		_DelegatingClass * o,  
		_InvokedClass * (_DelegatingClass::*f)(),
		void (_InvokedClass::*g)(const sad::Event &)
	)
	{ this->addKeyDownCallback(key, new fsm::Callback(o, f, g)); }
	/*! Removes a key up callback
		\param[in] key key data
	 */
	void removeKeyUpCallback(int key);
	/*! Removes a key down callback
		\param[in] key key data
	 */
	void removeKeyDownCallback(int key);

	/*! Invokes a callback with eventype if any
		\param[in] eventType type of event
	 */
	virtual void invoke(const sad::String & eventType);
	/*! Invokes a callback with eventype if any
		\param[in] eventType type of event
		\param[in] o real event
	 */
	virtual void invoke(const sad::String & eventType, const sad::Event & o);
	virtual ~State();

	template<typename T> inline T & as() { return static_cast<T&>(*this); }
	template<typename T> inline const T & as() const{ return static_cast<const T&>(*this); }
};

class Machine;

/*! A special callback for event forwarding for data
 */ 
class MachineEventCallback: public sad::EventHandler
{
private:
	fsm::Machine * m_machine;
	sad::String m_event_type;
public:
	/*! Creates a new callback
		\param[in] machine machine data
		\param[in] eventType type of event
	 */
	inline MachineEventCallback(fsm::Machine * machine, sad::String eventType)
	: m_machine(machine), m_event_type(eventType)
	{
	}
	/*! Invokes a functor with event
	    \param[in] o event
	 */
	virtual void operator()(const sad::Event & o);
	/*! Is it is empty
	 */
	virtual bool empty();
};


/*! A machine data information
 */
class Machine
{
public:
	/*! Initial state machine name
	 */
	static const sad::String INITIAL;
private:
	typedef sad::Hash<sad::String, fsm::State *> StateList;
	/*! Callbacks for hashes data
	 */
	StateList m_states;
	/*! A shared data for state machine
	 */
	fsm::Shared * m_shared;
	/*! A current state name
	 */ 
	sad::String m_current_state_name;
	/*! A previous state name
	 */
	sad::String m_previous_state_name;

	/*! Called on entering to a new state
	 */
	void enterCurrentState();
	/*! Called on leaving a previous state
	 */
	void leavePreviousState();
public:
	Machine();

	/*! Returns a current state name. A fsm::Machine::INITIAL at start
		\return a current name of state
	 */
	inline const sad::String & currentStateName() const { return m_current_state_name;}
	/*! At start both of previous and current state are set to fsm::Machine::INITIAL
		\return previous name of state
	 */
	inline const sad::String & previousStateName()  { return m_previous_state_name;}
	/*! Returns a shared data of state machine 
		\return a shared data of machine
	 */
	inline fsm::Shared * sharedData() { return m_shared; }
	/*! A shared data
		\param[in] data data items
	 */
	void setSharedData(fsm::Shared * data);
	/*! A current state data. Null if not found
		\return a current state
	 */
	fsm::State * currentState();

	/*! Adding a state
		\param[in] name name of state
		\param[in] state state of object
	 */
	void addState(const sad::String & name, fsm::State * state);
	/*! A removes a state for data
		\param[in] name state to remove
	 */
	void removeState(const sad::String & name);
	/*! An event name data
		\param[in] eventName event name
		\param[in] o         a linked event data
	 */
	void invokeEvent(const sad::String & eventName, const sad::Event & o);
	/*! Pushes a new state for data
		\param[in] newState a state for new data
	 */
	void pushState(const sad::String & newState);
	/*! Returns a machine to an old state
	 */
	void cancelState();
	/*! Creates a new callback for event type
		\param[in] type event type
	 */
	fsm::MachineEventCallback * callbackFor(const sad::String & type);
	/*! Binds linking callbacks to a controls points, so events will be propagated to machine
		\param[in] controls controls data
	 */
	void addCallbacks(sad::Input * controls = sad::Input::ref());
	~Machine();
};


}

}
