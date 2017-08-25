#include "../library/imebra/include/imebra.h"

#include <vector>
#include <array>
#include <stdlib.h>
#include <gtest/gtest.h>

namespace puntoexe
{

namespace imebra
{

namespace tests
{


// A buffer initialized to a default data type should use the data type OB
TEST(memoryStreamTest, test)
{
	ptr<memory> myMemory(new memory);
	ptr<baseStream> theMemoryStream(new memoryStream(myMemory));
	ptr<streamWriter> writer(new streamWriter(theMemoryStream));

	std::vector<std::uint8_t> values(4000);
	for(size_t fillValues = 0; fillValues < values.size(); ++fillValues)
	{
		std::uint8_t value =  rand() * 255 / RAND_MAX;
		values[fillValues] = value;
		writer->write(&value, 1);
	}
	writer->flushDataBuffer();

	ptr<streamReader> reader(new streamReader(theMemoryStream));

	for(size_t readValues = 0; readValues < values.size(); ++readValues)
	{
		std::uint8_t value;
		reader->read(&value, 1);
        EXPECT_EQ(values[readValues], value);
	}
}

TEST(memoryStreamTest, testBytes)
{
	ptr<memory> myMemory(new memory);
	ptr<baseStream> theMemoryStream(new memoryStream(myMemory));
	ptr<streamWriter> writer(new streamWriter(theMemoryStream));
	writer->m_bJpegTags = true;

	std::vector<std::uint8_t> values(4000);
	for(size_t fillValues = 0; fillValues < values.size(); ++fillValues)
	{
		std::uint8_t value =  rand() * 255 / RAND_MAX;
		if(fillValues % 10 == 0)
		{
			value = 0xff;
		}
		values[fillValues] = value;
		writer->writeByte(value);
	}
	writer->flushDataBuffer();

	ptr<streamReader> reader(new streamReader(theMemoryStream));
	reader->m_bJpegTags = true;

	for(size_t readValues = 0; readValues < values.size(); ++readValues)
	{
		std::uint8_t value(reader->readByte());
        EXPECT_EQ(values[readValues], value);
	}
}

bool compareStreamContent(std::uint8_t* buffer, size_t streamPosition, size_t numBytes)
{
    for(size_t position(streamPosition); position != streamPosition + numBytes; ++position)
    {
        std::uint8_t value = (std::uint8_t)(position & 0xff);
        if(value != *buffer++)
        {
            return false;
        }
    }
    return true;
}

TEST(memoryStreamTest, testVirtualStream)
{
    ptr<memory> streamMemory(new memory(4098));

    // Fill the memory
    std::uint8_t* data = streamMemory->data();
    for(size_t fillMemory(0); fillMemory != streamMemory->size(); ++fillMemory)
    {
        data[fillMemory] = (std::uint8_t)(fillMemory & 0xff);
    }

    ptr<memoryStream> myStream(new memoryStream(streamMemory));

    ptr<streamReader> reader(new streamReader(myStream));
    ptr<streamReader> readerPosition95_20(new streamReader(myStream, 95, 20));

    std::vector<std::uint8_t> buffer10(10);
    reader->read(buffer10.data(), buffer10.size());
    ASSERT_TRUE(compareStreamContent(buffer10.data(), 0, buffer10.size()));

    ptr<streamReader> embeddedReader_10_60 = reader->getReader(60);
    reader->read(buffer10.data(), buffer10.size());
    ASSERT_TRUE(compareStreamContent(buffer10.data(), 70, buffer10.size()));

    embeddedReader_10_60->read(buffer10.data(), buffer10.size());
    ASSERT_TRUE(compareStreamContent(buffer10.data(), 10, buffer10.size()));

    readerPosition95_20->read(buffer10.data(), buffer10.size());
    ASSERT_TRUE(compareStreamContent(buffer10.data(), 95, buffer10.size()));
    ptr<streamReader> embeddedReader_105_10 = readerPosition95_20->getReader(10);
    embeddedReader_105_10->read(buffer10.data(), buffer10.size());
    ASSERT_TRUE(compareStreamContent(buffer10.data(), 105, buffer10.size()));
    ASSERT_THROW(readerPosition95_20->read(buffer10.data(), 1), streamExceptionEOF);
}


} // namespace tests

} // namespace imebra

} // namespace puntoexe
