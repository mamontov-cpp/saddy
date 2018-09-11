#include "actor.h"
#include "../game.h"

#include <cstdio>

#include <p2d/world.h>

// ============================================== PUBLIC METHODS ==============================================

void game::Actor::Actor() : m_game(NULL), 
m_options(NULL), 
m_walking_animation(NULL), 
m_walking_instance(NULL), 
m_jumping_animation(NULL), 
m_jumping_instance(NULL),
m_own_horizontal_velocity(0), 
m_sprite(NULL), 
m_body(NULL), 
m_is_resting(false),
m_is_ducking(false),
m_is_free_fall(false),
m_is_walking_animation_playing(false),
m_is_jumping_animation_playing(false),
m_resting_platform(NULL),
m_fixed_x(false),
m_fixed_y(false),
m_game(NULL)
{

}

void game::Actor::~Actor()
{
    if (m_options)
    {
        m_options->delRef();
    }

    if (m_walking_instance)
    {
        m_walking_instance->delRef();
    }
    if (m_walking_animation)
    {
        m_walking_animation->delRef();
    }
 
    if (m_jumping_animation)
    {
        m_jumping_animation->delRef();
    }
    
    if (m_jumping_instance)
    {
        m_jumping_instance->delRef();
    }
}

bool game::Actor::setOptions(game::ActorOptions* opts)
{
    if (m_options)
    {
        return false;
    }
    
    if (!opts)
    {
        reurn false;
    }
    opts->validate();
    opts->addRef();
    m_options = opts;

    m_walking_animation = new sad::animations::OptionList();
    m_walking_animation->setList(opts->WalkingAnimationOptions);
    m_walking_animation->setTime(opts->WalkingAnimationTime);
    m_walking_animation->setLooped(true);
    m_walking_animation->addRef();

    m_walking_instance = new sad::animations::Instance();
    m_walking_instance->setAnimation(m_walking_animation);
    m_walking_instance->addRef();

    m_jumping_animation = new sad::animations::OptionList();
    m_jumping_animation->setList(opts->JumpingAnimationOptions);
    m_jumping_animation->setTime(opts->JumpingAnimationTime);
    m_jumping_animation->setLooped(false);
    m_jumping_animation->addRef();

    m_jumping_instance = new sad::animations::Instance();
    m_jumping_instance->setAnimation(m_jumping_animation);
    m_jumping_instance->end([this]() { this->m_is_jumping_animation_playing = false; }); 
    m_jumping_instance->addRef();

    return true;
}

void game::Actor::tryStartGoingUp()
{
    if (m_options)
    {
        if (m_options->IsFloater)
        {
            // TODO: Implement floater
        }
        else
        {
            this->tryJump();
        }
    }
}

void game::Actor::tryStopGoingUp()
{
    if (m_options)
    {
        if (m_options->IsFloater)
        {
            // TODO: Implement floater
        }
    }
}

void game::Actor::tryStartGoingDown()
{
    if (m_options)
    {
        if (m_options->IsFloater)
        {
            // TODO: Implement floater
        }
        else
        {
            this->startFallingOrDuck();
        }
    }
}

void game::Actor::tryStopGoingDown()
{
    if (m_options)
    {
        if (m_options->IsFloater)
        {
            // TODO: Implement floater
        }
        else
        {
            this->stopFallingOrStopDucking();
        }
    }
}

void game::Actor::tryStartGoingLeft()
{
    if (m_options)
    {
        if (m_options->IsFloater)
        {
            // TODO: Implement floater
        }
        else
        {
            this->startMovingLeft();
        }
    }
}

void game::Actor::tryStopGoingLeft()
{
    if (m_options)
    {
        if (m_options->IsFloater)
        {
            // TODO: Implement floater
        }
        else
        {
            this->stopMovingHorizontally();
        }
    }
}


void game::Actor::tryStartGoingRight()
{
    if (m_options)
    {
        if (m_options->IsFloater)
        {
            // TODO: Implement floater
        }
        else
        {
            this->startMovingRight();
        }
    }
}

void game::Actor::tryStopGoingRight()
{
    if (m_options)
    {
        if (m_options->IsFloater)
        {
            // TODO: Implement floater
        }
        else
        {
            this->stopMovingHorizontally();
        }
    }
}

