#include "../library/imebra/include/imebra.h"
#include <string.h>
#include <memory>

#include <gtest/gtest.h>

namespace puntoexe
{

namespace imebra
{

namespace tests
{

// Check the function pointerIsValid
TEST(numericHandlerTest, validPointer)
{
	ptr<buffer> buffer0(new buffer(0, "OW"));


	ptr<handlers::dataHandler> handlerBuffer0(buffer0->getDataHandler(true, 0));
    EXPECT_EQ(2, handlerBuffer0->getUnitSize());
	std::uint32_t bufferSize(4);
	handlerBuffer0->setSize(bufferSize);
	
	std::uint32_t checkSize0(0);
	while(handlerBuffer0->pointerIsValid(checkSize0))
	{
        EXPECT_GE(bufferSize, checkSize0);
		++checkSize0;
	}

    EXPECT_EQ(bufferSize, checkSize0);
	handlerBuffer0.release();

	ptr<handlers::dataHandler> handlerBuffer1(buffer0->getDataHandler(true, 0));
    EXPECT_EQ(2, handlerBuffer1->getUnitSize());
	
	std::uint32_t checkSize1(0);
	while(handlerBuffer1->pointerIsValid(checkSize1))
	{
        EXPECT_GE(bufferSize, checkSize1);
		++checkSize1;
	}

    EXPECT_EQ(bufferSize, checkSize1);
}

// A buffer initialized to a default data type should use the data type OB
TEST(numericHandlerTest, interleavedCopy)
{
	int sizeX = 601;
	int sizeY = 401;
	ptr<image> testImage(new image);
	testImage->create(sizeX, sizeY, image::depthS16, L"RGB", 7);
	std::uint32_t rowSize, channelSize, channelsNumber;
	ptr<handlers::dataHandlerNumericBase> testHandler = testImage->getDataHandler(true, &rowSize, &channelSize, &channelsNumber);

	size_t pointer(0);
	for(int y = 0; y < sizeY; ++y)
	{
		for(int x = 0; x < sizeX; ++x)
		{
			int squareX = x >> 3;
			int squareY = y >> 3;

			int squareX2 = squareX >> 1;
			int squareY2 = squareY >> 1;

			testHandler->setSignedLong(pointer++, squareY * 100 + squareX);
			testHandler->setSignedLong(pointer++, -squareY2 * 100 - squareX2);
			testHandler->setSignedLong(pointer++, -squareY2 * 100 + squareX2);
		}
	}

	int mcuX = (sizeX + 15) >> 4;
	int mcuY = (sizeY + 15) >> 4;

    std::vector<std::int32_t> buffer0((size_t)(mcuX * 16 * mcuY * 16));
    ::memset(buffer0.data(), 0, buffer0.size() * sizeof(std::int32_t));

    std::vector<std::int32_t> buffer1((size_t)(mcuX * 8 * mcuY * 8));
    ::memset(buffer1.data(), 0, buffer1.size() * sizeof(std::int32_t));

    std::vector<std::int32_t> buffer2((size_t)(mcuX * 8 * mcuY * 8));
    ::memset(buffer2.data(), 0, buffer2.size() * sizeof(std::int32_t));

	for(int scanMcuY = 0; scanMcuY < mcuY; ++scanMcuY)
	{
		for(int scanMcuX = 0; scanMcuX < mcuX; ++scanMcuX)
		{
			testHandler->copyToInt32Interleaved(
                buffer0.data() + (128*scanMcuX) + (256*mcuX*scanMcuY),
				1,
				1,
				scanMcuX * 16,
				scanMcuY * 16,
				scanMcuX * 16 + 8,
				scanMcuY * 16 + 8,
				0,
				sizeX,
				sizeY,
				3);
			
			testHandler->copyToInt32Interleaved(
                buffer0.data() + 64 + (128*scanMcuX) + (256*mcuX*scanMcuY),
				1,
				1,
				scanMcuX * 16 + 8,
				scanMcuY * 16,
				scanMcuX * 16 + 16,
				scanMcuY * 16 + 8,
				0,
				sizeX,
				sizeY,
				3);
			
			testHandler->copyToInt32Interleaved(
                buffer0.data() + (128*scanMcuX) + (256*mcuX*scanMcuY) + 128 * mcuX,
				1,
				1,
				scanMcuX * 16,
				scanMcuY * 16 + 8,
				scanMcuX * 16 + 8,
				scanMcuY * 16 + 16,
				0,
				sizeX,
				sizeY,
				3);
			
			testHandler->copyToInt32Interleaved(
                buffer0.data() + 64 + (128*scanMcuX) + (256*mcuX*scanMcuY) + 128 * mcuX,
				1,
				1,
				scanMcuX * 16 + 8,
				scanMcuY * 16 + 8,
				scanMcuX * 16 + 16,
				scanMcuY * 16 + 16,
				0,
				sizeX,
				sizeY,
				3);
			
			
			testHandler->copyToInt32Interleaved(
                buffer1.data() + (64*scanMcuX) + (64*mcuX*scanMcuY),
				2,
				2,
				scanMcuX * 16,
				scanMcuY * 16,
				scanMcuX * 16 + 16,
				scanMcuY * 16 + 16,
				1,
				sizeX,
				sizeY,
				3);
			
			testHandler->copyToInt32Interleaved(
                buffer2.data() + (64*scanMcuX) + (64*mcuX*scanMcuY),
				2,
				2,
				scanMcuX * 16,
				scanMcuY * 16,
				scanMcuX * 16 + 16,
				scanMcuY * 16 + 16,
				2,
				sizeX,
				sizeY,
				3);
		
		}
	}

	for(int y = 0; y < sizeY; y+=8)
	{
		for(int x = 0; x < sizeX; x+=8)
		{
			int squareX = x >> 3;
			int squareY = y >> 3;

			int squareX2 = squareX >> 1;
			int squareY2 = squareY >> 1;

            std::int32_t* pTestBuffer0 = buffer0.data() + squareX * 64 + squareY * mcuX * 128;
			for(int scanValues0 = 0; scanValues0 < 64; ++scanValues0)
			{
                EXPECT_EQ((std::int32_t)(squareY * 100 + squareX), pTestBuffer0[scanValues0]);
			}

            std::int32_t* pTestBuffer1 = buffer1.data() + squareX2 * 64 + squareY2 * mcuX * 64;
			for(int scanValues1 = 0; scanValues1 < 64; ++scanValues1)
			{
                EXPECT_EQ((std::int32_t)(-squareY2 * 100 - squareX2), pTestBuffer1[scanValues1]);
			}

            std::int32_t* pTestBuffer2 = buffer2.data() + squareX2 * 64 + squareY2 * mcuX * 64;
			for(int scanValues2 = 0; scanValues2 < 64; ++scanValues2)
			{
                EXPECT_EQ((std::int32_t)(-squareY2 * 100 + squareX2), pTestBuffer2[scanValues2]);
			}

		}
	}

}

TEST(numericHandlerTest, memcopy)
{
    ptr<memory> handlerBuffer(new memory());
    ptr<handlers::dataHandlerNumeric<std::int32_t> > testHandler(new handlers::dataHandlerNumeric<std::int32_t>);
    testHandler->parseBuffer(handlerBuffer);

    size_t size = 121;
    testHandler->setSize(121);
    for(size_t scanElements(0); scanElements != size; ++scanElements)
    {
        testHandler->setUnsignedLong(scanElements, scanElements * 2);
    }

    ptr<handlers::dataHandlerNumeric<std::uint16_t> > testHandlerDest(new handlers::dataHandlerNumeric<std::uint16_t>);
    testHandlerDest->parseBuffer(new memory());
    testHandlerDest->copyFrom(testHandler);

    testHandler->setUnsignedLong(0, 128);

    for(size_t scanElements(0); scanElements != size; ++scanElements)
    {
        EXPECT_EQ(scanElements * 2, testHandlerDest->getUnsignedLong(scanElements));
    }


}

TEST(numericHandlerTest, stringConversion)
{
    {
        ptr<memory> handlerBuffer(new memory);
        ptr<handlers::dataHandlerNumeric<std::int32_t> > testHandler(new handlers::dataHandlerNumeric<std::int32_t>);
        testHandler->parseBuffer(handlerBuffer);
        testHandler->setSize(2);

        testHandler->setUnicodeString(0, L"1300000000");
        testHandler->setString(1, "14");
        EXPECT_EQ(1300000000, testHandler->getSignedLong(0));
        EXPECT_EQ(14, testHandler->getSignedLong(1));
        EXPECT_EQ(L"1300000000", testHandler->getUnicodeString(0));
        EXPECT_EQ("1300000000", testHandler->getString(0));
        EXPECT_EQ(L"14", testHandler->getUnicodeString(1));
        EXPECT_EQ("14", testHandler->getString(1));

        testHandler->setUnicodeString(0, L"45.7");
        EXPECT_EQ(45, testHandler->getSignedLong(0));
    }

    {
        ptr<memory> handlerBuffer(new memory);
        ptr<handlers::dataHandlerNumeric<double> > testHandler(new handlers::dataHandlerNumeric<double>);
        testHandler->parseBuffer(handlerBuffer);
        testHandler->setSize(2);

        testHandler->setUnicodeString(0, L"13");
        testHandler->setString(1, "14");
        EXPECT_FLOAT_EQ(13, testHandler->getSignedLong(0));
        EXPECT_FLOAT_EQ(14, testHandler->getSignedLong(1));
        EXPECT_EQ(L"13", testHandler->getUnicodeString(0));
        EXPECT_EQ(L"14", testHandler->getUnicodeString(1));
        testHandler->setUnicodeString(0, L"45.7");
        testHandler->setString(1, "46.7");
        EXPECT_FLOAT_EQ(45.7, testHandler->getDouble(0));
        EXPECT_FLOAT_EQ(46.7, testHandler->getDouble(1));
        EXPECT_EQ(L"45.7", testHandler->getUnicodeString(0));
        EXPECT_EQ(L"46.7", testHandler->getUnicodeString(1));
    }

    {
        ptr<memory> handlerBuffer(new memory);
        ptr<handlers::dataHandlerNumeric<std::uint8_t> > testHandler(new handlers::dataHandlerNumeric<std::uint8_t>);
        testHandler->parseBuffer(handlerBuffer);
        testHandler->setSize(2);

        EXPECT_FALSE(testHandler->isSigned());

        testHandler->setUnicodeString(0, L"240");
        testHandler->setString(1, "14");
        EXPECT_FLOAT_EQ(240, testHandler->getSignedLong(0));
        EXPECT_FLOAT_EQ(14, testHandler->getSignedLong(1));
        EXPECT_EQ(L"240", testHandler->getUnicodeString(0));
        EXPECT_EQ(L"14", testHandler->getUnicodeString(1));
        testHandler->setUnicodeString(0, L"45.7");
        testHandler->setString(1, "46.7");
        EXPECT_FLOAT_EQ(45, testHandler->getDouble(0));
        EXPECT_FLOAT_EQ(46, testHandler->getDouble(1));
        EXPECT_EQ(L"45", testHandler->getUnicodeString(0));
        EXPECT_EQ(L"46", testHandler->getUnicodeString(1));
    }

    {
        ptr<memory> handlerBuffer(new memory);
        ptr<handlers::dataHandlerNumeric<std::int8_t> > testHandler(new handlers::dataHandlerNumeric<std::int8_t>);
        testHandler->parseBuffer(handlerBuffer);
        testHandler->setSize(2);

        EXPECT_TRUE(testHandler->isSigned());

        testHandler->setUnicodeString(0, L"-2");
        testHandler->setString(1, "14");
        EXPECT_FLOAT_EQ(-2, testHandler->getSignedLong(0));
        EXPECT_FLOAT_EQ(14, testHandler->getSignedLong(1));
        EXPECT_EQ(L"-2", testHandler->getUnicodeString(0));
        EXPECT_EQ(L"14", testHandler->getUnicodeString(1));
        testHandler->setUnicodeString(0, L"45.7");
        testHandler->setString(1, "46.7");
        EXPECT_FLOAT_EQ(45, testHandler->getDouble(0));
        EXPECT_FLOAT_EQ(46, testHandler->getDouble(1));
        EXPECT_EQ(L"45", testHandler->getUnicodeString(0));
        EXPECT_EQ(L"46", testHandler->getUnicodeString(1));
    }
}



} // namespace tests

} // namespace imebra

} // namespace puntoexe
