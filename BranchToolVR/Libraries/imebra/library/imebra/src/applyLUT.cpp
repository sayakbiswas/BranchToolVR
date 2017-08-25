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

/*! \file applyLUT.cpp
    \brief Implementation of the class applyLUT.

*/

#include "../../base/include/exception.h"
#include "../include/applyLUT.h"
#include "../include/colorTransformsFactory.h"

namespace puntoexe
{

namespace imebra
{

namespace transforms
{


///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
//
//
// Apply a LUT to an image
//
//
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
applyLUT::applyLUT(ptr<lut> pLUT):
        m_lut(pLUT)

{
}

bool applyLUT::isEmpty()
{
    return m_lut == 0 || m_lut->getSize() == 0 || !m_lut->checkValidDataRange();
}


ptr<image> applyLUT::allocateOutputImage(ptr<image> pInputImage, std::uint32_t width, std::uint32_t height)
{
	if(isEmpty())
	{
        ptr<image> newImage(new image());
		newImage->create(width, height, pInputImage->getDepth(), pInputImage->getColorSpace(), pInputImage->getHighBit());
		return newImage;
	}

	// LUT
	///////////////////////////////////////////////////////////
    std::uint8_t bits(m_lut->getBits());

    // Look for negative outputs
    bool bNegative(false);
    for(std::int32_t index(m_lut->getFirstMapped()), size(m_lut->getSize()); !bNegative && size != 0; --size, ++index)
    {
        bNegative = (m_lut->mappedValue(index) < 0);
    }

    image::bitDepth depth;
    if(bNegative)
    {
        if(bits > 16)
        {
            depth = image::depthS32;
        }
        else if(bits > 8)
        {
            depth = image::depthS16;
        }
        else
        {
            depth = image::depthS8;
        }
    }
    else
    {
        if(bits > 16)
        {
            depth = image::depthU32;
        }
        else if(bits > 8)
        {
            depth = image::depthU16;
        }
        else
        {
            depth = image::depthU8;
        }
    }
    ptr<image> returnImage(new image);
    returnImage->create(width, height, depth, L"MONOCHROME2", bits - 1);
    return returnImage;
}



} // namespace transforms

} // namespace imebra

} // namespace puntoexe
