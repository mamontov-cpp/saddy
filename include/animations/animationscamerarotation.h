/*! \file animationscamerarotation.h
	\author HiddenSeeker

	Describes an animation for editing a camera rotation
 */
#pragma once
#include "animationsanimation.h"

#include "../sadpoint.h"

namespace sad
{
	
namespace animations
{

/*! An animation of rotating the camera
 */
class CameraRotation: public sad::animations::Animation
{
SAD_OBJECT
public:
	/*! Creates new empty animation
	 */
	CameraRotation();
	/*! Can be inherited
	 */
	virtual ~CameraRotation();
    /*! Tries to load animation from value
        \param[in] v value
        \return whether it was successfull
     */
    virtual bool loadFromValue(const picojson::value& v);
	/*! Set minimal angle for camera rotation
		\param[in] angle an angle for rotation
	 */
	void setMinAngle(double angle);
	/*! Returns minimal angle
		\return minimal angle for rotation
	 */
	double minAngle() const;
	/*! Set maximal angle for camera rotation
		\param[in] angle an angle for rotation
	 */
	void setMaxAngle(double angle);
	/*! Returns maximal angle
		\return maximal angle for rotation
	 */
	double maxAngle() const;
	/*! Sets a pivot point for rotation
		\param[in] p pivot
	 */
	void setPivot(const sad::Point3D& p);
	/*! Returns a pivot point for rotation
		\return pivot point
	 */
	const sad::Point3D& pivot() const;
	/*! Sets state of object from animation
        \param[in] o object
        \param[in] time a time of playing of animation
     */
    virtual void setState(sad::db::Object* o, double time);
    /*! Saves states of object in animation instance
        \param[in] i an animation instance
        \return whether we can work further with this object in instance
     */
    virtual bool saveState(sad::animations::Instance* i);
    /*! Resets state of object in animation instance, when animation ended
        \param[in] i an animation instance
     */
    virtual void resetState(sad::animations::Instance* i);
protected:
	/*! A minimal angle for editing
	 */
	double m_min_angle;
	/*! A maximal angle for editing
	 */
	double m_max_angle;
	/*! A pivot point for rotation
	 */
	sad::Point3D m_pivot;
};

}

}