void game::Actor::onPlatformCollision(const sad::p2d::BasicCollisionEvent & ev)
{
    printf("Event\n");
    double tick = m_game->physicsWorld()->timeStep();
    double precision_collision = 0.1; // A correction to ensure, that TOI won't be negative

    sad::p2d::Vector force_value;
    ev.m_object_1->tangentialForces().value(force_value);

    bool willVelocityChange = ev.m_object_1->willTangentialVelocityChange();
    sad::p2d::Vector v = ev.m_object_2->tangentialVelocity();
    sad::p2d::Vector player_velocity = ev.m_object_1->tangentialVelocity();
    sad::p2d::Vector next_velocity;
    if (willVelocityChange)
    {
        next_velocity = ev.m_object_1->nextTangentialVelocity();
    }
    sad::Point2D current_position_1 = ev.m_object_1->position();
    sad::Point2D current_position_2 = ev.m_object_2->position();

    sad::p2d::BounceSolver* bounce_solver = m_game->bounceSolver();
    bounce_solver->pushResilienceCoefficient(0.0);
    bounce_solver->pushRotationFriction(0.0);
    if (!bounce_solver->bounce(ev.m_object_1, ev.m_object_2))
    {
        ev.m_object_1->setCurrentTangentialVelocity(this->m_player->oldVelocity());
        bool bounced = bounce_solver->bounce(ev.m_object_1, ev.m_object_2);
        ev.m_object_1->setCurrentTangentialVelocity(player_velocity);
        if (!bounced)
        {
            return;
        }
    }
    double ctoi = bounce_solver->correctedTOI();
    sad::Point2D next_position_1 = ev.m_object_1->nextPosition();
    sad::Point2D next_position_2 = ev.m_object_2->nextPosition();

    sad::Rect2D shape_1 = dynamic_cast<sad::p2d::Rectangle*>(ev.m_object_1->currentShape())->rect();
    sad::Rect2D shape_2 = dynamic_cast<sad::p2d::Rectangle*>(ev.m_object_2->currentShape())->rect();

    sad::moveBy(next_position_1 - current_position_1, shape_1);
    sad::moveBy(next_position_2 - current_position_2, shape_2);

    double min_player_y = std::min(shape_1[0].y(), shape_1[2].y());
    double max_platform_y = std::max(shape_2[0].y(), shape_2[2].y());

    sad::p2d::Cutter1D player_part(std::min(shape_1[0].x(), shape_1[2].x()), std::max(shape_1[0].x(), shape_1[2].x()));
    sad::p2d::Cutter1D platform_part(std::min(shape_2[0].x(), shape_2[2].x()), std::max(shape_2[0].x(), shape_2[2].x()));

    if ((sad::is_fuzzy_equal(max_platform_y, min_player_y) || (min_player_y > max_platform_y))
        && (sad::p2d::collides(player_part, platform_part)))
    {
        if (ev.m_object_1->willPositionChange())
        {
            double x = ev.m_object_1->nextPosition().x();
            double y = ev.m_object_1->position().y() + player_velocity.y() * (ctoi * 0.99) + precision_collision;
            if (!(this->isXCoordinateFixed()))
            {
                x = ev.m_object_1->position().x() + player_velocity.x() * tick + force_value.x() * tick * tick / 2.0;
            }
            if (ev.m_object_2->tangentialVelocity().y() > 0)
            {
                y += ev.m_object_2->tangentialVelocity().y() * tick;
            }
            ev.m_object_1->shedulePosition(sad::Point2D(x, y));
            this->setYCoordinateFixed(true);

        }
        else
        {
            double x = ev.m_object_1->position().x() + player_velocity.x() * tick + force_value.x() * tick * tick / 2.0;
            double y = ev.m_object_1->position().y() + player_velocity.y() * (ctoi * 0.99) + precision_collision;
            if (ev.m_object_2->tangentialVelocity().y() > 0)
            {
                y += ev.m_object_2->tangentialVelocity().y() * tick;
            }
            ev.m_object_1->shedulePosition(sad::Point2D(x, y));
            this->setYCoordinateFixed(true);
        }
        this->restOnPlatform(ev.m_object_2, v);
        this->setYCoordinateFixed(true);
    }
    else
    {
        ev.m_object_1->setCurrentAngularVelocity(0);
        ev.m_object_1->sheduleAngularVelocity(0);

        // Force sliding by offsetting objects after collision
        if (sad::p2d::collides(player_part, platform_part))
        {
            if (ev.m_object_1->willPositionChange())
            {
                double x = ev.m_object_1->nextPosition().x();
                double y = ev.m_object_1->position().y() + player_velocity.y() * (ctoi * 0.99) - precision_collision;
                if (!(this->isXCoordinateFixed()))
                {
                    x = ev.m_object_1->position().x() + player_velocity.x() * tick + force_value.x() * tick * tick / 2.0;
                }
                if (ev.m_object_2->tangentialVelocity().y() < 0)
                {
                    y += ev.m_object_2->tangentialVelocity().y() * tick;
                }
                ev.m_object_1->shedulePosition(sad::Point2D(x, y));
                this->setYCoordinateFixed(true);

            }
            else
            {
                double x = ev.m_object_1->position().x() + player_velocity.x() * tick + force_value.x() * tick * tick / 2.0;
                double y = ev.m_object_1->position().y() + player_velocity.y() * (ctoi * 0.99) - precision_collision;
                if (ev.m_object_2->tangentialVelocity().y() < 0)
                {
                    y += ev.m_object_2->tangentialVelocity().y() * tick;
                }
                ev.m_object_1->shedulePosition(sad::Point2D(x, y));
                this->setYCoordinateFixed(true);
            }
        }
        else
        {
            bool is_front_collision = player_part.p2() < platform_part.p1();
            double correction = (is_front_collision) ? (precision_collision * - 1) : precision_collision;
            if ((is_front_collision && v.x() < 0) || (v.x() > 0))
            {
               correction += v.x() * tick;
            }
            double x = ev.m_object_1->position().x() + player_velocity.x() * (ctoi * 0.99) + correction;
            if (ev.m_object_1->willPositionChange())
            {
                double y = ev.m_object_1->nextPosition().y();
                if (!(this->isYCoordinateFixed()))
                {
                    y = ev.m_object_1->position().y() + player_velocity.y() * tick + force_value.y() * tick * tick / 2.0;
                }
                ev.m_object_1->shedulePosition(sad::Point2D(x, y));
                this->setXCoordinateFixed(true);

            }
            else
            {
                double y = ev.m_object_1->position().y() + player_velocity.y() * tick + force_value.y() * tick * tick / 2.0;
                ev.m_object_1->shedulePosition(sad::Point2D(x, y));
                this->setXCoordinateFixed(true);
            }
        }
        if (!willVelocityChange)
        {
            player_velocity += force_value * ev.m_time;
            ev.m_object_1->sheduleTangentialVelocity(player_velocity);
        }
        else
        {
            ev.m_object_1->sheduleTangentialVelocity(next_velocity);
        }
    }
    ev.m_object_2->setCurrentTangentialVelocity(v);
    ev.m_object_2->setCurrentAngularVelocity(0);
    ev.m_object_2->sheduleTangentialVelocity(v);
    ev.m_object_2->sheduleAngularVelocity(0);
    ev.m_object_2->shedulePosition(ev.m_object_2->position() + v * tick);
}


