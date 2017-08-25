#include "../library/imebra/include/imebra.h"
#include "buildImageForTest.h"
#include <gtest/gtest.h>

namespace puntoexe
{

namespace imebra
{

namespace tests
{

using namespace puntoexe::imebra;

TEST(drawBitmapTest, testDrawBitmap)
{
	for(int monochrome(0); monochrome != 2; ++monochrome)
	{
		for(int subsample = 1; subsample != 4; subsample <<= 1)
		{
			ptr<image> testImage(buildImageForTest(
					400,
					300,
					puntoexe::imebra::image::depthU8,
					7,
					400,
					300,
					monochrome == 1 ? L"MONOCHROME2" : L"RGB",
					50));

			ptr<dataSet> testDataSet(new dataSet);
			testDataSet->setImage(0, testImage, L"1.2.840.10008.1.2", codecs::codec::high);

			int bufferX(400 / subsample), bufferY(300 / subsample);

			ptr<drawBitmap> testDraw(new drawBitmap(testImage, 0));
			ptr<memory> bitmapBuffer(testDraw->getBitmap<puntoexe::imebra::drawBitmapRGB, 1>(bufferX, bufferY, 0, 0, bufferX, bufferY, 0));
			std::uint8_t* pBuffer(bitmapBuffer->data());

			std::uint32_t imageRowSize, imageChannelPixelSize, channels;
			ptr<handlers::dataHandlerNumericBase > pImageHandler(testImage->getDataHandler(false, &imageRowSize, &imageChannelPixelSize, &channels));
			std::uint8_t* pImageBuffer (pImageHandler->getMemoryBuffer());

			for(int scanY = 0; scanY != bufferY; ++scanY)
			{
				for(int scanX = 0; scanX != bufferX; ++scanX)
				{
					std::int32_t averageRed(0), averageGreen(0), averageBlue(0);
					for(int scanSubY(0); scanSubY != subsample; ++scanSubY)
					{
						for(int scanSubX(0); scanSubX != subsample; ++scanSubX)
						{
							std::uint32_t pixel = (scanX * subsample + scanSubX) * channels + (scanY * subsample + scanSubY) * (bufferX * subsample * channels);
							if(monochrome)
							{
								averageRed += pImageBuffer[pixel];
								averageGreen += pImageBuffer[pixel];
								averageBlue += pImageBuffer[pixel];
							}
							else
							{
								averageRed += pImageBuffer[pixel];
								averageGreen += pImageBuffer[++pixel];
								averageBlue += pImageBuffer[++pixel];
							}
						}
					}
					averageRed /= subsample * subsample;
					averageGreen /= subsample * subsample;
					averageBlue /= subsample * subsample;

					std::uint8_t displayRed(*pBuffer++);
					std::uint8_t displayGreen(*pBuffer++);
					std::uint8_t displayBlue(*pBuffer++);
                    EXPECT_EQ(averageRed, displayRed);
                    EXPECT_EQ(averageGreen, displayGreen);
                    EXPECT_EQ(averageBlue, displayBlue);
				}

			}
		}
	}

}


} // namespace tests

} // namespace imebra

} // namespace puntoexe
