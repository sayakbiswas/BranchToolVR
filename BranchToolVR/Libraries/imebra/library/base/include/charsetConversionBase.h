/*

Imebra 2015 build 20160119-002

Imebra: a C++ Dicom library

Copyright (c) 2003, 2004, 2005, 2006, 2007, 2008, 2009, 2010, 2011, 2012, 2013, 2014, 2015
by Paolo Brandoli/Binarno s.p.

All rights reserved.

This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License version 2 as published by
 the Free Software Foundation.

This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

-------------------

If you want to use Imebra commercially then you have to buy the commercial
 license available at http://imebra.com

After you buy the commercial license then you can use Imebra according
 to the terms described in the Imebra Commercial License Version 2.
A copy of the Imebra Commercial License Version 2 is available in the
 documentation pages.

Imebra is available at http://imebra.com

The author can be contacted by email at info@binarno.com or by mail at
 the following address:
 Binarno s.p., Paolo Brandoli
 Rakuseva 14
 1000 Ljubljana
 Slovenia



*/

/*! \file charsetConversion.h
    \brief Declaration of the class used to convert a string between different
	        charsets.

The class hides the platform specific implementations and supplies a common
 interface for the charsets translations.

*/

#if !defined(imebraCharsetConversionBase_3146DA5A_5276_4804_B9AB_A3D54C6B123A__INCLUDED_)
#define imebraCharsetConversionBase_3146DA5A_5276_4804_B9AB_A3D54C6B123A__INCLUDED_

#include "configuration.h"
#include "baseObject.h"
#include <string>
#include <stdexcept>
#include <map>

///////////////////////////////////////////////////////////
//
// Everything is in the namespace puntoexe
//
///////////////////////////////////////////////////////////
namespace puntoexe
{

/// \addtogroup group_baseclasses
///
/// @{

///////////////////////////////////////////////////////////
/// \internal
/// \brief Stores the information related to a single
///         charset.
///
///////////////////////////////////////////////////////////
struct charsetInformation
{
    charsetInformation(
            const std::string& dicomName,
            const std::string& escapeSequence,
            const std::string& isoRegistration,
            unsigned long codePage,
            bool bZeroFlag);

    charsetInformation(const charsetInformation& right);

    charsetInformation& operator=(const charsetInformation& right);

    std::string m_dicomName;       ///< DICOM name for the charset
    std::string m_escapeSequence;  ///< Escape sequence for ISO 2022
    std::string m_isoRegistration; ///< ISO registration string
    unsigned long m_codePage;      ///< codePage used by Windows
    bool m_bZeroFlag;              ///< needs flags=0 in Windows
};


///////////////////////////////////////////////////////////
/// \internal
/// \brief Stores all the recognized charsets.
///
///////////////////////////////////////////////////////////
class charsetDictionary
{
public:
    charsetDictionary();

    const charsetInformation& getCharsetInformation(const std::string& dicomName) const;

    typedef std::map<std::string, std::string> escapeSequences_t;
    const escapeSequences_t& getEscapeSequences() const;

private:
    void registerCharset(const std::string& dicomName, const std::string& escapeSequence, const std::string& isoName, const unsigned long windowsPage, const bool bZeroFlag);

    typedef std::map<std::string, charsetInformation> dictionary_t;
    dictionary_t m_dictionary;

    escapeSequences_t m_escapeSequences;
};

///////////////////////////////////////////////////////////
/// \internal
/// \brief This class converts a string from multibyte to
///         unicode and viceversa.
///
/// The class uses the function iconv on Posix systems and
///  the functions MultiByteToWideChars and
///  WideCharsToMultiByte on Windows systems.
///
///////////////////////////////////////////////////////////
class charsetConversionBase
{
public:

    virtual ~charsetConversionBase();

    /// \brief Retrieve the only instance of the charset
    ///         dictionary.
    ///
    /// @return the only instance of the charset dictionary
    ///
    ///////////////////////////////////////////////////////////
    const charsetDictionary& getDictionary() const;

    /// \brief Removes all the non alphanumeric and non digit
    ///        chars from the string, convert it to upper-case.
    ///
    /// @return the normalized DICOM charset name (to be used
    ///          with the charset dictionary)
    ///
    ///////////////////////////////////////////////////////////
    static std::string normalizeIsoCharset(const std::string& isoCharset);

	/// \brief Transform a multibyte string into an unicode
	///         string using the charset declared with the
	///         method initialize().
	///
	/// initialize() must have been called before calling this
	///  method.
	///
	/// @param unicodeStr
	///
	///////////////////////////////////////////////////////////
    virtual std::string fromUnicode(const std::wstring& unicodeString) const = 0;

	/// \brief Transform a multibyte string into an unicode
	///         string using the charset declared with the
	///         method initialize().
	///
	/// initialize() must have been called before calling this
	///  method.
	///
	/// @param asciiString the multibyte string that will be
	///                     converted to unicode
	/// @return            the converted unicode string
	///
	///////////////////////////////////////////////////////////
    virtual std::wstring toUnicode(const std::string& asciiString) const = 0;

};



///////////////////////////////////////////////////////////
/// \brief Base class for the exceptions thrown by
///         charsetConversion.
///
///////////////////////////////////////////////////////////
class charsetConversionException: public std::runtime_error
{
public:
	charsetConversionException(const std::string& message): std::runtime_error(message){}
};


///////////////////////////////////////////////////////////
/// \brief Exception thrown when the requested charset
///         is not supported by the DICOM standard.
///
///////////////////////////////////////////////////////////
class charsetConversionExceptionNoTable: public charsetConversionException
{
public:
	charsetConversionExceptionNoTable(const std::string& message): charsetConversionException(message){}
};


///////////////////////////////////////////////////////////
/// \brief Exception thrown when the requested charset
///         is not supported by the system.
///
///////////////////////////////////////////////////////////
class charsetConversionExceptionNoSupportedTable: public charsetConversionException
{
public:
	charsetConversionExceptionNoSupportedTable(const std::string& message): charsetConversionException(message){}
};


///////////////////////////////////////////////////////////
/// \brief Exception thrown when the system doesn't have
///         a supported size for wchar_t
///
///////////////////////////////////////////////////////////
class charsetConversionExceptionUtfSizeNotSupported: public charsetConversionException
{
public:
	charsetConversionExceptionUtfSizeNotSupported(const std::string& message): charsetConversionException(message){}
};

///@}

} // namespace puntoexe



#endif // !defined(imebraCharsetConversionBase_3146DA5A_5276_4804_B9AB_A3D54C6B123A__INCLUDED_)
