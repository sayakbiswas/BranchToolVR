#include "DicomReader.h"

bool startup = true;

struct less_than_key
{
	inline bool operator() (const DicomSingle& struct1, const DicomSingle& struct2)
	{
		return (struct1.layer < struct2.layer);
	}
};

// Changed parameters to accomodate for user folder selection
DicomSet DicomReader::ReadSet(std::string _dicomFolder)
{
	if (startup) {
		_dicomFolder = MiscFunctions::relativeToAbsolutePath(DirectoryInfo::RELATIVE_DICOM_DIR + _dicomFolder);
		startup = false;
	}
	DicomSet _dSet;

	tinydir_dir dir;
	tinydir_open(&dir, _dicomFolder.c_str()); 

	// single threaded version
	/*
	while (dir.has_next){
	
		tinydir_file file;
		tinydir_readfile(&dir, &file);
	
		if (std::string(file.extension) == "dcm") {
			_dSet.data.push_back(ReadSingle(std::string(file.path)));
		}
	
		tinydir_next(&dir);
	}
	*/

	// multi-threaded version
	std::vector<std::future<DicomSingle>> threads;
	
	while (dir.has_next)
	{
		tinydir_file file;
		tinydir_readfile(&dir, &file);
	
		if (std::string(file.extension) == "dcm")
		{
			threads.emplace_back(std::async(ReadSingle, std::string(file.path)));
		}
	
		tinydir_next(&dir);
	}
	
	tinydir_close(&dir);
	
	for (std::future<DicomSingle> & d : threads) 
	{
		_dSet.data.push_back(d.get());
	}
	
	std::sort(_dSet.data.begin(), _dSet.data.end(), less_than_key());

	if (_dSet.data.size() > 1) 
	{
		float perPixel = 1.0f / (_dSet.data[0].pixel_spacing * _dSet.data[0].width);
		float heightToWidthRatio = _dSet.data[0].spacing / _dSet.data[0].pixel_spacing;
		float scaledHeight = perPixel * heightToWidthRatio;

		_dSet.scale = glm::vec3(1.0f, 1.0f, (float) _dSet.data.size() / (float)_dSet.data[0].width);
		_dSet.scale.z = (float)_dSet.data.size() * scaledHeight;
	}


	for (int i = 0; i < _dSet.data.size(); ++i) {
		auto tmp_max = *std::max_element(_dSet.data[i].isovalues.begin(), _dSet.data[i].isovalues.end());
		auto tmp_min = *std::min_element(_dSet.data[i].isovalues.begin(), _dSet.data[i].isovalues.end());

		if (i == 0) {
			_dSet.max = tmp_max;
			_dSet.min = tmp_min;
			continue;
		}
		
		if (_dSet.max < tmp_max) _dSet.max = tmp_max;
		if (_dSet.min > tmp_min) _dSet.min = tmp_min;
	}

	std::cout << _dSet.max << std::endl;
	std::cout << _dSet.min << std::endl;

	return _dSet;
}

DicomSingle DicomReader::ReadSingle(std::string _filePath) 
{
	DicomSingle _dSingle;

	using namespace puntoexe;

	ptr<stream> readStream(new stream);
	readStream->openFile(_filePath, std::ios::in);
	ptr<streamReader> reader(new streamReader(readStream));

	ptr<imebra::dataSet> testDataSet = imebra::codecs::codecFactory::getCodecFactory()->load(reader);

	_dSingle.layer = std::atoi(testDataSet->getString(0x0020, 0, 0x0013, 0).c_str());
	_dSingle.series_id = testDataSet->getString(0x0020, 0, 0x000E, 0);

	float spacingBetweenSlices = abs(std::atof(testDataSet->getString(0x0018, 0, 0x0088, 0).c_str()));
	float pixelSpacing = abs(std::atof(testDataSet->getString(0x0028, 0, 0x0030, 0).c_str()));

	_dSingle.spacing = spacingBetweenSlices;
	_dSingle.pixel_spacing = pixelSpacing;

	ptr<imebra::image> firstImage = testDataSet->getImage(0);
	std::uint32_t rowSize, channelPixelSize, channelsNumber;
	ptr<imebra::handlers::dataHandlerNumericBase> myHandler = firstImage->getDataHandler(false, &rowSize, &channelPixelSize, &channelsNumber);
	
	std::uint32_t sizeX, sizeY;
	firstImage->getSize(&sizeX, &sizeY);

	_dSingle.width = sizeX;
	_dSingle.height = sizeY;
	_dSingle.isovalues.clear();

	std::uint32_t index(0);
	for (std::uint32_t scanY = 0; scanY < sizeY; ++scanY)
	{
		for (std::uint32_t scanX = 0; scanX < sizeX; ++scanX)
		{
			for (std::uint32_t scanChannel = 0; scanChannel < 1; ++scanChannel) // TEMP: loop condition set to 1 (was channelsNumber)
			{ 
				_dSingle.isovalues.push_back(myHandler->getUnsignedLong(index++));
			}
		}
	}

	return _dSingle;
}
