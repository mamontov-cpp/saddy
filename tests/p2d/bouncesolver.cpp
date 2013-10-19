#pragma warning(push)
#pragma warning(disable: 4273)
#pragma warning(disable: 4351)
#include <stdio.h>
#include "3rdparty/tpunit++/tpunit++.hpp"
#include <p2d/bouncesolver.h>
#include <p2d/walls.h>
#include <p2d/body.h>
#include <p2d/circle.h>
#include <p2d/world.h>
#include <object.h>
#pragma warning(pop)


class Ball: public sad::Object
{
	SAD_OBJECT;
};

DECLARE_SOBJ(Ball);

/*!
 * Tests collides1d operation
 */
struct BounceSolverTest : tpunit::TestFixture
{
 public:
   BounceSolverTest() : tpunit::TestFixture(
	   TEST(BounceSolverTest::testBug1)
   ) {}
   
   sad::p2d::BounceSolver * m_solver;

   void onWallBall(const sad::p2d::CollisionEvent<Ball, sad::p2d::Wall> & ev)
   {
	   m_solver->pushRotationFriction(0.0001, 1);
	   m_solver->bounce(ev.sad::p2d::BasicCollisionEvent::m_object_1, ev.m_object_2->body());
   }

   /**
    * This test-case reproduces bug of 17.09.13, when ball left bounds and continued
	* to drown beyond bounds of screen
    */
   void testBug1()
   {
		// Create ball
	    Ball * ball = new Ball(); 
		sad::p2d::Body * ballbody = new sad::p2d::Body();
		
		sad::p2d::Circle * circle = new sad::p2d::Circle();
		circle->setRadius(11);
		circle->setCenter(sad::p2d::Vector(0, 0));
		ballbody->setShape(circle);
		
		ballbody->setUserObject(ball);
		ballbody->tangentialForces().add( 
			new sad::p2d::TangentialForce(sad::p2d::Vector(0, -30) ) 
		);
		ballbody->setCurrentTangentialVelocity(sad::p2d::Vector(-195.131, -269.817));
		ballbody->setCurrentAngularVelocity(1.0);
		ballbody->setCurrentPosition(sad::p2d::Vector(14.6407, 14.0724));

		// Create walls
		sad::p2d::Walls * walls = new sad::p2d::Walls(800, 600);

		// Create world
		sad::p2d::World * world = new sad::p2d::World();
		world->add(ballbody);
		for(unsigned int i = 0; i < walls->bodies().size(); i++) 
		{
			world->add(walls->bodies()[i]);
		}
		
		// Create solver
		m_solver = new sad::p2d::BounceSolver();

		// FPS was 106.58, so timestep must be like this
		double timestep = 1.0 / 106.58;
		world->step(timestep);

		delete world;
		delete m_solver;
		delete walls;
		delete ball;
   }


} _test_angle_of;
