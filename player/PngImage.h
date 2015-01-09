// ----------
// PngImage.h
// ----------
/**
* @file
* @brief Re: PngImage
* @author Achim Klein
* @version 0.2
*/


// -------------------
// One-Definition-Rule
// -------------------
#ifndef PNGIMAGE_H_INCLUDE_NR1
#define PNGIMAGE_H_INCLUDE_NR1


// --------
// Includes
// --------
#include "png.h"


// --------------------------------
// Definition of the PngImage class
// --------------------------------
/**
 * This class reads png files.
 */
class PngImage
{

public:

	// ------------
	// Construction
	// ------------

	/// standard-constructor
	PngImage();

	/// virtual destructor
	virtual ~PngImage();


	// --------
	// Handling
	// --------

	/// reads a png file
	bool load(const char* Path);

	/// returns true if the object contains valid data
	bool good() const;


	// --------------
	// Getter methods
	// --------------

	/// returns the image's width
	int getWidth() const;

	/// returns the image's height
	int getHeight() const;

	/// returns the image's RGB+Alpha values (8 bit each)
	unsigned char* getBGRA() const;


protected:

	// ----------------
	// Internal methods
	// ----------------

	/// returns true if the specified file is a png file
	bool doCheckFileHeader(const char* Path) const;

	/// converts the png bytes to BGRA
	bool doExtractCanonicData(png_structp& PngPtr, png_infop& InfoPtr);

	/// gets the data from an 8-bit rgb image
	unsigned char* doConvertRGB8(png_structp& PngPtr, png_infop& InfoPtr) const;

	/// gets the data from an 8-bit rgb image with alpha values
	unsigned char* doConvertRGBA8(png_structp& PngPtr, png_infop& InfoPtr) const;

	/// gets the data from an 8-bit monochrome image
	unsigned char* doConvertGrey8(png_structp& PngPtr, png_infop& InfoPtr) const;

	/// gets the data from an 8-bit monochrome image with alpha values
	unsigned char* doConvertGreyA8(png_structp& PngPtr, png_infop& InfoPtr) const;


private:

	// ----------
	// Attributes
	// ----------

	/// health flag
	bool m_good;

	/// the image's width
	int m_width;

	/// the image's height
	int m_height;

	/// the color values (blue, green, red, alpha)
	unsigned char* m_bgra;
};


// --------------
// Inline methods
// --------------

#endif	// PNGIMAGE_H_INCLUDE_NR1

