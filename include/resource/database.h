/*! \file resource/database.h
	\author HiddenSeeker
	
	Contains definition of class Database.

	This class responds for all stored values in tables.
	Also here are stored file tables, which allows to fast reload of files.
 */
#pragma once


namespace resource
{

/*! \class Database

	This class responds for all stored values in tables.
	Also here are stored file tables, which allows to fast reload of files.
 */
class Database  
{	
public:	
	/*! This class can be inherited 
	 */
	virtual ~Database();
	/*! 
		\param[in] string
	 */
	void initFromString(sad::String string);
	/*! 
		\param[in] string
	 */
	void initFromFile(sad::String string);
	/*! 
		\param[in] file
		\return
	 */
	bool load(sad::String file);
	/*! 
		\param[in] file
		\return
	 */
	bool unload(sad::String file);
	/*! 
		\return
	 */
	resource::Folder* root();
	/*! 
		\param[in] name
		\return
	 */
	resource::PhysicalFile* file(sad::String name);
	/*! 
		\return
	 */
	sad::Vector<resource::PhysicalFile*>& files();
	/*! 
		\param[in] format
	 */
	void addFormat(resource::FileFormat* format);
	/*! 
	 */
	void clearFormats();
	/*! 
		\return
	 */
	resource::ResourceFactory* resourceFactory();
	/*! 
		\param[in] factory
	 */
	void setResourceFactory(resource::ResourceFactory* factory);
protected: 
	/*! 
	 */
	sad::Vector<resource::PhysicalFile*> m_files;
	/*! 
	 */
	resource::Folder* m_root;
	/*! 
	 */
	sad::Vector<resource::FileFormat*> m_formats;
	/*! 
	 */
	resource::ResourceFactory* m_resource_factory;

}

}