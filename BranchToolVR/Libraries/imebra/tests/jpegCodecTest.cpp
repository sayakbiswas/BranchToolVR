#include "../library/imebra/include/imebra.h"
#include <gtest/gtest.h>

namespace puntoexe
{

namespace imebra
{

namespace tests
{

// A buffer initialized to a default data type should use the data type OB
TEST(jpegCodecTest, testBaseline)
{
	for(int precision=0; precision != 2; ++precision)
	{
		ptr<imebra::dataSet> dataset(new imebra::dataSet);

		std::uint32_t sizeX = 600;
		std::uint32_t sizeY = 400;
		ptr<image> baselineImage(new image);
		baselineImage->create(sizeX, sizeY, precision == 0 ? image::depthU8 : image::depthU16, L"RGB", precision == 0 ? 7 : 11);

		std::uint32_t rowSize, channelsPixelSize, channelsNumber;
		ptr<handlers::dataHandlerNumericBase> imageHandler = baselineImage->getDataHandler(true, &rowSize, &channelsPixelSize, &channelsNumber);

		// Make 3 bands (RGB)
		int elementPointer(0);
		for(std::uint32_t y=0; y<sizeY; ++y)
		{
			for(std::uint32_t x=0; x<sizeX; ++x)
			{
				std::int32_t r, g, b;
				std::uint32_t value = y * (precision == 0 ? 255 : 4095) / sizeY;
				r = g = 0;
				b = value;
				if(x < sizeX - sizeX/3)
				{
					r = 0;
					g = value;
					b = 0;
				}
				if(x < sizeX / 3)
				{
					r = value;
					g = 0;
					b = 0;
				}
				imageHandler->setUnsignedLong(elementPointer++, r);
				imageHandler->setUnsignedLong(elementPointer++, g);
				imageHandler->setUnsignedLong(elementPointer++, b);
			}
		}
		imageHandler.release();

		ptr<transforms::colorTransforms::colorTransformsFactory> colorFactory;
		colorFactory = transforms::colorTransforms::colorTransformsFactory::getColorTransformsFactory();
		ptr<transforms::colorTransforms::colorTransform> colorTransform = colorFactory->getTransform(L"RGB", L"YBR_FULL");
		ptr<image> ybrImage = colorTransform->allocateOutputImage(baselineImage, sizeX, sizeY);
		colorTransform->runTransform(baselineImage, 0, 0, sizeX, sizeY, ybrImage, 0, 0);

		std::wstring fileName;
		if(precision == 0)
		{
			fileName = L"testDicomLossyJpeg8bit.dcm";
			dataset->setImage(0, ybrImage, L"1.2.840.10008.1.2.4.50", codecs::codec::veryHigh);
		}
		else
		{
			fileName = L"testDicomLossyJpeg12bit.dcm";
			dataset->setImage(0, ybrImage, L"1.2.840.10008.1.2.4.51", codecs::codec::veryHigh);
		}

		ptr<imebra::codecs::dicomCodec> saveDicom(new imebra::codecs::dicomCodec);
		ptr<puntoexe::stream> saveDicomStream(new puntoexe::stream);
		saveDicomStream->openFile(fileName, std::ios_base::out | std::ios_base::trunc);
		ptr<puntoexe::streamWriter> saveDicomStreamWriter(new puntoexe::streamWriter(saveDicomStream));
		saveDicom->write(saveDicomStreamWriter, dataset);
		saveDicomStreamWriter.release();
		saveDicomStream.release();

		ptr<image> checkImage = dataset->getImage(0);
		std::uint32_t checkSizeX, checkSizeY;
		checkImage->getSize(&checkSizeX, &checkSizeY);

		colorTransform = colorFactory->getTransform(L"YBR_FULL", L"RGB");
		ptr<image> rgbImage = colorTransform->allocateOutputImage(checkImage, checkSizeX, checkSizeY);
		colorTransform->runTransform(checkImage, 0, 0, checkSizeX, checkSizeY, rgbImage, 0, 0);
		ptr<handlers::dataHandlerNumericBase> rgbHandler = rgbImage->getDataHandler(false, &rowSize, &channelsPixelSize, &channelsNumber);
		ptr<handlers::dataHandlerNumericBase> originalHandler = baselineImage->getDataHandler(false, &rowSize, &channelsPixelSize, &channelsNumber);

		// Compare the buffers. A little difference is allowed
        EXPECT_EQ(sizeX, checkSizeX);
        EXPECT_EQ(sizeY, checkSizeY);

		elementPointer = 0;

		std::uint32_t difference = 0;
		for(std::uint32_t checkY = 0; checkY < sizeY; ++checkY)
		{
			for(std::uint32_t checkX = 0; checkX < sizeX; ++checkX)
			{
				for(std::uint32_t channel = 3; channel != 0; --channel)
				{
					std::int32_t value0 = rgbHandler->getUnsignedLong(elementPointer);
					std::int32_t value1 = originalHandler->getUnsignedLong(elementPointer++);
					if(value0 > value1)
					{
						difference += value0 - value1;
					}
					else
					{
						difference += value1 - value0;
					}
				}
			}
		}
        ASSERT_LE(difference, sizeX * sizeY);
	}
}


TEST(jpegCodecTest, testBaselineSubsampled)
{
    for(int subsampledX = 0; subsampledX != 2; ++subsampledX)
    {
        for(int subsampledY = 0; subsampledY != 2; ++subsampledY)
        {
            for(int interleaved = 0; interleaved != 2; ++interleaved)
            {
                std::uint32_t sizeX = 600;
                std::uint32_t sizeY = 400;
                ptr<image> baselineImage(new image);
                baselineImage->create(sizeX, sizeY, image::depthU8, L"RGB", 7);

                std::uint32_t rowSize, channelsPixelSize, channelsNumber;
                ptr<handlers::dataHandlerNumericBase> imageHandler = baselineImage->getDataHandler(true, &rowSize, &channelsPixelSize, &channelsNumber);

                // Make 3 bands (RGB)
                std::uint32_t elementNumber(0);
                for(std::uint32_t y=0; y<sizeY; ++y)
                {
                    for(std::uint32_t x=0; x<sizeX; ++x)
                    {
                        std::int32_t r, g, b;
                        std::uint32_t value = y * 255 / sizeY;
                        r = g = 0;
                        b = value;
                        if(x < sizeX - sizeX/3)
                        {
                            r = 0;
                            g = value;
                            b = 0;
                        }
                        if(x < sizeX / 3)
                        {
                            r = value;
                            g = 0;
                            b = 0;
                        }
                        imageHandler->setUnsignedLong(elementNumber++, r);
                        imageHandler->setUnsignedLong(elementNumber++, g);
                        imageHandler->setUnsignedLong(elementNumber++, b);
                    }
                }
                imageHandler.release();

                ptr<transforms::colorTransforms::colorTransformsFactory> colorFactory;
                colorFactory = transforms::colorTransforms::colorTransformsFactory::getColorTransformsFactory();
                ptr<transforms::colorTransforms::colorTransform> colorTransform = colorFactory->getTransform(L"RGB", L"YBR_FULL");
                ptr<image> ybrImage = colorTransform->allocateOutputImage(baselineImage, sizeX, sizeY);
                colorTransform->runTransform(baselineImage, 0, 0, sizeX, sizeY, ybrImage, 0, 0);

                ptr<memory> streamMemory(new memory);
                {
                    ptr<baseStream> writeStream(new memoryStream(streamMemory));
                    ptr<streamWriter> writer(new streamWriter(writeStream));

                    ptr<codecs::jpegCodec> testCodec(new codecs::jpegCodec);
                    testCodec->setImage(writer, ybrImage, L"1.2.840.10008.1.2.4.50", codecs::codec::medium, "OB", 8, subsampledX != 0, subsampledY != 0, interleaved != 0, false);
                }

                ptr<baseStream> readStream(new memoryStream(streamMemory));
                ptr<streamReader> reader(new streamReader(readStream));

                ptr<codecs::jpegCodec> testCodec(new codecs::jpegCodec);
                ptr<dataSet> readDataSet = testCodec->read(reader);
                ptr<image> checkImage = readDataSet->getImage(0);
                std::uint32_t checkSizeX, checkSizeY;
                checkImage->getSize(&checkSizeX, &checkSizeY);

                colorTransform = colorFactory->getTransform(L"YBR_FULL", L"RGB");
                ptr<image> rgbImage = colorTransform->allocateOutputImage(checkImage, checkSizeX, checkSizeY);
                colorTransform->runTransform(checkImage, 0, 0, checkSizeX, checkSizeY, rgbImage, 0, 0);
                ptr<handlers::dataHandlerNumericBase> rgbHandler = rgbImage->getDataHandler(false, &rowSize, &channelsPixelSize, &channelsNumber);
                ptr<handlers::dataHandlerNumericBase> originalHandler = baselineImage->getDataHandler(false, &rowSize, &channelsPixelSize, &channelsNumber);

                // Compare the buffers. A little difference is allowed
                EXPECT_EQ(sizeX, checkSizeX);
                EXPECT_EQ(sizeY, checkSizeY);

                std::uint32_t difference = 0;
                elementNumber = 0;
                for(std::uint32_t checkY = 0; checkY < sizeY; ++checkY)
                {
                    for(std::uint32_t checkX = 0; checkX < sizeX; ++checkX)
                    {
                        for(std::uint32_t channel = 3; channel != 0; --channel)
                        {
                            std::int32_t value0 = rgbHandler->getUnsignedLong(elementNumber);
                            std::int32_t value1 = originalHandler->getUnsignedLong(elementNumber++);
                            if(value0 > value1)
                            {
                                difference += value0 - value1;
                            }
                            else
                            {
                                difference += value1 - value0;
                            }
                        }
                    }
                }
                ASSERT_LE(difference, sizeX * sizeY * 12);

            }
        }
    }
}

TEST(jpegCodecTest, testLossless)
{
    for(int interleaved = 0; interleaved != 2; ++interleaved)
    {
        for(int bits = 8; bits <= 16; bits += 8)
        {
            for(int firstOrderPrediction = 0; firstOrderPrediction != 2; ++firstOrderPrediction)
            {
                ptr<imebra::dataSet> dataset(new imebra::dataSet);

                std::uint32_t sizeX = 115;
                std::uint32_t sizeY = 400;
                ptr<image> baselineImage(new image);
                baselineImage->create(sizeX, sizeY, bits == 8 ? image::depthU8 : image::depthU16, L"RGB", bits - 1);

                std::uint32_t rowSize, channelsPixelSize, channelsNumber;
                ptr<handlers::dataHandlerNumericBase> imageHandler = baselineImage->getDataHandler(true, &rowSize, &channelsPixelSize, &channelsNumber);

                // Make 3 bands (RGB)
                std::uint32_t elementNumber(0);
                for(std::uint32_t y=0; y<sizeY; ++y)
                {
                    for(std::uint32_t x=0; x<sizeX; ++x)
                    {
                        std::int32_t r, g, b;
                        std::uint32_t value = y * ((std::uint32_t)0x00ff << (bits - 8)) / sizeY;
                        r = g = 0;
                        b = value;
                        if(x < sizeX - sizeX/3)
                        {
                            r = 0;
                            g = value;
                            b = 0;
                        }
                        if(x < sizeX / 3)
                        {
                            r = value;
                            g = 0;
                            b = 0;
                        }
                        imageHandler->setUnsignedLong(elementNumber++, r);
                        imageHandler->setUnsignedLong(elementNumber++, g);
                        imageHandler->setUnsignedLong(elementNumber++, b);
                    }
                }
                imageHandler.release();

                std::wstring transferSyntax = (firstOrderPrediction == 0) ? L"1.2.840.10008.1.2.4.57" : L"1.2.840.10008.1.2.4.70";
                ptr<memory> streamMemory(new memory);
                {
                    ptr<baseStream> writeStream(new memoryStream(streamMemory));
                    ptr<streamWriter> writer(new streamWriter(writeStream));
                    ptr<codecs::jpegCodec> testCodec(new codecs::jpegCodec);
                    testCodec->setImage(writer, baselineImage, transferSyntax, codecs::codec::veryHigh, "OB", bits, false, false, interleaved != 0, false);
                }

                ptr<baseStream> readStream(new memoryStream(streamMemory));
                ptr<streamReader> reader(new streamReader(readStream));

                ptr<codecs::jpegCodec> testCodec(new codecs::jpegCodec);
                ptr<dataSet> readDataSet = testCodec->read(reader);
                ptr<image> checkImage = readDataSet->getImage(0);
                std::uint32_t checkSizeX, checkSizeY;
                checkImage->getSize(&checkSizeX, &checkSizeY);

                ptr<handlers::dataHandlerNumericBase> rgbHandler = checkImage->getDataHandler(false, &rowSize, &channelsPixelSize, &channelsNumber);
                ptr<handlers::dataHandlerNumericBase> originalHandler = baselineImage->getDataHandler(false, &rowSize, &channelsPixelSize, &channelsNumber);

                if(interleaved == 0)
                {
                    dataset->setImage(0, baselineImage, transferSyntax, puntoexe::imebra::codecs::codec::veryHigh);
                    std::wostringstream fileName;
                    fileName << L"dicom_" << transferSyntax << L"_bits" << bits << L".dcm";
                    ptr<imebra::codecs::dicomCodec> saveDicom(new imebra::codecs::dicomCodec);
                    ptr<puntoexe::stream> saveDicomStream(new puntoexe::stream);
                    saveDicomStream->openFile(fileName.str(), std::ios_base::out | std::ios_base::trunc);
                    ptr<puntoexe::streamWriter> saveDicomStreamWriter(new puntoexe::streamWriter(saveDicomStream));
                    saveDicom->write(saveDicomStreamWriter, dataset);
                    saveDicomStreamWriter.release();
                    saveDicomStream.release();
                }

                // Compare the buffers. No difference is allowed
                ASSERT_EQ(sizeX, checkSizeX);
                ASSERT_EQ(sizeY, checkSizeY);

                elementNumber = 0;

                for(std::uint32_t checkY = 0; checkY < sizeY; ++checkY)
                {
                    for(std::uint32_t checkX = 0; checkX < sizeX; ++checkX)
                    {
                        for(std::uint32_t channel = 3; channel != 0; --channel)
                        {
                            std::int32_t value0 = originalHandler->getUnsignedLong(elementNumber);
                            std::int32_t value1 = rgbHandler->getUnsignedLong(elementNumber++);
                            EXPECT_EQ(value0, value1);
                        }
                    }
                }
            }
        }
    }
}


} // namespace tests

} // namespace imebra

} // namespace puntoexe
