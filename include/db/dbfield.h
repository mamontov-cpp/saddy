/*! \file   dbfield.h
	\author HiddenSeeker
	
	Describes a field of class, as property
 */
#pragma once
#include "dbproperty.h"
#include <cassert>

namespace sad
{

namespace db
{

/*! Describes a mapped field for a class
 */
template<
	typename _Object,
	typename _FieldTypeName
>
class Field: public sad::db::Property
{
public:
	/*! Creates new field for a class
		\param[in] o a field data
	 */
	Field(_FieldTypeName (_Object::*f)) : m_f(f)
	{
		
	}
	/*! A field data
	 */
	virtual ~Field()
	{

	}
	/*! Sets a value for a property
		\param[in] v a variant for property
		\return whether value is set successfully
	 */
	virtual bool setValue(const sad::db::Variant & v)
	{
		assert( m_o );
		sad::Maybe<_FieldTypeName> value = v.get<_FieldTypeName>();
		bool result = false;
		if (value.exists())
		{
			(reinterpret_cast<_Object*>(m_o)->*m_f)  = value.value();
			result = true;
		}
		return result;
	}
	/*! Gets a value for a property
		\return a value for a property
	 */
	virtual const sad::db::Variant & getValue() const
	{
		assert( m_o );
		const_cast<sad::db::Variant&>(m_tmp).set( (reinterpret_cast<_Object*>(m_o)->*m_f) );
		return m_tmp;
	}
	/*! Checks, whether value has property type in key field
		\param[in] key a key of field to check
		\param[in] v value
		\return whether field has following type
	 */
	virtual bool check(const sad::String& key, const picojson::value& v)
	{
		bool result = false;
		if (v.is<picojson::object>())
		{
			picojson::value data = v.get(key);
			if (data.is<picojson::null>() == false) 
			{
				result = picojson::ValueToType<_FieldTypeName>().get(data).exists();
			}
		}
		return result;
	}

protected:
	/*! A temporary variant for returning property from field
	 */
	sad::db::Variant m_tmp;
	/*! A field of class
	 */
	_FieldTypeName (_Object::*m_f);
};

/*! Returns a new field by valye
	\param[in] f object
	\return field
 */
template<
	typename _Object,
	typename _FieldTypeName
>
Field<_Object, _FieldTypeName> * define_field(_FieldTypeName (_Object::*f))
{
	return new Field<_Object, _FieldTypeName>(f);
}

}

}
