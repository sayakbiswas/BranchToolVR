#include "../library/imebra/include/imebra.h"

#include <vector>
#include <stdlib.h>
#include <gtest/gtest.h>

namespace puntoexe
{

namespace imebra
{

namespace tests
{

// A buffer initialized to a default data type should use the data type OB
TEST(huffmanTest, test)
{
	ptr<memory> myMemory(new memory);
	ptr<baseStream> theMemoryStream(new memoryStream(myMemory));
	ptr<streamWriter> writer(new streamWriter(theMemoryStream));

	huffmanTable huffman(8);

	std::vector<std::uint8_t> values(4000);
	for(size_t fillValues = 0; fillValues < values.size(); ++fillValues)
	{
		std::uint8_t value =  (std::uint8_t)(rand() * (double)255 / (double)RAND_MAX);
		values[fillValues] = value;
		huffman.incValueFreq(values[fillValues]);
	}
	huffman.calcHuffmanCodesLength(16);
	huffman.calcHuffmanTables();

	for(size_t writeValues = 0; writeValues < values.size(); ++writeValues)
	{
		std::uint32_t value = values[writeValues];
		huffman.writeHuffmanCode(value, writer.get());
	}
	writer->resetOutBitsBuffer();

	ptr<streamReader> reader(new streamReader(theMemoryStream));
	for(size_t readValues = 0; readValues < values.size(); ++readValues)
	{
		std::uint32_t value(huffman.readHuffmanCode(reader.get()));

        EXPECT_EQ(values[readValues], (std::uint8_t)value);
	}
}



} // namespace tests

} // namespace imebra

} // namespace puntoexe
