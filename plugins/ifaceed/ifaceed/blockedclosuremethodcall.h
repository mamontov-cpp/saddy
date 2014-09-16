/*!  \file blockedclosuremethodcall.h
 *   \author HiddenSeeker
 *
 *   Defines a simple wrapper for creating a closure for method call, which also blocks widget's signals before call
 */
#pragma once
#include <closure.h>

/*! \class BlockedClosureMethodCall1
 *
 * A class for closure method call with one argument
 */
template<
    typename _ClassName,
    typename _Method,
    typename _Arg
>
class BlockedClosureMethodCall1: public sad::ClosureBasic
{
public:
    /*!
     * Constructs new instance of method call
     * \param o object
     * \param f method
     */
    inline BlockedClosureMethodCall1(_ClassName * o, _Method f, const _Arg & a) : m_o(o), m_f(f), m_a(a)
    {

    }
    /*!
     * Can be inherited
     */
    virtual ~BlockedClosureMethodCall1()
    {

    }
    /*!
     * Invokes method
     */
    virtual void run()
    {
        bool b = m_o->blockSignals(true);
        (m_o->*m_f)(m_a);
        m_o->blockSignals(b);
    }
protected:
    /*!
     * An object for method invocation
     */
    _ClassName* m_o;
    /*!
     * A method to be called on object
     */
    _Method m_f;
    /*!
     * An argument to be passed into method call
     */
    _Arg m_a;
};

/*!
 * Creates new call
 * \param[in] o object
 * \param[in] f method
 * \param[in] a argument
 */
template<
  typename _ClassName,
  typename _Method,
  typename _Arg
>
sad::ClosureBasic* blocked_bind(_ClassName* o, _Method f, const _Arg& a)
{
    return new BlockedClosureMethodCall1<_ClassName, _Method, _Arg>(o, f, a);
}

