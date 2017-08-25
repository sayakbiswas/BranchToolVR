#include "buildImageForTest.h"
#include <stdlib.h>

namespace puntoexe
{

namespace imebra
{

namespace tests
{


puntoexe::ptr<puntoexe::imebra::image> buildImageForTest(
	std::uint32_t pixelsX, 
	std::uint32_t pixelsY, 
	puntoexe::imebra::image::bitDepth depth,
	std::uint32_t highBit, 
	double sizeX, 
	double sizeY, 
	std::wstring colorSpace, 
	std::uint32_t continuity)
{
	puntoexe::ptr<puntoexe::imebra::image> newImage(new puntoexe::imebra::image);
	puntoexe::ptr<puntoexe::imebra::handlers::dataHandlerNumericBase> imgHandler = newImage->create(pixelsX, pixelsY, depth, colorSpace, highBit);
	std::uint32_t channelsNumber = newImage->getChannelsNumber();

	std::int32_t range = (std::uint32_t)1 << highBit;
	std::int32_t minValue = 0;
	if(depth == puntoexe::imebra::image::depthS16 || depth == puntoexe::imebra::image::depthS8)
	{
		minValue = -1 << (highBit - 1);
	}
	std::int32_t maxValue(minValue + range);

	std::uint32_t index(0);
	for(std::uint32_t scanY(0); scanY != pixelsY; ++scanY)
	{
		for(std::uint32_t scanX(0); scanX != pixelsX; ++scanX)
		{
			for(std::uint32_t scanChannels = 0; scanChannels != channelsNumber; ++scanChannels)
			{
				std::int32_t value = (std::int32_t)(((double)((scanX * channelsNumber+ scanY + scanChannels) % continuity) / (double)continuity) * (double)range)  + minValue;
 				if(value < minValue)
				{
					value = minValue;
				}
				if(value >= maxValue)
				{
					value = maxValue - 1;
				}
				imgHandler->setSignedLong(index++, value);
			}
		}
	}

	newImage->setSizeMm(sizeX, sizeY);

	return newImage;
}


double compareImages(ptr<image> image0, ptr<image> image1)
{
	std::uint32_t sizeX0, sizeY0, sizeX1, sizeY1;
	image0->getSize(&sizeX0, &sizeY0);
	image1->getSize(&sizeX1, &sizeY1);
	if(sizeX0 != sizeX1 || sizeY0 != sizeY1)
	{
		return 1000;
	}

	std::uint32_t rowSize, channelSize, channelsNumber0, channelsNumber1;
	ptr<handlers::dataHandlerNumericBase> hImage0 = image0->getDataHandler(false, &rowSize, &channelSize, &channelsNumber0);
	ptr<handlers::dataHandlerNumericBase> hImage1 = image1->getDataHandler(false, &rowSize, &channelSize, &channelsNumber1);
	if(channelsNumber0 != channelsNumber1)
	{
		return 1000;
	}

	std::uint32_t highBit0 = image0->getHighBit();
	std::uint32_t highBit1 = image1->getHighBit();
	if(highBit0 != highBit1)
	{
		return 1000;
	}

	image::bitDepth depth0 = image0->getDepth();
	image::bitDepth depth1 = image1->getDepth();
	if(depth0 != depth1)
	{
		return 1000;
	}
	
	if(sizeX0 == 0 || sizeY0 == 0)
	{
		return 0;
	}

	std::uint32_t valuesNum = sizeX0 * sizeY0 * channelsNumber0;
	double divisor = double(valuesNum);
	double range = (double)(1 << image0->getHighBit());
	double difference(0);
	int index(0);
	unsigned long long total(0);
	while(valuesNum--)
	{
		difference += 1000 * (double)labs(hImage0->getSignedLong(index) - hImage1->getSignedLong(index)) / range;
		total += labs(hImage0->getSignedLong(index));
		++index;
	}

	difference /= divisor;

	return difference;
}


} // namespace tests

} // namespace imebra

} // namespace puntoexe
