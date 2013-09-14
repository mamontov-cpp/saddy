/*! \file startscreenrain.h
	\author HiddenSeeker

	Describes a rain in start screen
 */
#include <input.h>
#include <extra/periodicalevent.h>
#pragma once

class Game;

/*! Descibes a rain of objects in start screen
 */ 
class StartScreenRain: public PeriodicalEvent
{
 protected:
	 Game * m_game; //!< A game which rain is linked to

	 /*! Adds a new objects
	  */
	 virtual void perform();
 public:
	 /*! Creates new rain, linked to specified game
	  */
	 StartScreenRain(Game * g);
};


