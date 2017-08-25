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

/*! \file applyLUT.h
    \brief Declaration of the class applyLUT.

*/

#if !defined(imebraApplyLUT_8347C70F_1FC8_4df8_A887_8DE9E968B2CF__INCLUDED_)
#define imebraApplyLUT_8347C70F_1FC8_4df8_A887_8DE9E968B2CF__INCLUDED_

#include "transform.h"
#include "image.h"
#include "dataSet.h"
#include "LUT.h"
#include "colorTransformsFactory.h"


///////////////////////////////////////////////////////////
//
// Everything is in the namespace puntoexe::imebra
//
///////////////////////////////////////////////////////////
namespace puntoexe
{

namespace imebra
{

namespace transforms
{

///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
/// \brief This exception is thrown by applyLUT
///         when the images passed to the transform are
///         not monochromatic.
///
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
class applyLUTException: public transformException
{
public:
    applyLUTException(const std::string& message): transformException(message){}
};


/// \addtogroup group_transforms
///
/// @{

///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
/// \brief This class applies a LUT to an image.
///
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
class applyLUT: public transformHandlers
{
public:
	/// \brief Constructor.
	///
    /// @param pLUT the LUT to apply to the image
	///
	///////////////////////////////////////////////////////////
    applyLUT(ptr<lut> pLUT);

	DEFINE_RUN_TEMPLATE_TRANSFORM;

	template <class inputType, class outputType>
			void templateTransform(
					inputType* inputHandlerData, size_t /* inputHandlerSize */, std::uint32_t inputHandlerWidth, const std::wstring& inputHandlerColorSpace,
					ptr<palette> /* inputPalette */,
                    std::int32_t /* inputHandlerMinValue */, std::uint32_t /* inputHighBit */,
					std::int32_t inputTopLeftX, std::int32_t inputTopLeftY, std::int32_t inputWidth, std::int32_t inputHeight,
					outputType* outputHandlerData, size_t /* outputHandlerSize */, std::int32_t outputHandlerWidth, const std::wstring& outputHandlerColorSpace,
					ptr<palette> /* outputPalette */,
                    std::int32_t /* outputHandlerMinValue */, std::uint32_t /* outputHighBit */,
					std::int32_t outputTopLeftX, std::int32_t outputTopLeftY)
	{
        PUNTOEXE_FUNCTION_START(L"applyLUT::templateTransform");
		if(!colorTransforms::colorTransformsFactory::isMonochrome(inputHandlerColorSpace) || !colorTransforms::colorTransformsFactory::isMonochrome(outputHandlerColorSpace))
		{
            PUNTOEXE_THROW(applyLUTException, "applyLUT can process only monochromatic images");
		}
		inputType* pInputMemory(inputHandlerData);
		outputType* pOutputMemory(outputHandlerData);

		pInputMemory += inputTopLeftY * inputHandlerWidth + inputTopLeftX;
		pOutputMemory += outputTopLeftY * outputHandlerWidth + outputTopLeftX;

        for(; inputHeight != 0; --inputHeight)
        {
            for(int scanPixels(inputWidth); scanPixels != 0; --scanPixels)
            {
                *(pOutputMemory++) = (outputType) ( m_lut->mappedValue((std::int32_t) *(pInputMemory++)) );
            }
            pInputMemory += (inputHandlerWidth - inputWidth);
            pOutputMemory += (outputHandlerWidth - inputWidth);
        }

		PUNTOEXE_FUNCTION_END();
	}

	virtual bool isEmpty();

	virtual ptr<image> allocateOutputImage(ptr<image> pInputImage, std::uint32_t width, std::uint32_t height);

private:
    ptr<lut> m_lut;
};


/// @}

} // namespace transforms

} // namespace imebra

} // namespace puntoexe

#endif // !defined(imebraApplyLUT_8347C70F_1FC8_4df8_A887_8DE9E968B2CF__INCLUDED_)
