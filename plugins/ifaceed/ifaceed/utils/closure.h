/*! \file closure.h
	\author HiddenSeeker

	Describes a closure-like thing in terms of C++
 */
#include <QMetaType>
#pragma once
/*! Basic class for all of closures. Need to put this stuff for template data
 */
class ClosureBasic
{
 public:
    /*! Executes a closure
     */
    virtual void run()=0;
	/*! Destroys a closure
	 */
    virtual ~ClosureBasic();
};


/*! Macro, that starts a closure definition
 */
#define       CLOSURE      {                                      \
                                class ____: public ClosureBasic   \
                                {                                 \
                                 public:                          \
                                         ClosureBasic  * ___()    \
                                         {                        \
                                          return new ____(*this); \
                                         }
/*! Macro for declaration of closure data
 */
#define      CLOSURE_DATA(X)              X
/*! Macro for declaration of closure code
 */
#define      CLOSURE_CODE(X)              virtual void run()  { X }
/*! Macro, that constructs a closure and inits it with data
 */
#define      INITCLOSURE(X)          } ______; X;
/*! Submits a closure to an executor
 */
#define SUBMITCLOSURE( X )   X (______.___() ); } 
/*! Macro for initialization of closure value for data
 */
#define CLSET(PROP,VAL) ______. PROP = VAL;

Q_DECLARE_METATYPE(ClosureBasic*);
