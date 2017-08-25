#include "../library/imebra/include/imebra.h"
#include <gtest/gtest.h>

namespace puntoexe
{

namespace imebra
{

namespace tests
{

using namespace puntoexe::imebra;

TEST(dicomDirTest, createDicomDir)
{
	ptr<dicomDir> newDicomDir(new dicomDir(new dataSet));

	ptr<directoryRecord> pRootRecord(newDicomDir->getNewRecord());
	pRootRecord->getRecordDataSet()->setUnicodeString(0x10, 0, 0x10, 0, L"Surname");
	pRootRecord->setType(directoryRecord::patient);
	newDicomDir->setFirstRootRecord(pRootRecord);

	ptr<directoryRecord> pNextRecord(newDicomDir->getNewRecord());
	pNextRecord->getRecordDataSet()->setUnicodeString(0x10, 0, 0x10, 0, L"Surname 1");
	pNextRecord->setType(directoryRecord::patient);
	pRootRecord->setNextRecord(pNextRecord);

	ptr<directoryRecord> pImageRecord(newDicomDir->getNewRecord());
	pImageRecord->getRecordDataSet()->setUnicodeString(0x8, 0, 0x18, 0, L"1.2.840.34.56.78999654.235");
	pImageRecord->setType(directoryRecord::image);
        pImageRecord->setFilePart(0, L"folder");
        pImageRecord->setFilePart(1, L"file.dcm");

	pNextRecord->setFirstChildRecord(pImageRecord);

	ptr<dataSet> dicomDirDataSet(newDicomDir->buildDataSet());

	ptr<memory> streamMemory(new memory);
	ptr<memoryStream> memStream(new memoryStream(streamMemory));
	ptr<streamWriter> memWriter(new streamWriter(memStream));
	ptr<imebra::codecs::dicomCodec> dicomWriter(new imebra::codecs::dicomCodec);
	dicomWriter->write(memWriter, dicomDirDataSet);
	memWriter.release();

	ptr<streamReader> memReader(new streamReader(memStream));
	ptr<imebra::codecs::dicomCodec> dicomReader(new imebra::codecs::dicomCodec);
	ptr<dataSet> readDataSet(dicomReader->read(memReader));

	ptr<dicomDir> testDicomDir(new dicomDir(readDataSet));
	ptr<directoryRecord> testRootRecord(testDicomDir->getFirstRootRecord());
    EXPECT_EQ(directoryRecord::patient, testRootRecord->getType());
    EXPECT_EQ(std::wstring(L"Surname"), testRootRecord->getRecordDataSet()->getUnicodeString(0x10, 0, 0x10, 0));

	ptr<directoryRecord> testNextRecord(testRootRecord->getNextRecord());
    EXPECT_EQ(directoryRecord::patient, testNextRecord->getType());
    EXPECT_EQ(std::wstring(L"Surname 1"), testNextRecord->getRecordDataSet()->getUnicodeString(0x10, 0, 0x10, 0));

	ptr<directoryRecord> testImageRecord(testNextRecord->getFirstChildRecord());
    EXPECT_EQ(directoryRecord::image, testImageRecord->getType());
    EXPECT_EQ(std::wstring(L"1.2.840.34.56.78999654.235"), testImageRecord->getRecordDataSet()->getUnicodeString(0x8, 0, 0x18, 0));
    EXPECT_EQ(std::wstring(L"folder"), testImageRecord->getFilePart(0));
    EXPECT_EQ(std::wstring(L"file.dcm"), testImageRecord->getFilePart(1));
}


} // namespace tests

} // namespace imebra

} // namespace puntoexe
