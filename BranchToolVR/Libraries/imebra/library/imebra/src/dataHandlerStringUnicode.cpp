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

/*! \file dataHandlerStringUnicode.cpp
    \brief Implementation of the base class used by the string handlers that need
	        to handle several charsets.

*/

#include "../../base/include/exception.h"
#include "../include/dataHandlerStringUnicode.h"


namespace puntoexe
{

namespace imebra
{

namespace handlers
{

///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
//
//
//
// dataHandlerStringUnicode
//
//
//
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////


dataHandlerStringUnicode::dataHandlerStringUnicode()
{
}

///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
//
//
// Convert a string stored in a dicom tag to an unicode
//  string
//
//
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
std::wstring dataHandlerStringUnicode::convertToUnicode(const std::string& value) const
{
	PUNTOEXE_FUNCTION_START(L"dataHandlerStringUnicode::convertToUnicode");

	// Should we take care of the escape sequences...?
	///////////////////////////////////////////////////////////
    if(m_charsetsList.empty())
	{
        throw std::logic_error("The charsets list must be set before converting to unicode");
	}

    // Initialize the conversion engine with the default
    //  charset
    ///////////////////////////////////////////////////////////
    std::unique_ptr<defaultCharsetConversion> localCharsetConversion(new defaultCharsetConversion(m_charsetsList.front()));

    // Only one charset is present: we don't need to check
    //  the escape sequences
    ///////////////////////////////////////////////////////////
    if(m_charsetsList.size() == 1)
    {
        return localCharsetConversion->toUnicode(value);
    }

	// Here we store the value to be returned
	///////////////////////////////////////////////////////////
	std::wstring returnString;
    returnString.reserve(value.size());

    // Get the escape sequences from the unicode conversion
    //  engine
    ///////////////////////////////////////////////////////////
    const charsetDictionary::escapeSequences_t& escapeSequences(localCharsetConversion->getDictionary().getEscapeSequences());

    // Position and properties of the next escape sequence
    ///////////////////////////////////////////////////////////
    size_t escapePosition = std::string::npos;
    std::string escapeString;
    std::string isoTable;

	// Scan all the string and look for valid escape sequences.
	// The partial strings are converted using the dicom
	//  charset specified by the escape sequences.
	///////////////////////////////////////////////////////////
    for(size_t scanString = 0; scanString < value.size(); /* empty */)
	{
		// Find the position of the next escape sequence
		///////////////////////////////////////////////////////////
        if(escapePosition == std::string::npos)
        {
            escapePosition = value.size();
            for(charsetDictionary::escapeSequences_t::const_iterator scanEscapes(escapeSequences.begin()), endEscapes(escapeSequences.end());
                scanEscapes != endEscapes;
                ++scanEscapes)
            {
                size_t findEscape = value.find(scanEscapes->first, scanString);
                if(findEscape != std::string::npos && findEscape < escapePosition)
                {
                    escapePosition = findEscape;
                    escapeString = scanEscapes->first;
                    isoTable = scanEscapes->second;
                }
            }
        }

        // No more escape sequences. Just convert everything
        ///////////////////////////////////////////////////////////
        if(escapePosition == value.size())
        {
            return returnString + localCharsetConversion->toUnicode(value.substr(scanString));
        }

        // The escape sequence can wait, now we are still in the
        //  already activated charset
        ///////////////////////////////////////////////////////////
        if(escapePosition > scanString)
		{
            returnString += localCharsetConversion->toUnicode(value.substr(scanString, escapePosition - scanString));
            scanString = escapePosition;
            continue;
		}

		// Move the char pointer to the next char that has to be
		//  analyzed
		///////////////////////////////////////////////////////////
		scanString = escapePosition + escapeString.length();
        escapePosition = std::string::npos;

		// An iso table is coupled to the found escape sequence.
		///////////////////////////////////////////////////////////
        localCharsetConversion.reset(new defaultCharsetConversion(isoTable));
	}

	return returnString;

	PUNTOEXE_FUNCTION_END();
}


///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
//
//
// Convert an unicode string to a string ready to be
//  stored in a dicom tag
//
//
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
std::string dataHandlerStringUnicode::convertFromUnicode(const std::wstring& value, charsetsList::tCharsetsList* pCharsetsList) const
{
	PUNTOEXE_FUNCTION_START(L"dataHandlerStringUnicode::convertFromUnicode");

    // Check for the dicom charset's name
    ///////////////////////////////////////////////////////////
    if(pCharsetsList->empty())
    {
        throw std::logic_error("The charsets list must be set before converting from unicode");
    }

    // Setup the conversion objects
    ///////////////////////////////////////////////////////////
    std::unique_ptr<defaultCharsetConversion> localCharsetConversion(new defaultCharsetConversion(pCharsetsList->front()));

    // Get the escape sequences from the unicode conversion
    //  engine
    ///////////////////////////////////////////////////////////
    const charsetDictionary::escapeSequences_t& escapes(localCharsetConversion->getDictionary().getEscapeSequences());

	// Returned string
	///////////////////////////////////////////////////////////
    std::string rawString;
    rawString.reserve(value.size());
	
	// Convert all the chars. Each char is tested with the
	//  active charset first, then with other charsets if
	//  the active one doesn't work
	///////////////////////////////////////////////////////////
    for(size_t scanString = 0; scanString != value.size(); ++scanString)
	{
        // Get the UNICODE char. On windows the code may be spread
        //  across 2 16 bit wide codes.
		///////////////////////////////////////////////////////////
        std::wstring code(size_t(1), value[scanString]);

        // Check UTF-16 extension (Windows only)
        ///////////////////////////////////////////////////////////
        if(sizeof(wchar_t) == 2)
        {
            if(code[0] >= 0xd800 && code[0] <=0xdfff && scanString < (value.size() - 1))
            {
                code += value[++scanString];
            }
        }

        // Check composed chars extension (diacritical marks)
        ///////////////////////////////////////////////////////////
        while(scanString < (value.size() - 1) && value[scanString + 1] >= 0x0300 && value[scanString + 1] <= 0x036f)
        {
            code += value[++scanString];
        }

        // Remember the return string size so we can check if we
        //  added something to it
        ///////////////////////////////////////////////////////////
        size_t currentRawSize(rawString.size());
        rawString += localCharsetConversion->fromUnicode(code);
        if(rawString.size() != currentRawSize)
        {
            // The conversion succeeded: continue with the next char
            ///////////////////////////////////////////////////////////
            continue;
        }

		// Find the escape sequence
		///////////////////////////////////////////////////////////
        for(charsetDictionary::escapeSequences_t::const_iterator scanEscapes(escapes.begin()), endEscapes(escapes.end());
            scanEscapes != endEscapes;
            ++scanEscapes)
		{
			try
			{
                std::unique_ptr<defaultCharsetConversion> testEscapeSequence(new defaultCharsetConversion(scanEscapes->second));
                std::string convertedChar(testEscapeSequence->fromUnicode(code));
                if(!convertedChar.empty())
				{
                    rawString += scanEscapes->first;
                    rawString += convertedChar;

                    localCharsetConversion.reset(testEscapeSequence.release());

					// Add the dicom charset to the charsets
					///////////////////////////////////////////////////////////
					bool bAlreadyUsed = false;
					for(charsetsList::tCharsetsList::const_iterator scanUsedCharsets = pCharsetsList->begin(); scanUsedCharsets != pCharsetsList->end(); ++scanUsedCharsets)
					{
                        if(*scanUsedCharsets == scanEscapes->second)
						{
							bAlreadyUsed = true;
							break;
						}
					}
					if(!bAlreadyUsed)
					{
                        pCharsetsList->push_back(scanEscapes->second);
					}
					break;
				}
			}
			catch(charsetConversionExceptionNoSupportedTable)
			{
				continue;
			}
		}
	}

    return rawString;

	PUNTOEXE_FUNCTION_END();
}


///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
//
//
// Set the charset used in the tag
//
//
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
void dataHandlerStringUnicode::setCharsetsList(charsetsList::tCharsetsList* pCharsetsList)
{
	PUNTOEXE_FUNCTION_START(L"dataHandlerStringUnicode::setCharsetInfo");

	// Copy the specified charsets into the tag
	///////////////////////////////////////////////////////////
	m_charsetsList.clear();
	charsetsList::updateCharsets(pCharsetsList, &m_charsetsList);

	// If no charset has been defined then we use the default 
	//  one
	///////////////////////////////////////////////////////////
	if(m_charsetsList.empty())
	{
        m_charsetsList.push_back("ISO 2022 IR 6");
	}

	PUNTOEXE_FUNCTION_END();
}


///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
//
//
// Retrieve the dicom charsets used in the string
//
//
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
void dataHandlerStringUnicode::getCharsetsList(charsetsList::tCharsetsList* pCharsetsList) const
{
	PUNTOEXE_FUNCTION_START(L"dataHandlerStringUnicode::getCharsetList");

    pCharsetsList->insert(pCharsetsList->end(), m_charsetsList.begin(), m_charsetsList.end());

	PUNTOEXE_FUNCTION_END();
}

} // namespace handlers

} // namespace imebra

} // namespace puntoexe
