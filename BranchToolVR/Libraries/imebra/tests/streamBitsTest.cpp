#include "../library/imebra/include/imebra.h"

#include <vector>
#include <cstdlib>
#include <gtest/gtest.h>

namespace puntoexe
{

namespace imebra
{

namespace tests
{

TEST(streamBitsTest, adjustEndianessTest0)
{
    std::uint16_t uint16Numbers[3];
    uint16Numbers[0] = 0xfafb;
    uint16Numbers[1] = 0xfcfd;
    uint16Numbers[2] = 0xfeff;

    streamController::adjustEndian((std::uint8_t*)uint16Numbers, 2, streamController::lowByteEndian, 3);

    EXPECT_EQ(0xfb, ((std::uint8_t*)uint16Numbers)[0]);
    EXPECT_EQ(0xfa, ((std::uint8_t*)uint16Numbers)[1]);
    EXPECT_EQ(0xfd, ((std::uint8_t*)uint16Numbers)[2]);
    EXPECT_EQ(0xfc, ((std::uint8_t*)uint16Numbers)[3]);
    EXPECT_EQ(0xff, ((std::uint8_t*)uint16Numbers)[4]);
    EXPECT_EQ(0xfe, ((std::uint8_t*)uint16Numbers)[5]);

    std::uint16_t uint16Number = streamController::adjustEndian((std::uint16_t)0xfafb, streamController::lowByteEndian);
    EXPECT_EQ(0xfb, ((std::uint8_t*)&uint16Number)[0]);
    EXPECT_EQ(0xfa, ((std::uint8_t*)&uint16Number)[1]);
}

TEST(streamBitsTest, adjustEndianessTest1)
{
    std::uint16_t uint16Numbers[3];
    uint16Numbers[0] = 0xfafb;
    uint16Numbers[1] = 0xfcfd;
    uint16Numbers[2] = 0xfeff;

    streamController::adjustEndian((std::uint8_t*)uint16Numbers, 2, streamController::highByteEndian, 3);

    EXPECT_EQ(0xfa, ((std::uint8_t*)uint16Numbers)[0]);
    EXPECT_EQ(0xfb, ((std::uint8_t*)uint16Numbers)[1]);
    EXPECT_EQ(0xfc, ((std::uint8_t*)uint16Numbers)[2]);
    EXPECT_EQ(0xfd, ((std::uint8_t*)uint16Numbers)[3]);
    EXPECT_EQ(0xfe, ((std::uint8_t*)uint16Numbers)[4]);
    EXPECT_EQ(0xff, ((std::uint8_t*)uint16Numbers)[5]);

    std::uint16_t uint16Number = streamController::adjustEndian((std::uint16_t)0xfafb, streamController::highByteEndian);
    EXPECT_EQ(0xfa, ((std::uint8_t*)&uint16Number)[0]);
    EXPECT_EQ(0xfb, ((std::uint8_t*)&uint16Number)[1]);
}

TEST(streamBitsTest, adjustEndianessTest2)
{
    std::uint32_t uint32Numbers[2];
    uint32Numbers[0] = 0xaaabacad;
    uint32Numbers[1] = 0xbabbbcbd;

    streamController::adjustEndian((std::uint8_t*)uint32Numbers, 4, streamController::lowByteEndian, 2);

    EXPECT_EQ(0xad, ((std::uint8_t*)uint32Numbers)[0]);
    EXPECT_EQ(0xac, ((std::uint8_t*)uint32Numbers)[1]);
    EXPECT_EQ(0xab, ((std::uint8_t*)uint32Numbers)[2]);
    EXPECT_EQ(0xaa, ((std::uint8_t*)uint32Numbers)[3]);
    EXPECT_EQ(0xbd, ((std::uint8_t*)uint32Numbers)[4]);
    EXPECT_EQ(0xbc, ((std::uint8_t*)uint32Numbers)[5]);
    EXPECT_EQ(0xbb, ((std::uint8_t*)uint32Numbers)[6]);
    EXPECT_EQ(0xba, ((std::uint8_t*)uint32Numbers)[7]);

    std::uint32_t uint32Number = streamController::adjustEndian((std::uint32_t)0xfafbfcfd, streamController::lowByteEndian);
    EXPECT_EQ(0xfd, ((std::uint8_t*)&uint32Number)[0]);
    EXPECT_EQ(0xfc, ((std::uint8_t*)&uint32Number)[1]);
    EXPECT_EQ(0xfb, ((std::uint8_t*)&uint32Number)[2]);
    EXPECT_EQ(0xfa, ((std::uint8_t*)&uint32Number)[3]);
}

TEST(streamBitsTest, adjustEndianessTest3)
{
    std::uint32_t uint32Numbers[2];
    uint32Numbers[0] = 0xaaabacad;
    uint32Numbers[1] = 0xbabbbcbd;

    streamController::adjustEndian((std::uint8_t*)uint32Numbers, 4, streamController::highByteEndian, 2);

    EXPECT_EQ(0xaa, ((std::uint8_t*)uint32Numbers)[0]);
    EXPECT_EQ(0xab, ((std::uint8_t*)uint32Numbers)[1]);
    EXPECT_EQ(0xac, ((std::uint8_t*)uint32Numbers)[2]);
    EXPECT_EQ(0xad, ((std::uint8_t*)uint32Numbers)[3]);
    EXPECT_EQ(0xba, ((std::uint8_t*)uint32Numbers)[4]);
    EXPECT_EQ(0xbb, ((std::uint8_t*)uint32Numbers)[5]);
    EXPECT_EQ(0xbc, ((std::uint8_t*)uint32Numbers)[6]);
    EXPECT_EQ(0xbd, ((std::uint8_t*)uint32Numbers)[7]);

    std::uint32_t uint32Number = streamController::adjustEndian((std::uint32_t)0xfafbfcfd, streamController::highByteEndian);
    EXPECT_EQ(0xfa, ((std::uint8_t*)&uint32Number)[0]);
    EXPECT_EQ(0xfb, ((std::uint8_t*)&uint32Number)[1]);
    EXPECT_EQ(0xfc, ((std::uint8_t*)&uint32Number)[2]);
    EXPECT_EQ(0xfd, ((std::uint8_t*)&uint32Number)[3]);
}

TEST(streamBitsTest, adjustEndianessTest4)
{
    std::uint64_t uint64Numbers[2];
    uint64Numbers[0] = 0xaaabacadaeafb0b1;
    uint64Numbers[1] = 0xbabbbcbdbebfc0c1;

    streamController::adjustEndian((std::uint8_t*)uint64Numbers, 8, streamController::lowByteEndian, 2);

    EXPECT_EQ(0xb1, ((std::uint8_t*)uint64Numbers)[0]);
    EXPECT_EQ(0xb0, ((std::uint8_t*)uint64Numbers)[1]);
    EXPECT_EQ(0xaf, ((std::uint8_t*)uint64Numbers)[2]);
    EXPECT_EQ(0xae, ((std::uint8_t*)uint64Numbers)[3]);
    EXPECT_EQ(0xad, ((std::uint8_t*)uint64Numbers)[4]);
    EXPECT_EQ(0xac, ((std::uint8_t*)uint64Numbers)[5]);
    EXPECT_EQ(0xab, ((std::uint8_t*)uint64Numbers)[6]);
    EXPECT_EQ(0xaa, ((std::uint8_t*)uint64Numbers)[7]);
    EXPECT_EQ(0xc1, ((std::uint8_t*)uint64Numbers)[8]);
    EXPECT_EQ(0xc0, ((std::uint8_t*)uint64Numbers)[9]);
    EXPECT_EQ(0xbf, ((std::uint8_t*)uint64Numbers)[10]);
    EXPECT_EQ(0xbe, ((std::uint8_t*)uint64Numbers)[11]);
    EXPECT_EQ(0xbd, ((std::uint8_t*)uint64Numbers)[12]);
    EXPECT_EQ(0xbc, ((std::uint8_t*)uint64Numbers)[13]);
    EXPECT_EQ(0xbb, ((std::uint8_t*)uint64Numbers)[14]);
    EXPECT_EQ(0xba, ((std::uint8_t*)uint64Numbers)[15]);

    std::uint64_t uint64Number = streamController::adjustEndian((std::uint64_t)0xfafbfcfdfeff0001, streamController::lowByteEndian);
    EXPECT_EQ(0x01, ((std::uint8_t*)&uint64Number)[0]);
    EXPECT_EQ(0x00, ((std::uint8_t*)&uint64Number)[1]);
    EXPECT_EQ(0xff, ((std::uint8_t*)&uint64Number)[2]);
    EXPECT_EQ(0xfe, ((std::uint8_t*)&uint64Number)[3]);
    EXPECT_EQ(0xfd, ((std::uint8_t*)&uint64Number)[4]);
    EXPECT_EQ(0xfc, ((std::uint8_t*)&uint64Number)[5]);
    EXPECT_EQ(0xfb, ((std::uint8_t*)&uint64Number)[6]);
    EXPECT_EQ(0xfa, ((std::uint8_t*)&uint64Number)[7]);
}

TEST(streamBitsTest, adjustEndianessTest5)
{
    std::uint64_t uint64Numbers[2];
    uint64Numbers[0] = 0xaaabacadaeafb0b1;
    uint64Numbers[1] = 0xbabbbcbdbebfc0c1;

    streamController::adjustEndian((std::uint8_t*)uint64Numbers, 8, streamController::highByteEndian, 2);

    EXPECT_EQ(0xaa, ((std::uint8_t*)uint64Numbers)[0]);
    EXPECT_EQ(0xab, ((std::uint8_t*)uint64Numbers)[1]);
    EXPECT_EQ(0xac, ((std::uint8_t*)uint64Numbers)[2]);
    EXPECT_EQ(0xad, ((std::uint8_t*)uint64Numbers)[3]);
    EXPECT_EQ(0xae, ((std::uint8_t*)uint64Numbers)[4]);
    EXPECT_EQ(0xaf, ((std::uint8_t*)uint64Numbers)[5]);
    EXPECT_EQ(0xb0, ((std::uint8_t*)uint64Numbers)[6]);
    EXPECT_EQ(0xb1, ((std::uint8_t*)uint64Numbers)[7]);
    EXPECT_EQ(0xba, ((std::uint8_t*)uint64Numbers)[8]);
    EXPECT_EQ(0xbb, ((std::uint8_t*)uint64Numbers)[9]);
    EXPECT_EQ(0xbc, ((std::uint8_t*)uint64Numbers)[10]);
    EXPECT_EQ(0xbd, ((std::uint8_t*)uint64Numbers)[11]);
    EXPECT_EQ(0xbe, ((std::uint8_t*)uint64Numbers)[12]);
    EXPECT_EQ(0xbf, ((std::uint8_t*)uint64Numbers)[13]);
    EXPECT_EQ(0xc0, ((std::uint8_t*)uint64Numbers)[14]);
    EXPECT_EQ(0xc1, ((std::uint8_t*)uint64Numbers)[15]);

    std::uint64_t uint64Number = streamController::adjustEndian((std::uint64_t)0xfafbfcfdfeff0001, streamController::highByteEndian);
    EXPECT_EQ(0xfa, ((std::uint8_t*)&uint64Number)[0]);
    EXPECT_EQ(0xfb, ((std::uint8_t*)&uint64Number)[1]);
    EXPECT_EQ(0xfc, ((std::uint8_t*)&uint64Number)[2]);
    EXPECT_EQ(0xfd, ((std::uint8_t*)&uint64Number)[3]);
    EXPECT_EQ(0xfe, ((std::uint8_t*)&uint64Number)[4]);
    EXPECT_EQ(0xff, ((std::uint8_t*)&uint64Number)[5]);
    EXPECT_EQ(0x00, ((std::uint8_t*)&uint64Number)[6]);
    EXPECT_EQ(0x01, ((std::uint8_t*)&uint64Number)[7]);
}


TEST(streamBitsTest, test)
{
	std::srand(100);
	ptr<memory> myMemory(new memory);
	ptr<baseStream> theMemoryStream(new memoryStream(myMemory));
	ptr<streamWriter> writer(new streamWriter(theMemoryStream));
	
	writer->m_bJpegTags = true;

	std::vector<std::uint8_t> bitsNumber(4000);
	std::vector<std::uint32_t> bitsValue(4000);
	for(size_t fillValues = 0; fillValues < bitsValue.size(); ++fillValues)
	{
		std::uint8_t bits =  rand() * 16 / RAND_MAX;
		if(bits == 0) bits = 4;
		std::uint32_t maxValue = (1 << bits) - 1;
		std::uint32_t value = std::rand() * maxValue / RAND_MAX;
		bitsNumber[fillValues] = bits;
		bitsValue[fillValues] = value;
		writer->writeBits(value, bits);
	}
	writer->resetOutBitsBuffer();
	writer->flushDataBuffer();

	{
		ptr<streamReader> reader(new streamReader(theMemoryStream));
		reader->m_bJpegTags = true;

		for(size_t readValues = 0; readValues < bitsValue.size(); ++readValues)
		{
			std::uint32_t value(reader->readBits(bitsNumber[readValues]));
            EXPECT_EQ(bitsValue[readValues], value);
		}
	}

	{
		ptr<streamReader> reader(new streamReader(theMemoryStream));
		reader->m_bJpegTags = true;

		for(size_t readValues = 0; readValues < bitsValue.size(); ++readValues)
		{
			std::uint32_t value(0);
			for(std::uint8_t count(bitsNumber[readValues]); count != 0; --count)
			{
				value <<= 1;
				value |= reader->readBit();
			}
            EXPECT_EQ(bitsValue[readValues], value);
		}
	}

	{
		ptr<streamReader> reader(new streamReader(theMemoryStream));
		reader->m_bJpegTags = true;

		for(size_t readValues = 0; readValues < bitsValue.size(); ++readValues)
		{
			std::uint32_t value(0);
			for(std::uint8_t count(bitsNumber[readValues]); count != 0; --count)
			{
				reader->addBit(&value);
			}
            EXPECT_EQ(bitsValue[readValues], value);
		}
	}

}




} // namespace tests

} // namespace imebra

} // namespace puntoexe
