/*

Imebra 2015 build 20160119-002

Imebra: a C++ Dicom library

Copyright (c) 2003, 2004, 2005, 2006, 2007, 2008, 2009, 2010, 2011, 2012, 2013, 2014, 2015
by Paolo Brandoli/Binarno s.p.

All rights reserved.

This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License version 2 as published by
 the Free Software Foundation.

This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

-------------------

If you want to use Imebra commercially then you have to buy the commercial
 license available at http://imebra.com

After you buy the commercial license then you can use Imebra according
 to the terms described in the Imebra Commercial License Version 2.
A copy of the Imebra Commercial License Version 2 is available in the
 documentation pages.

Imebra is available at http://imebra.com

The author can be contacted by email at info@binarno.com or by mail at
 the following address:
 Binarno s.p., Paolo Brandoli
 Rakuseva 14
 1000 Ljubljana
 Slovenia



*/

#include <iostream>


#include "../../library/imebra/include/imebra.h"
#include <sstream>

#ifdef PUNTOEXE_WINDOWS
#include <process.h>
#else
#include <spawn.h>
#include <sys/wait.h>
#endif

#include <memory>
#include <list>

using namespace puntoexe;
using namespace puntoexe::imebra;

int findArgument(const char* argument, int argc, char* argv[])
{
	for(int scanArg(0); scanArg != argc; ++scanArg)
	{
		if(std::string(argv[scanArg]) == argument)
		{
			return scanArg;
		}
	}
	return -1;
}

void CopyGroups(ptr<dataSet> source, ptr<dataSet> destination)
{
	for(ptr<dataCollectionIterator<dataGroup> > scanGroups(source->getDataIterator()); scanGroups->isValid(); scanGroups->incIterator())
	{
		ptr<dataGroup> sourceGroup(scanGroups->getData());
		ptr<dataGroup> newGroup(destination->getGroup(scanGroups->getId(), scanGroups->getOrder(), true));

		for(ptr<dataCollectionIterator<imebra::data> > scanTags(sourceGroup->getDataIterator()); scanTags->isValid(); scanTags->incIterator())
		{
			if(!newGroup->getDataType(scanTags->getId()).empty())
			{
				continue;
			}

			ptr<data> sourceTag(scanTags->getData());
			ptr<data> destTag(newGroup->getTag(scanTags->getId(), true));

			for(imbxUint32 itemId(0); ; ++itemId)
			{
				ptr<dataSet> sourceSequenceItem(sourceTag->getDataSet(itemId));
				if(sourceSequenceItem != 0)
				{
					ptr<dataSet> destSequenceItem(new dataSet);
					destTag->setDataSet(itemId, destSequenceItem);
					CopyGroups(sourceSequenceItem, destSequenceItem);
					continue;
				}
				ptr<handlers::dataHandlerRaw> sourceHandler(sourceTag->getDataHandlerRaw(itemId, false, ""));
				if(sourceHandler == 0)
				{
					break;
				}
				ptr<handlers::dataHandlerRaw> destHandler(destTag->getDataHandlerRaw(itemId, true, sourceHandler->getDataType()));
				imbxUint8* pSourceBuffer(sourceHandler->getMemoryBuffer());
				size_t sourceSize(sourceHandler->getMemorySize());
				destHandler->setSize(sourceSize);
				imbxUint8* pDestBuffer(destHandler->getMemoryBuffer());
				::memcpy(pDestBuffer, pSourceBuffer, sourceSize);
			}
		}
	}

}

int main(int argc, char* argv[])
{
	std::wstring version(L"1.0.0.1");
	std::wcout << L"changeTransferSyntax version " << version << std::endl;

	try
	{
		std::string inputFileName;
		std::string outputFileName;
		std::wstring transferSyntax;
		if(argc == 4)
		{
            const wchar_t* transferSyntaxAllowedValues[]=
			{
				L"1.2.840.10008.1.2.1", // Explicit VR little endian
				L"1.2.840.10008.1.2.2", // Explicit VR big endian
				L"1.2.840.10008.1.2.5", // RLE compression
				L"1.2.840.10008.1.2.4.50", // Jpeg baseline (8 bits lossy)
				L"1.2.840.10008.1.2.4.51", // Jpeg extended (12 bits lossy)
				L"1.2.840.10008.1.2.4.57" // Jpeg lossless NH
			};

			inputFileName = argv[1];
			outputFileName = argv[2];

			std::istringstream convertTransferSyntax(argv[3]);
			int transferSyntaxValue(-1);
			convertTransferSyntax >> transferSyntaxValue;
            if(transferSyntaxValue >= 0 && (size_t)transferSyntaxValue < sizeof(transferSyntaxAllowedValues)/sizeof(wchar_t*))
			{
				transferSyntax = transferSyntaxAllowedValues[transferSyntaxValue];
			}
		}

		if(inputFileName.empty() || outputFileName.empty() || transferSyntax.empty())
		{
					std::wcout << L"Usage: changeTransferSyntax inputFileName outputFileName newTransferSyntax" << std::endl;
					std::wcout << L"newTransferSyntax values: 0 = Explicit VR little endian" << std::endl;
					std::wcout << L"                          1 = Explicit VR big endian" << std::endl;
					std::wcout << L"                          2 = RLE compression" << std::endl;
					std::wcout << L"                          3 = Jpeg baseline (8 bits lossy)" << std::endl;
					std::wcout << L"                          4 = Jpeg extended (12 bits lossy)" << std::endl;
					std::wcout << L"                          5 = Jpeg lossless NH" << std::endl;
					return 1;
		}

		ptr<dataSet> loadedDataSet;

		// Open the file containing the dicom dataset
		ptr<puntoexe::stream> inputStream(new puntoexe::stream);
		inputStream->openFile(inputFileName, std::ios_base::in);

		// Connect a stream reader to the dicom stream. Several stream reader
		//  can share the same stream
		ptr<streamReader> reader(new streamReader(inputStream));

		// Get a codec factory and let it use the right codec to create a dataset
		//  from the input stream
		ptr<codecs::codecFactory> codecsFactory(codecs::codecFactory::getCodecFactory());
		loadedDataSet = codecsFactory->load(reader, 2048);

		// Now we create a new dataset and copy the tags and images from the loaded dataset
		ptr<puntoexe::imebra::dataSet> newDataSet(new puntoexe::imebra::dataSet);

		// Copy the images first
		try
		{
			imbxUint32 scanImages(0);
			for(;;)
			{
				ptr<image> copyImage(loadedDataSet->getImage(scanImages));
				newDataSet->setImage(scanImages, copyImage, transferSyntax, imebra::codecs::codec::high);
				++scanImages;
			}
		}
		catch(const dataSetImageDoesntExist&)
		{
			// Ignore this
		}

		CopyGroups(loadedDataSet, newDataSet);

		// Write dataset
		ptr<puntoexe::stream> outputStream(new puntoexe::stream);
		outputStream->openFile(outputFileName, std::ios::out);
		ptr<streamWriter> writer(new streamWriter(outputStream));
		ptr<codecs::dicomCodec> writeCodec(new codecs::dicomCodec);
		writeCodec->write(writer, newDataSet);
	}
	catch(...)
	{
		std::wcout << exceptionsManager::getMessage();
		return 1;
	}
}

