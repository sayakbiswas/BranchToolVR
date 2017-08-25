#include <gtest/gtest.h>
#include "../library/imebra/include/imebra.h"

namespace puntoexe
{

namespace imebra
{

namespace tests
{

TEST(bufferTest, testDefaultType)
{
	ptr<buffer> patientBuffer(new buffer(ptr<baseObject>(0)));
    EXPECT_EQ(std::string("OB"), patientBuffer->getDataType());
}

TEST(bufferTest, testReadWrite)
{
	ptr<buffer> patientBuffer(new buffer(ptr<baseObject>(0), "UL"));

	// get a data reading handler
	ptr<handlers::dataHandler> readingHandler0 = patientBuffer->getDataHandler(false);

	// get a writing handler and write something
	{
		ptr<handlers::dataHandler> writingHandler0 = patientBuffer->getDataHandler(true);
		writingHandler0->setSize(10);
		for(int writeNumbers = 0; writeNumbers < 10; ++writeNumbers)
		{
			writingHandler0->setSignedLong(writeNumbers, writeNumbers);
		}
	}

	// now, the first reading handler should still be empty
    EXPECT_EQ(0, readingHandler0->getSize());

	// Get a new reading handler
	ptr<handlers::dataHandler> readingHandler1 = patientBuffer->getDataHandler(false);

	// Get two different writing handlers
	ptr<handlers::dataHandler> writingHandler1 = patientBuffer->getDataHandler(true);
	ptr<handlers::dataHandler> writingHandler2 = patientBuffer->getDataHandler(true);

	// Get another reading handler
	ptr<handlers::dataHandler> readingHandler2 = patientBuffer->getDataHandler(false);

	// Check the values in all the new handlers first
		for(std::int32_t checkValues = 0; checkValues < 10; ++checkValues)
	{
                EXPECT_EQ(checkValues, writingHandler1->getSignedLong(checkValues));
                EXPECT_EQ(checkValues, writingHandler2->getSignedLong(checkValues));
                EXPECT_EQ(checkValues, readingHandler1->getSignedLong(checkValues));
                EXPECT_EQ(checkValues, readingHandler2->getSignedLong(checkValues));
	}

	// Change the content in the first writing handler
	writingHandler1->setSize(20);
    EXPECT_EQ(20, writingHandler1->getSize());
	for(int writeValues = 0; writeValues < 20; ++writeValues)
	{
		writingHandler1->setSignedLong(writeValues, writeValues+100);
	}

	// Check the other data handlers. They should still have the original values
    for(std::int32_t checkValues = 0; checkValues < 10; ++checkValues)
	{
        EXPECT_EQ(writingHandler2->getSignedLong(checkValues), checkValues);
        EXPECT_EQ(readingHandler1->getSignedLong(checkValues), checkValues);
        EXPECT_EQ(readingHandler2->getSignedLong(checkValues), checkValues);
	}

	// Now modify the second writing handler
	writingHandler2->setSize(5);
    EXPECT_EQ(5, writingHandler2->getSize());
	for(int writeValues = 0; writeValues < 5; ++writeValues)
	{
		writingHandler2->setSignedLong(writeValues, writeValues+200);
	}

	// Release the first writing handler, then recheck the values in the other handlers
	writingHandler1 = ptr<handlers::dataHandler>(0);
    EXPECT_EQ(10, readingHandler1->getSize());
    EXPECT_EQ(10, readingHandler2->getSize());
    for(std::int32_t checkValues = 0; checkValues < 10; ++checkValues)
	{
        EXPECT_EQ(checkValues, readingHandler1->getSignedLong(checkValues));
        EXPECT_EQ(checkValues, readingHandler2->getSignedLong(checkValues));
	}
    EXPECT_EQ(5, writingHandler2->getSize());
    for(std::int32_t checkValues = 0; checkValues < 5; ++checkValues)
	{
        EXPECT_EQ(checkValues + std::int32_t(200), writingHandler2->getSignedLong(checkValues));
	}

	// Get a reading handler. It should have the value written by writingHandler1
	ptr<handlers::dataHandler> readingHandler3 = patientBuffer->getDataHandler(false);
    EXPECT_EQ(20, readingHandler3->getSize());
    for(std::int32_t checkValues = 0; checkValues < 20; ++checkValues)
	{
        EXPECT_EQ(checkValues + std::int32_t(100), readingHandler3->getSignedLong(checkValues));
	}

	// Release a reading handler. It shouldn't change the values in the buffer
	readingHandler1 = ptr<handlers::dataHandler>(0);
	ptr<handlers::dataHandler> readingHandler4 = patientBuffer->getDataHandler(false);
    EXPECT_EQ(20, readingHandler4->getSize());
    for(std::int32_t checkValues = 0; checkValues < 20; ++checkValues)
	{
        EXPECT_EQ(checkValues + std::int32_t(100), readingHandler4->getSignedLong(checkValues));
	}

	// Release the second writing handler. It should change the buffer, but already
	//  existing handlers should continue with their values
	writingHandler2 = ptr<handlers::dataHandler>(0);
	ptr<handlers::dataHandler> writingHandler3 = patientBuffer->getDataHandler(true);
    EXPECT_EQ(5, writingHandler3->getSize());
    for(std::int32_t checkValues = 0; checkValues < 5; ++checkValues)
	{
        EXPECT_EQ(checkValues + std::int32_t(200), writingHandler3->getSignedLong(checkValues));
	}
	writingHandler3 = ptr<handlers::dataHandler>(0);

	// ReadingHandler2 still exist. Check its values
    EXPECT_EQ(10, readingHandler2->getSize());
    for(std::int32_t checkValues = 0; checkValues < 10; ++checkValues)
	{
        EXPECT_EQ(checkValues, readingHandler2->getSignedLong(checkValues));
	}
	readingHandler2 = ptr<handlers::dataHandler>(0);

	// Get a reading handler and check it. It should have the values of writingHandler2
	ptr<handlers::dataHandler> readingHandler5 = patientBuffer->getDataHandler(false);
    EXPECT_EQ(5, readingHandler5->getSize());
    for(std::int32_t checkValues = 0; checkValues < 5; ++checkValues)
	{
        EXPECT_EQ(checkValues + std::int32_t(200), readingHandler5->getSignedLong(checkValues));
	}
}

TEST(bufferTest, testOddLength)
{
	ptr<buffer> patientBuffer(new buffer(ptr<baseObject>(0), "OB"));
	ptr<handlers::dataHandler> writingHandler = patientBuffer->getDataHandler(true);
	
	// Write 3 numbers
	writingHandler->setSize(3);
	writingHandler->setUnsignedLong(0, 10);
	writingHandler->setUnsignedLong(1, 20);
	writingHandler->setUnsignedLong(2, 30);
	writingHandler = ptr<handlers::dataHandler>(0);

	ptr<handlers::dataHandler> readingHandler = patientBuffer->getDataHandler(false);
    EXPECT_EQ(4, readingHandler->getSize());
    EXPECT_EQ(10, readingHandler->getUnsignedLong(0));
    EXPECT_EQ(20, readingHandler->getUnsignedLong(1));
    EXPECT_EQ(30, readingHandler->getUnsignedLong(2));
    EXPECT_EQ(0, readingHandler->getUnsignedLong(3));
}

} // namespace tests

} // namespace imebra

} // namespace puntoexe
