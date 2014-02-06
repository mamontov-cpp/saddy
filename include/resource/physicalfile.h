/*! \file resource/physicalfile.h
	\author HiddenSeeker
	
	Contains definition of class PhysicalFile.

	This is a physical file, where resources were stored before loading, and where they are belong.
	So you can try to reload it, replacing resources with new ones.
 */
#pragma once
#include "../sadstring.h"
#include "../sadvector.h"
#include "error.h"

namespace sad
{

namespace resource
{
class Tree;
class Resource;
/*! \class PhysicalFile

	This is a physical file, where resources were stored before loading, and where they are belong.
	So you can try to reload it, replacing resources with new ones.
 */
class PhysicalFile  
{	
public:	
	/*! Creates new flle with specified name. Supposedly it must be path to specified file.
		\param[in] name a filename (with or without path) to it
	 */
	PhysicalFile(const sad::String& name = "");	
	/*! This class does not own any of resources, only tree frees a resources
	 */
	virtual ~PhysicalFile();
	/*! Returns true if file is anonymous
		\return whether file is anonymous
	 */
	bool isAnonymous() const;
	/*! Returns name of file (with or without path)
		\return name of file (with or without path)
	 */
	const sad::String & name() const;
	/*! Sets name of file (with or without path)
		\param[in] name name of file (with or without path)
	 */
	void setName(const sad::String& name);
	/*! Reloads all resources from a file
		\return errors if any occured on resources
	 */
	virtual sad::Vector<sad::resource::Error*> reload();
	/*! Adds resource to file
		\param[in] r a resource
	 */
	void add(sad::resource::Resource * r);
	/*! Removes a resource from registered resources of file
		\param[in] r a resource
	 */
	void remove(sad::resource::Resource * r);
	/*! Sets tree for file
		\param[in] tree a tree
	 */
	void setTree(sad::resource::Tree * tree);
	/*! Returns a tree for a file
		\return a tree for file
	 */
	sad::resource::Tree * tree() const;
protected: 
	/*! A file name (with or without path), where file is stored
	 */
	sad::String m_name;
	/*! A tree, where all resources are stored
	 */
	sad::resource::Tree* m_tree;
	/*!  A resources, linked to file
	 */
	sad::Vector<sad::resource::Resource*> m_resources;
};

}

}