void game::Actor::setGame(Game* game)
{
    m_game = game;
}

void game::Actor::reset()
{
    m_sprite = NULL;
    m_body = NULL;
    m_is_resting = false;
    m_is_ducking = false;
    m_is_free_fall = false;
    m_resting_platform = NULL;
    m_own_horizontal_velocity = 0;
    m_fixed_x = false;
    m_fixed_y = false;
    m_old_velocity = sad::p2d::Vector(0, 0);
    m_is_walking_animation_playing = false;
    m_is_jumping_animation_playing = false;
}



bool game::Actor::canJump() const
{
    return isResting();
}

bool game::Actor::isResting() const
{
    return m_is_resting;
}


void game::Actor::setSprite(sad::Sprite2D* sprite)
{
    m_sprite = sprite;
}

void game::Actor::setBody(sad::p2d::Body* body)
{
    m_body = body;
}

void game::Actor::setHorizontalVelocity(double value)
{
    if (!m_options)
    {
        return;
    }
    sad::p2d::Body* b = m_body;
    sad::p2d::Vector v = b->tangentialVelocity();
    m_old_velocity = v;
    printf("Old horizontal velocity: %lf, Old own velocity %lf, Setting %lf\n", v.x(), m_own_horizontal_velocity, value);
    if (m_is_resting && !(m_options->IsFloater))
    {
        v.setX(m_resting_platform->tangentialVelocity().x());
    } 
    else
    {
        v.setX(0.0);
    }
    v.setX(v.x() + value);
    printf("New horizontal velocity: %lf\n", v.x());
    b->setCurrentTangentialVelocity(v);
    b->sheduleTangentialVelocity(v);
    
    m_own_horizontal_velocity = value;
    if (m_is_resting && !m_is_ducking)
    {
        if (sad::is_fuzzy_zero(m_own_horizontal_velocity))
        {
            this->cancelWalkingAnimation();
            if (m_options->CanEmitSound)
            {
                m_game->stopWalkingSound();
            }
        }
        else
        {
            this->playWalkingAnimation();
            if (m_options->CanEmitSound)
            {
                m_game->playWalkingSound();
            }
        }
    }
}

