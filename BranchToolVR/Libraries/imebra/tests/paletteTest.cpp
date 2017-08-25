#include "../library/imebra/include/imebra.h"
#include <gtest/gtest.h>

namespace puntoexe
{

namespace imebra
{

namespace tests
{


// A buffer initialized to a default data type should use the data type OB
TEST(paletteTest, testPalette)
{
    ptr<lut> red(new lut());
    ptr<lut> green(new lut());
    ptr<lut> blue(new lut());

    red->create(256, 0, 8, L"Test Red");
    green->create(256, 0, 8, L"Test Green");
    blue->create(256, 0, 8, L"Test Blue");

    for(size_t fillPalette(0); fillPalette != 256; ++fillPalette)
    {
        red->setLutValue(fillPalette, (fillPalette + 10) & 0xff);
        green->setLutValue(fillPalette, (fillPalette + 21) & 0xff);
        blue->setLutValue(fillPalette, (fillPalette + 32) & 0xff);
    }

    ptr<palette> newPalette(new palette(red, green, blue));
	std::uint32_t sizeX = 600;
	std::uint32_t sizeY = 400;
    ptr<image> paletteImage(new image);
    paletteImage->create(sizeX, sizeY, image::depthU8, L"PALETTE COLOR", 7);
    paletteImage->setPalette(newPalette);

	std::uint32_t rowSize, channelsPixelSize, channelsNumber;
    ptr<handlers::dataHandlerNumericBase> imageHandler = paletteImage->getDataHandler(true, &rowSize, &channelsPixelSize, &channelsNumber);

    size_t pointer(0);

	// Make 3 bands (RGB)
	for(std::uint32_t y=0; y<sizeY; ++y)
	{
		for(std::uint32_t x=0; x<sizeX; ++x)
		{
            imageHandler->setUnsignedLong(pointer, pointer & 0xff);
            ++pointer;
		}
	}
	imageHandler.release();

	ptr<transforms::colorTransforms::colorTransformsFactory> colorFactory;
	colorFactory = transforms::colorTransforms::colorTransformsFactory::getColorTransformsFactory();
    ptr<transforms::transform> colorTransform = colorFactory->getTransform(L"PALETTE COLOR", L"RGB");

    ptr<image> rgbImage(colorTransform->allocateOutputImage(paletteImage, sizeX, sizeY));
    colorTransform->runTransform(paletteImage, 0, 0, sizeX, sizeY, rgbImage, 0, 0);

    std::uint32_t checkSizeX, checkSizeY;
	rgbImage->getSize(&checkSizeX, &checkSizeY);

    ptr<handlers::dataHandlerNumericBase> rgbHandler = rgbImage->getDataHandler(false, &rowSize, &channelsPixelSize, &channelsNumber);

	// Compare the buffers. A little difference is allowed
    ASSERT_EQ(sizeX, checkSizeX);
    ASSERT_EQ(sizeY, checkSizeY);

    pointer = 0;
    size_t rgbPointer = 0;
	for(std::uint32_t checkY = 0; checkY < sizeY; ++checkY)
	{
		for(std::uint32_t checkX = 0; checkX < sizeX; ++checkX)
		{
            std::uint32_t r = rgbHandler->getUnsignedLong(rgbPointer++);
            std::uint32_t g = rgbHandler->getUnsignedLong(rgbPointer++);
            std::uint32_t b = rgbHandler->getUnsignedLong(rgbPointer++);

            ASSERT_EQ(((pointer & 0xff) + 10) & 0xff, r);
            ASSERT_EQ(((pointer & 0xff) + 21) & 0xff, g);
            ASSERT_EQ(((pointer & 0xff) + 32) & 0xff, b);

            ++pointer;
		}
	}
}



} // namespace tests

} // namespace imebra

} // namespace puntoexe
