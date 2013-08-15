/*! \file findcontactpoints.h
	\author HiddenSeeker

	Contains a multimethod, which can be used to determine a contact points
	for shapes
 */
#include "collisionmultimethod.h"
#include "infiniteline.h"
#include "point.h"
#include "rectangle.h"
#include "circle.h"
#include "line.h"
#pragma once

namespace p2d
{

typedef hst::vector<p2d::PointsPair> SetOfPointsPair;

/*! Finds a contact point set between two shapes
 */
class FindContactPoints: 
public p2d::CollisionMultiMethodWithArg<p2d::SetOfPointsPair, p2d::Vector>
{
 private:
	 /*! Find a contact points for two convex hulls, moving with
		 speeds v1 and v2
		 \param[in] c1 first convex hull
		 \param[in] v1 speed of first convex hull
		 \param[in] c2 second convex hull
		 \param[in] v2 speed of second convex hull
		 \return set of pairs if any exists
	  */
	 static p2d::SetOfPointsPair invoke(
		 const p2d::ConvexHull & c1, 
		 const p2d::Vector & v1,
		 const p2d::ConvexHull & c2,
		 const p2d::Vector & v2
	 );
	 /*! Find a contact points for circle and convex hull, moving with
		 speeds v1 and v2
		 \param[in] c1 circle
		 \param[in] v1 speed of circle
		 \param[in] c2 convex hull
		 \param[in] v2 speed of convex hull
		 \return set of pairs if any exists
	  */
	 static p2d::SetOfPointsPair invoke(
		 const p2d::Circle * c1, 
		 const p2d::Vector & v1,
		 const p2d::ConvexHull & c2,
		 const p2d::Vector & v2
	 );
	 /*! Find a contact points for two circles, moving with
		 speeds v1 and v2
		 \param[in] c1 first circle
		 \param[in] v1 speed of first circle
		 \param[in] c2 second circle
		 \param[in] v2 speed of second circle
		 \return set of pairs if any exists
	  */
	 static p2d::SetOfPointsPair invoke(
		 const p2d::Circle * c1, 
		 const p2d::Vector & v1,
		 const p2d::Circle * c2,
		 const p2d::Vector & v2
	 );

	 static p2d::SetOfPointsPair getRtoR(
		 p2d::Rectangle * s1, 
		 const p2d::Vector & v1,
		 p2d::Rectangle * s2,
		 const p2d::Vector & v2
	 );
	 static p2d::SetOfPointsPair getRtoL(
		 p2d::Rectangle * s1, 
		 const p2d::Vector & v1,
		 p2d::Line * s2,
		 const p2d::Vector & v2
	 );
	 static p2d::SetOfPointsPair getCtoR(
		 p2d::Circle * s1, 
		 const p2d::Vector & v1,
		 p2d::Rectangle * s2,
		 const p2d::Vector & v2
	 );
	 static p2d::SetOfPointsPair getCtoC(
		 p2d::Circle * s1, 
		 const p2d::Vector & v1,
		 p2d::Circle * s2,
		 const p2d::Vector & v2
	 );
	 static p2d::SetOfPointsPair getCtoL(
		 p2d::Circle * s1, 
		 const p2d::Vector & v1,
		 p2d::Line * s2,
		 const p2d::Vector & v2
	 );
	 static p2d::SetOfPointsPair getLtoL(
		 p2d::Line * s1, 
		 const p2d::Vector & v1,
		 p2d::Line * s2,
		 const p2d::Vector & v2
	 );
    protected:
		/*! Reverses a set of points if neeeded
			\param[in, out] set
		 */
		virtual void reverse(p2d::SetOfPointsPair & pairs);
	    /*! Inits a callbacks in set
			\param[in] initted set
		 */
		virtual void init();
};

/*! Computes intersection for normal, emitted from p, to a line, determined
	by cutter
	\param[in] p point, where normal is emitted
	\param[in] c cutter
	\return intersection
 */
p2d::Point intersectionWithNormalFrom(
	const p2d::Point & p,
	const p2d::Cutter2D & c
);

/*! Computes an intersection points of line and circle
	\param[in] l line
	\param[in] c circle
	\return intersection points
 */
hst::vector<p2d::Point> intersection(
	const p2d::InfiniteLine & l,
	const Circle * c
); 
/*! Computes contact points for cutter c, moving with speed v towards 
	circle ci
	\param[in] c cutter
	\param[in] v vector
	\param[in] ci circle
 */
p2d::SetOfPointsPair findContacts(
	const p2d::Cutter2D & c,
	const p2d::Vector & v,
	const Circle * ci
);

}
