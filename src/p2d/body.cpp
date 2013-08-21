#include "p2d/body.h"
#include "p2d/world.h"
#include "p2d/circle.h"
#include "p2d/line.h"

DECLARE_SOBJ(p2d::Body);


double p2d::Body::timeStep() const
{
	if (m_world == NULL)
		return 1.0;
	return m_world->timeStep();
}

void p2d::Body::notifyRotate(const double & delta)
{
	m_current->rotate(delta);
}

void p2d::Body::notifyMove(const p2d::Vector & delta)
{
	m_current->move(delta);
}

void p2d::Body::setUserObject(sad::Object * o)
{
	m_user_object = o;
}


sad::Object * p2d::Body::userObject() const
{
	return m_user_object;
}

const hst::string & p2d::Body::userType() const
{
	if (m_user_object == NULL)
	{
		return this->metaData()->name();
	}
	return m_user_object->metaData()->name();
}

p2d::CollisionShape & p2d::Body::at(double time) const
{
	p2d::Body * me = const_cast<p2d::Body *>(this);
	delete me->m_temporary;
	me->m_temporary = me->m_current->clone();
	me->m_temporary->move(m_tangential->positionDelta(time, me->timeStep()));
	me->m_temporary->rotate(m_angular->positionDelta(time, me->timeStep()));
	return *(me->m_temporary);
}

void p2d::Body::stepDiscreteChangingValues()
{
	m_ghost->step();
	m_tangential->force()->step();
	m_angular->force()->step();
}

void p2d::Body::stepPositionsAndVelocities(double time)
{
	m_tangential->step(time, this->timeStep());
	m_angular->step(time, this->timeStep());
}

void p2d::Body::trySetTransformer()
{
	if (m_world)
	{
		if (this->m_current->metaData()->canBeCastedTo("p2d::Circle"))
		{
			p2d::Circle * c = hst::checked_cast<p2d::Circle>(m_current);
			c->setTransformer(m_world->transformer());
		}
	}
}

p2d::Body::Body()
{
	m_weight = new p2d::Weight();
	m_ghost = new p2d::GhostOptionsFlow();
	m_world = NULL;
	m_user_object = NULL;
	
	m_tangential = new p2d::TangentialMovement();
	m_tangential->addListener( new move_t(this, &p2d::Body::notifyMove) );

	m_angular = new p2d::AngularMovement();
	m_angular->addListener( new rotate_t(this, &p2d::Body::notifyRotate) );

	p2d::Line * l = new p2d::Line();
	l->setCutter(p2d::cutter(0,0,0,0));
	m_current =  l;

	m_temporary = NULL;
}

p2d::Body::~Body()
{
	delete m_weight;
	delete m_tangential;
	delete m_angular;
	delete m_current;
	delete m_temporary;
}

void p2d::Body::setWeight(p2d::Weight * weight)
{
	delete m_weight;
	m_weight = weight;
}

void p2d::Body::setWeight(const p2d::Weight & weight)
{
	*m_weight = weight;
}

const p2d::Weight & p2d::Body::weight() const
{
	return *m_weight;
}

void p2d::Body::setCurrentGO(p2d::GhostOptions * ghost)
{
	m_ghost->setCurrent(ghost);
}

void p2d::Body::sheduleGO(p2d::GhostOptions *next)
{
	m_ghost->push(next);	
}

bool p2d::Body::isGhost() const
{
	return m_ghost->value();
}

void p2d::Body::setWorld(p2d::World * world)
{
	m_world = world;
	this->trySetTransformer();
}

p2d::World * p2d::Body::world()
{
	return m_world;
}

void p2d::Body::setShape(p2d::CollisionShape * shape)
{
	delete m_current;
	m_current = shape;
	this->trySetTransformer();
	m_current->move(this->m_tangential->position());
	m_current->rotate(this->m_angular->position());
}


void p2d::Body::setCurrentPosition(const p2d::Point & p)
{
	m_tangential->setCurrentPosition(p);
}

void p2d::Body::shedulePosition(const p2d::Point & p)
{
	m_tangential->setNextPosition(p);
}


const p2d::Vector & p2d::Body::position() const
{
	return m_tangential->position();
}

bool p2d::Body::willPositionChange() const
{
	return m_tangential->willPositionChange();
}

p2d::Vector p2d::Body::nextPosition() const
{
	return m_tangential->nextPosition();
}

void p2d::Body::setCurrentTangentialVelocity(const p2d::Vector & v)
{
	m_tangential->setCurrentVelocity(v);
}

void p2d::Body::sheduleTangentialVelocity(const p2d::Vector & v)
{
	m_tangential->setNextVelocity(v);
}

const p2d::Vector & p2d::Body::tangentialVelocity() const
{
	return m_tangential->velocity();
}

bool p2d::Body::willTangentialVelocityChange() const
{
	return m_tangential->willVelocityChange();
}

p2d::Vector p2d::Body::nextTangentialVelocity() const
{
	return m_tangential->nextVelocity();
}

void p2d::Body::setCurrentAngle(double angle)
{
	m_angular->setCurrentPosition(angle);
}

void p2d::Body::sheduleAngle(double angle)
{
	m_angular->setNextPosition(angle);	
}

double p2d::Body::angle() const
{
	return m_angular->position();
}

bool p2d::Body::willAngleChange() const
{
	return m_angular->willPositionChange();
}

double p2d::Body::nextAngle() const
{
	return m_angular->nextPosition();
}

void p2d::Body::setCurrentAngularVelocity(double v)
{
	m_angular->setCurrentVelocity(v);
}

void p2d::Body::sheduleAngularVelocity(double v)
{
	m_angular->setNextVelocity(v);
}

double p2d::Body::angularVelocity() const
{
	return m_angular->velocity();
}

bool p2d::Body::willAngularVelocityChange() const
{
	return m_angular->willVelocityChange();
}

double p2d::Body::nextAngularVelocity() const
{
	return m_angular->nextVelocity();
}

void p2d::Body::move(const p2d::Vector & v)
{
	return m_tangential->setCurrentPosition(m_tangential->position() + v);
}


void p2d::Body::rotate(double delta)
{
	return m_angular->setCurrentPosition(m_angular->position() + delta);
}

