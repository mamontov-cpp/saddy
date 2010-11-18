/*!	\file fontmanager.h
	\author Kolesov Dmitiy
	\brief Declaration of FontManager class

	This file contain declaration of FontManager class, which work with font
	load, render and free it.
*/

#ifndef FONTMANAGER_H__
#define FONTMANAGER_H__
#ifdef WIN32
#include <windows.h>
#include <WinDef.h>
#endif
#include "types.h"
#include "gl\gl.h"
#include "gl\glu.h"

#ifdef WIN32
typedef HFONT tfont;
#endif

/*!	\class This class work with one font: load, render and free it.
	Also class get and keep settings for render, size, colour, place...
*/
class Font
{
public:
	/*! Destructor free loaded font
	*/
	~Font();
	/*!	Default constructor, no load anything
	*/
	Font();
	/*! Copying constructor, copy all properties of getting object
		\param[in] other object for copying, this object will be copy of other
	*/
	Font(Font &other);
	/*! Constructor load font and size
		\param[in] name name (and path, if need) of file with true type font
	*/
	Font(hString &name, int size);
	/*!	function free current font and load new font
	\param[in] name name (and path, if need) of file with true type font
	*/
#ifdef WIN32
	/*!	Default constructor, no load anything
		\param hdc HDC
	*/
	Font(HDC hdc);
	/*! Constructor load font and size
		\param[in] name name (and path, if need) of file with true type font
		\param hdc HDC
	*/
	Font(hString &name, int size, HDC hdc);
#endif
	void reloadFont(hString &filename);
	/*!	function return last error in human redable form
		\return last error as string
	*/
	hString getLastError();
	/*!	function create texture with text and return it
		\param[in] text text to drawing on texture
		\param[in] size size of font
		\param[in] color color of text
		\param[out] lenght lenght of texture
		\param[out] height height of texture
		\return texture with drawing text as GLuint*
	*/
	GLuint *drawText(hString &text, int size, hColor color, int *lenght, int *height);

protected:
	/*! function load font from file with name
		\param[in] name name of file with font
		\return font pointer
	*/
	void loadFont(hString &name, int size);
	hString m_last_error;///<last error in human readable form
	GLuint m_font;///<loaded font
	bool m_font_loaded;
#ifdef WIN32
	HDC m_hdc;
#endif
};

typedef Font* pFont;

/*!	\class manage fonts classes: keep and return pointers to font classes and free it
*/
class FontManager
{
public:
	/*!	function create font object and return pointer
		\param[in] name name of file with font, it will be key for this font object
		\return pointer to font object, if font already exist return NULL
	*/
	pFont loadFont(hString &name, int size);
	/*!	function return font object with key
		\param[in] key key for object for returning
		\return pointer to font object
	*/
	pFont getFont(hString &key);
	/*!	function delete font object
		\param[in] key key of deleting object
	*/
	void removeFont(hString &key);
	/*! destructor delete all fonts object keeping in this manager
	*/
	~FontManager();
protected:
	hhash<hString, pFont> m_fonts;
};

#endif //FONTMANAGER_H__
	