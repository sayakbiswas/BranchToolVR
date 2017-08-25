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

/*! \file charsetConversion.cpp
    \brief Implementation of the charsetConversion class.

*/

#include "../include/exception.h"
#include "../include/charsetConversion.h"
#include <memory>
#include <sstream>

namespace puntoexe
{

charsetDictionary::charsetDictionary()
{
    registerCharset("ISO_IR 6", "", "ISO-IR-6", 1252, false);
    registerCharset("ISO_IR 100", "", "ISO-IR-100", 1252, false);
    registerCharset("ISO_IR 101", "", "ISO-IR-101", 28592, false);
    registerCharset("ISO_IR 109", "", "ISO-IR-109", 28593, false);
    registerCharset("ISO_IR 110", "", "ISO-IR-110", 28594, false);
    registerCharset("ISO_IR 144", "", "ISO-IR-144", 28595, false);
    registerCharset("ISO_IR 127", "", "ISO-IR-127", 28596, false);
    registerCharset("ISO_IR 126", "", "ISO-IR-126", 28597, false);
    registerCharset("ISO_IR 138", "", "ISO-IR-138", 28598, false);
    registerCharset("ISO_IR 148", "", "ISO-IR-148", 28599, false);
    registerCharset("ISO_IR 149", "", "ISO-IR-149", 949, false);
    registerCharset("ISO_IR 13",  "", "ISO-IR-13", 50930, false);
    registerCharset("ISO_IR 14",  "", "ISO-IR-14", 932, false );
    registerCharset("ISO_IR 166", "", "ISO-IR-166", 874, false);
    registerCharset("ISO_IR 87",  "", "ISO-IR-87", 20932, false);
    registerCharset("ISO_IR 159", "", "ISO-IR-159", 20932, false);

    registerCharset("ISO 2022 IR 6",   "\x1b\x28\x42", "ISO-IR-6", 1252, false);
    registerCharset("ISO 2022 IR 100", "\x1b\x2d\x41", "ISO-IR-100", 1252, false);
    registerCharset("ISO 2022 IR 101", "\x1b\x2d\x42", "ISO-IR-101", 28592, false);
    registerCharset("ISO 2022 IR 109", "\x1b\x2d\x43", "ISO-IR-109", 28593, false);
    registerCharset("ISO 2022 IR 110", "\x1b\x2d\x44", "ISO-IR-110", 28594, false);
    registerCharset("ISO 2022 IR 144", "\x1b\x2d\x4c", "ISO-IR-144", 28595, false);
    registerCharset("ISO 2022 IR 127", "\x1b\x2d\x47", "ISO-IR-127", 28596, false);
    registerCharset("ISO 2022 IR 126", "\x1b\x2d\x46", "ISO-IR-126", 28597, false);
    registerCharset("ISO 2022 IR 138", "\x1b\x2d\x48", "ISO-IR-138", 28598, false);
    registerCharset("ISO 2022 IR 148", "\x1b\x2d\x4d", "ISO-IR-148", 28599, false);
    registerCharset("ISO 2022 IR 149", "\x1b\x24\x29\x43", "ISO-IR-149", 949, false);
    registerCharset("ISO 2022 IR 13",  "\x1b\x29\x49", "ISO-IR-13", 50930, false);
    registerCharset("ISO 2022 IR 14",  "\x1b\x28\x4a", "ISO-IR-14", 932, false);
    registerCharset("ISO 2022 IR 166", "\x1b\x2d\x54", "ISO-IR-166", 874, false);
    registerCharset("ISO 2022 IR 87",  "\x1b\x24\x42", "ISO-IR-87", 20932, false);
    registerCharset("ISO 2022 IR 159", "\x1b\x24\x28\x44", "ISO-IR-159", 20932, false);

    registerCharset("ISO_IR 192", "", "UTF-8", 65001, true);
    registerCharset("GB18030",    "", "GB18030", 54936, true);

    for(dictionary_t::const_iterator scanInfo(m_dictionary.begin()), endInfo(m_dictionary.end()); scanInfo != endInfo; ++scanInfo)
    {
        if(!scanInfo->second.m_escapeSequence.empty())
        {
            m_escapeSequences[scanInfo->second.m_escapeSequence] = scanInfo->first;
        }
    }
}

const charsetInformation& charsetDictionary::getCharsetInformation(const std::string& dicomName) const
{
    std::string normalizedName(charsetConversionBase::normalizeIsoCharset((dicomName)));
    dictionary_t::const_iterator findInfo(m_dictionary.find(normalizedName));
    if(findInfo == m_dictionary.end())
    {
        std::ostringstream errorMessage;
        errorMessage << "Charset table " << dicomName << " not found in the charset dictionary";
        throw charsetConversionExceptionNoTable(errorMessage.str());
    }
    return findInfo->second;
}

const charsetDictionary::escapeSequences_t& charsetDictionary::getEscapeSequences() const
{
    return m_escapeSequences;
}

void charsetDictionary::registerCharset(const std::string& dicomName, const std::string& escapeSequence, const std::string& isoName, const unsigned long windowsPage, const bool bZeroFlag)
{
    m_dictionary.insert(std::pair<std::string, charsetInformation>(
                            charsetConversionBase::normalizeIsoCharset(dicomName),
                            charsetInformation(dicomName, escapeSequence, isoName, windowsPage, bZeroFlag)));
}

charsetInformation::charsetInformation(const std::string &dicomName, const std::string &escapeSequence, const std::string &isoRegistration, unsigned long codePage, bool bZeroFlag):
    m_dicomName(dicomName),
    m_escapeSequence(escapeSequence),
    m_isoRegistration(isoRegistration),
    m_codePage(codePage),
    m_bZeroFlag(bZeroFlag)
{

}

charsetInformation::charsetInformation(const charsetInformation &right):
    m_dicomName(right.m_dicomName),
    m_escapeSequence(right.m_escapeSequence),
    m_isoRegistration(right.m_isoRegistration),
    m_codePage(right.m_codePage),
    m_bZeroFlag(right.m_bZeroFlag)
{

}

charsetInformation& charsetInformation::operator =(const charsetInformation& right)
{
    m_dicomName = right.m_dicomName;
    m_escapeSequence = right.m_escapeSequence;
    m_isoRegistration = right.m_isoRegistration;
    m_codePage = right.m_codePage;
    m_bZeroFlag = right.m_bZeroFlag;

    return *this;
}


//////////////////////////////////////////////////////////
//
// Destructor
//
///////////////////////////////////////////////////////////
charsetConversionBase::~charsetConversionBase()
{
}


const charsetDictionary& charsetConversionBase::getDictionary() const
{
    static charsetDictionary dictionary;
    return dictionary;
}


std::string charsetConversionBase::normalizeIsoCharset(const std::string &isoCharset)
{
    std::string normalizedIsoCharset;

    normalizedIsoCharset.reserve(isoCharset.size());

    for(size_t scanChars(0), endChars(isoCharset.size()); scanChars != endChars; ++scanChars)
    {
        char isoChar(isoCharset[scanChars]);

        if(isoChar >= 'a' && isoChar <= 'z')
        {
            normalizedIsoCharset.push_back(isoChar - ('a' - 'A'));
            continue;
        }

        if( (isoChar >= 'A' && isoChar <= 'Z') || (isoChar >= '0' && isoChar <= '9'))
        {
            normalizedIsoCharset.push_back(isoChar);
        }
    }

    return normalizedIsoCharset;
}

} // namespace puntoexe

