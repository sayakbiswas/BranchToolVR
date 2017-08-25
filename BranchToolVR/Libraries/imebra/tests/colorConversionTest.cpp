#include "../library/imebra/include/imebra.h"
#include "buildImageForTest.h"
#include <gtest/gtest.h>

namespace puntoexe
{

namespace imebra
{

namespace tests
{

TEST(colorConversion, RGB2YBR_FULL)
{
    ptr<image> rgb(new image());
    rgb->create(1, 1, image::depthU8, L"RGB", 7);

    std::uint32_t rowSize, channelsPixelSize, channelsNumber;
    ptr<handlers::dataHandlerNumericBase> rgbHandler = rgb->getDataHandler(true, &rowSize, &channelsPixelSize, &channelsNumber);
    rgbHandler->setUnsignedLong(0, 255);
    rgbHandler->setUnsignedLong(1, 255);
    rgbHandler->setUnsignedLong(2, 255);

    rgbHandler = 0;

    ptr<transforms::transform> rgb2ybr = transforms::colorTransforms::colorTransformsFactory::getColorTransformsFactory()->getTransform(L"RGB", L"YBR_FULL");

    ptr<image> ybr1(new image());
    ybr1->create(1, 1, image::depthU8, L"YBR_FULL", 4);
    rgb2ybr->runTransform(rgb, 0, 0, 1, 1, ybr1, 0, 0);
    ptr<handlers::dataHandlerNumericBase> ybr1Handler = ybr1->getDataHandler(false, &rowSize, &channelsPixelSize, &channelsNumber);

    ASSERT_EQ(31, ybr1Handler->getSignedLong(0));
    ASSERT_EQ(16, ybr1Handler->getSignedLong(1));
    ASSERT_EQ(16, ybr1Handler->getSignedLong(2));

    ptr<image> ybr2(new image());
    ybr2->create(1, 1, image::depthS16, L"YBR_FULL", 12);
    rgb2ybr->runTransform(rgb, 0, 0, 1, 1, ybr2, 0, 0);
    ptr<handlers::dataHandlerNumericBase> ybr2Handler = ybr2->getDataHandler(false, &rowSize, &channelsPixelSize, &channelsNumber);

    ASSERT_EQ(4064, ybr2Handler->getSignedLong(0));
    ASSERT_EQ(0, ybr2Handler->getSignedLong(1));
    ASSERT_EQ(0, ybr2Handler->getSignedLong(2));

}

TEST(colorConversion, YBRFULL2RGB)
{
    ptr<image> ybr(new image());
    ybr->create(1, 1, image::depthU8, L"YBR_FULL", 7);

    std::uint32_t rowSize, channelsPixelSize, channelsNumber;
    ptr<handlers::dataHandlerNumericBase> ybrHandler = ybr->getDataHandler(true, &rowSize, &channelsPixelSize, &channelsNumber);
    ybrHandler->setUnsignedLong(0, 255);
    ybrHandler->setUnsignedLong(1, 128);
    ybrHandler->setUnsignedLong(2, 128);

    ybrHandler = 0;

    ptr<transforms::transform> ybr2rgb = transforms::colorTransforms::colorTransformsFactory::getColorTransformsFactory()->getTransform(L"YBR_FULL", L"RGB");

    ptr<image> rgb1(new image());
    rgb1->create(1, 1, image::depthU8, L"RGB", 4);
    ybr2rgb->runTransform(ybr, 0, 0, 1, 1, rgb1, 0, 0);
    ptr<handlers::dataHandlerNumericBase> rgb1Handler = rgb1->getDataHandler(false, &rowSize, &channelsPixelSize, &channelsNumber);

    ASSERT_EQ(31, rgb1Handler->getSignedLong(0));
    ASSERT_EQ(31, rgb1Handler->getSignedLong(1));
    ASSERT_EQ(31, rgb1Handler->getSignedLong(2));

    ptr<image> rgb2(new image());
    rgb2->create(1, 1, image::depthS16, L"RGB", 12);
    ybr2rgb->runTransform(ybr, 0, 0, 1, 1, rgb2, 0, 0);
    ptr<handlers::dataHandlerNumericBase> rgb2Handler = rgb2->getDataHandler(false, &rowSize, &channelsPixelSize, &channelsNumber);

    ASSERT_EQ(4064, rgb2Handler->getSignedLong(0));
    ASSERT_EQ(4064, rgb2Handler->getSignedLong(1));
    ASSERT_EQ(4064, rgb2Handler->getSignedLong(2));

}

}

}

}
