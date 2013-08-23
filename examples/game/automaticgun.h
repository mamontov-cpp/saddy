/*! \file automaticgun.h
	\author HiddenSeeker

	Describes an automatic gun, that shoots to enemy or specified 
	direction by emitting some bullet with specified speed
 */
#include <time.h>
#include <extra/periodicalevent.h>
#include "gameobjectconstants.h"
#pragma once

class GameObject;
/*! A basic automatic gun can shoot. It's knows about position of object
	and tries to shoot by a bullet every period of time
 */
class AbstractAutomaticGun: public PeriodicalEvent
{
  protected:
	  /*! A game object, which has a gun
	   */
	  GameObject * m_object;
	  /*! A delta between game object angle and automatic gun's angle.
		  For most parts it's need to be M_PI because sprites are facing right,
		  and cos of zero will face bullets into opposite direction
	   */
	  double m_dangle;
  public:
	  /*! As a default creates a enabled gun with M_PI, interval is 50 milliseconds
	   */
	  AbstractAutomaticGun();
	  /*! Tries to shoot as fast as it can
	   */
	  void tryShoot();
      /*! Sets difference between angle of object and shooting angle
		  \param[in] delta delta of angle
	   */
	  void setAngleDifference(double delta);
	  /*! Sets a game object
		  \param[in] o game object
	   */
	  void setObject(GameObject * o);
	  virtual ~AbstractAutomaticGun();
};

/*! An automatic gun, which shoots bullets of specified type
 */
template<
	typename _Bullet
>
class AutomaticGun: public AbstractAutomaticGun
{
 protected:
	 /*! Shoots bullets of specified bullet type
	  */
	 virtual void perform()
	 {
		 // Check paused flag
		 if (m_object->game()->isPaused() == false)
		 {
		  _Bullet * bullet = new _Bullet();
		  m_object->game()->addObject(bullet);
		  p2d::Point p = m_object->position();
		  double angle = m_object->angle() + m_dangle;
		  bullet->setPosition(p);
		  bullet->setAngularVelocity(1.0);
		  double speed = GameObjectConstants<_Bullet>::velocity();
		  bullet->setHorizontalSpeed(speed * cos(angle));
		  bullet->setVerticalSpeed(speed * sin(angle));
		 }
	 }
 public:
	 /*! Sets interval from game constants
	  */
	 AutomaticGun() : AbstractAutomaticGun()
	 {
		 setInterval(GameObjectConstants<_Bullet>::interval());
	 }
};
