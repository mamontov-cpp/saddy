/*! \file weight.h
	\author HiddenSeeker

	Describes a weight of object
 */
#pragma once

namespace p2d
{
/*! Describes a weight of object
 */
class Weight
{
protected:
	bool m_finite;  //!< Is weight of object is finite
	double m_value; //!< Value of weight
public:
	/*! Creates new weight
		\param[in] value weight value
		\param[in] finite
	 */
	Weight(double value, bool finite = true);
	/*! Determines, whether weight is infinite
		\return true if infinite
	 */
	virtual bool isInfinite() const;
	/*! Returns weight value
		\return true value
	 */
	virtual double value() const;
	/*! Creates new constant weight
		\param[in] value weight 
		\return created weight
	 */
	static Weight constant(double value);
	/*! Returns infinite weight
		\return created weight
	 */
	static Weight infinite();
};


}