void game::Actor::incrementVerticalVelocity(double value) const
{
    if (m_body->willTangentialVelocityChange())
    {
        sad::p2d::Vector v = m_body->nextTangentialVelocity();
        if (!(this->isXCoordinateFixed()))
        {
            v.setX(m_own_horizontal_velocity);
        }        
        m_body->sheduleTangentialVelocity(v + sad::p2d::Vector(0, value));
    }
    else
    {
         sad::p2d::Vector v = m_body->tangentialVelocity();
         if (m_is_resting)
         {
             v.setX(m_own_horizontal_velocity);
             v.setY(0.0);
         }
         m_body->sheduleTangentialVelocity(m_body->tangentialVelocity() + sad::p2d::Vector(0, value));
    }
    printf("Next tangential velocity after increment: %lf, %lf\n", m_body->nextTangentialVelocity().x(), m_body->nextTangentialVelocity().y());
}

void game::Actor::enableGravity() const
{
    if (!m_options)
    {
        return;
    }
    Game::enableGravity(m_body);
}

void game::Actor::disableGravity() const
{
{
    if (!m_options)
    {
        return;
    }
    Game::disableGravity(m_body);
}

void game::Actor::restOnPlatform(sad::p2d::Body* b, const  sad::p2d::Vector& old_velocity)
{
    if (!m_options)
    {
        return;
    }
    if (!b)
    {
        return;
    }
    this->cancelJumpingAnimation();
    this->disableGravity();
    m_is_resting = true;
    m_is_free_fall = false;
    m_is_ducking = false;
    m_resting_platform = b;
    
    double av = 0;
    b->setCurrentTangentialVelocity(old_velocity);
    b->setCurrentAngularVelocity(av);
    b->sheduleTangentialVelocity(old_velocity);
    b->sheduleAngularVelocity(av);

    sad::p2d::Vector own_velocity = old_velocity;
    own_velocity.setX(own_velocity.x() + m_own_horizontal_velocity);

    m_body->setCurrentTangentialVelocity(own_velocity);
    m_body->setCurrentAngularVelocity(av);
    m_body->sheduleTangentialVelocity(own_velocity);
    m_body->sheduleAngularVelocity(av);

    m_old_options.clear();
    if (!sad::is_fuzzy_zero(m_own_horizontal_velocity))
    {
         m_sprite->set(m_options->StandingSprite);
         if (m_options->CanEmitSound)
         {
             m_game->playWalkingSound();
         }
    } 
    else 
    {
        m_sprite->set(m_options->WalkingSprite);
        if (m_options->CanEmitSound)
        {
            m_game->stopWalkingSound();
        }
    }
}

void game::Actor::disableResting()
{
    if (!m_options)
    {
        return;
    }
    this->enableGravity();
    m_is_resting = false;
    m_resting_platform = NULL;
    
    this->cancelWalkingAnimation();
    if (m_options->CanEmitSound)
    {
        m_game->stopWalkingSound();
    }
   this->m_sprite->set(m_options->JumpingSprite);
}


sad::Rect2D game::Actor::area() const
{
    return m_sprite->area();
}

void game::Actor::move(const sad::Point2D& p) const
{
    if (m_body->willPositionChange())
    {
        m_body->shedulePosition(m_body->nextPosition() + p);
    }
    else
    {
        m_body->move(p);
    }
}


void game::Actor::clearFixedFlags()
{
    m_fixed_x = false;
    m_fixed_y = false;
}

bool game::Actor::isXCoordinateFixed() const
{
    return m_fixed_x;
}

bool game::Actor::isYCoordinateFixed() const
{
    return m_fixed_y;
}


void game::Actor::setXCoordinateFixed(bool value)
{
    m_fixed_x = value;
}

void game::Actor::setYCoordinateFixed(bool value)
{
    m_fixed_y = value;
}

sad::p2d::Body* game::Actor::body() const
{
    return m_body;
}

sad::Sprite2D* game::Actor::sprite() const
{
    return m_sprite;
}

void game::Actor::testResting()
{
    if (!m_options)
    {
        return;
    }
    if (m_is_resting)
    {
        sad::Rect2D shape_1 = dynamic_cast<sad::p2d::Rectangle*>(m_body->currentShape())->rect();
        sad::Rect2D shape_2 = dynamic_cast<sad::p2d::Rectangle*>(m_resting_platform->currentShape())->rect();

        sad::p2d::Cutter1D player_part(std::min(shape_1[0].x(), shape_1[2].x()), std::max(shape_1[0].x(), shape_1[2].x()));
        sad::p2d::Cutter1D platform_part(std::min(shape_2[0].x(), shape_2[2].x()), std::max(shape_2[0].x(), shape_2[2].x()));
        if (!sad::p2d::collides(player_part, platform_part))
        {
            this->disableResting();
        } 
        else
        {
            sad::p2d::Vector old_velocity =  m_resting_platform->tangentialVelocity();
            sad::p2d::Vector own_velocity = old_velocity;
            own_velocity.setX(own_velocity.x() + m_own_horizontal_velocity);
            if (this->isYCoordinateFixed())
            {
                own_velocity.setY(m_body->tangentialVelocity().y());
            }
            if (this->isXCoordinateFixed())
            {
                own_velocity.setX(m_body->tangentialVelocity().x());
            }

            m_body->setCurrentTangentialVelocity(own_velocity);
            m_body->sheduleTangentialVelocity(own_velocity);
        }

    }
}


void game::Actor::pushOptions(const sad::String& new_options)
{
    if (new_options != m_sprite->optionsName())
    {
        printf("Is not equal\n");
        m_old_options << m_sprite->optionsName();
    }
    printf("Setting options %s\n", new_options.c_str());
    m_sprite->set(new_options);
}

void game::Actor::popOptions()
{
    if (m_old_options.size())
    {
        printf("Restoring options %s\n", m_old_options[m_old_options.size() - 1].c_str());
        m_sprite->set(m_old_options[m_old_options.size() - 1]);
        m_old_options.removeAt(m_old_options.size() - 1);
    }
}

void game::Actor::startMovingLeft()
{
    if (!m_options)
    {
        return;
    }
    this->startMoving(true, m_options->MaxHorizontalVelocity * -1);
}

void game::Actor::startMovingRight()
{
    if (!m_options)
    {
        return;
    }
    this->startMoving(false, m_options->MaxHorizontalVelocity);
}

void game::Actor::stopMovingHorizontally()
{
    if (!m_options)
    {
        return;
    }
    if (m_is_resting)
    {
        if (!m_is_ducking)
        {
            this->m_sprite->set(m_options->StandingSprite);
        }
    }
    else
    {
        if (!m_is_free_fall)
        {
            this->m_sprite->set(m_options->JumpingSprite);
        }
    }
    this->setHorizontalVelocity(0);
}

void game::Actor::tryJump()
{
    if (!m_options)
    {
        return;
    }
    if (this->canJump()) 
    {
        this->stopFallingOrStopDucking();
        this->incrementVerticalVelocity(m_options->MaxVerticalVelocity);
        this->disableResting();
        this->playJumpingAnimation();
        if (m_options->CanEmitSound)
        {
            m_game->playSound("jump");
        }
    }
}

void game::Actor::startFallingOrDuck()
{
    if (!m_options)
    {
        return;
    }
    if (!m_is_resting) {
        m_is_free_fall = true;
        this->cancelJumpingAnimation();
        this->incrementVerticalVelocity(m_options->MaxVerticalVelocity * -1);
        this->pushOptions(m_options->FallingSprite);
    } else {
        this->duck();
    }
}

void game::Actor::stopFallingOrStopDucking()
{
    if (!m_options)
    {
        return;
    }
    if (!m_is_resting) {
        m_is_free_fall = false;
        this->cancelJumpingAnimation();
        this->incrementVerticalVelocity(m_options->MaxVerticalVelocity);
        this->popOptions();
    } else {
        this->stopDucking();
    }
}

void game::Actor::duck()
{
    if (!m_options)
    {
        return;
    }
    if (!m_is_ducking && m_is_resting)
    {
        m_is_ducking = true;
        this->cancelWalkingAnimation();
        if (m_options->CanEmitSound)
        {
            m_game->stopWalkingSound();
        }
        this->pushOptions(m_options->DuckingSprite);
        correctShape();
    }
}

void game::Actor::stopDucking()
{ 
    if (m_is_ducking && m_is_resting)
    {
        m_is_ducking = false;
        this->popOptions();
        if (!sad::is_fuzzy_zero(m_own_horizontal_velocity))
        {
            this->playWalkingAnimation();
        }
        correctShape();
    }
}

bool game::Actor::isDucking() const
{
    return m_is_ducking;
}

bool game::Actor::isFreefalling() const
{
    return m_is_free_fall;
}

const sad::p2d::Vector& game::Actor::oldVelocity() const
{
    return m_old_velocity;
}

// ===================================== PRIVATE METHODS =====================================

sad::animations::Animations* game::Actor::animations()
{
    return this->m_sprite->scene()->renderer()->animations();
}

void game::Actor::playWalkingAnimation()
{
    if (!m_options)
    {
        return;
    }

    if (!m_is_walking_animation_playing)
    {
        m_walking_instance->clearFinished();
        m_walking_instance->setObject(m_sprite); 
        this->animations()->add(m_walking_instance);
        m_is_walking_animation_playing = true;
    }
}

void game::Actor::cancelWalkingAnimation()
{
    if (!m_options)
    {
        return;
    }

    if (m_is_walking_animation_playing)
    {
        m_walking_instance->cancel(this->animations());
        this->animations()->remove(m_walking_instance);
        m_is_walking_animation_playing = false;
    }
}

void game::Actor::playJumpingAnimation()
{
    if (!m_options)
    {
        return;
    }
    if (!m_is_jumping_animation_playing)
    {
        m_jumping_instance->clearFinished();
        m_jumping_instance->setObject(m_sprite); 
        this->animations()->add(m_jumping_instance);
        m_is_jumping_animation_playing = true;
    }
}

void game::Actor::cancelJumpingAnimation()
{
    if (!m_is_jumping_animation_playing)
    {
        m_jumping_instance->cancel(this->animations());
        this->animations()->remove(m_jumping_instance);
        m_is_jumping_animation_playing = false;
    }
}

void game::Actor::startMoving(bool flip_flag, double velocity)
{
    if (!m_options)
    {
        return;
    }
    m_sprite->setFlipX(flip_flag);
    if (m_is_resting)
    {
        if (!m_is_ducking)
        {
            if (!m_is_walking_animation_playing)
            {
                m_sprite->set(m_options->WalkingSprite);
            }
        }
    }
    this->setHorizontalVelocity(velocity);
}

void game::Actor::correctShape()
{
    sad::p2d::Rectangle*  shape = dynamic_cast<sad::p2d::Rectangle*>(m_body->currentShape());
    sad::Point2D start_point = shape->rect()[0];
    sad::Point2D width_height(this->m_sprite->area().width(), this->m_sprite->area().height());
    sad::Rect2D corrected_rect(start_point, start_point + width_height);
    this->m_sprite->setArea(corrected_rect);
    shape->setRect(corrected_rect);
}

