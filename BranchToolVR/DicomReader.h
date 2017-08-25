#pragma once

#include <future>
#include <windows.h>
#include <Windows.h>
#include <thread>
#include <string>
#include <iostream>

#include <glm/glm.hpp>
#include <imebra.h>
#include <tinydir.h>
#include <pathtools.h>

#include "Constants.h"
#include "MiscFunctions.h"

struct DicomSingle 
{
	std::vector<unsigned short> isovalues;
	int width;
	int height;
	int layer;
	float spacing;
	float pixel_spacing;
	std::string series_id;
};

struct DicomSet 
{
	std::vector<DicomSingle> data;
	std::string series_id;
	glm::vec3 scale;
	bool is_loaded;
	int isovalue;
	int isovalue_tolerance;
	int window_center;
	int window_width;
	int current_index;
	unsigned short min;
	unsigned short max;

	DicomSet() 
	{
		series_id = "";
		scale = glm::vec3(0.0f, 0.0f, 0.0f);
		current_index = 0;
		is_loaded = false;
		isovalue = DEFAULT_ISOVALUE;
		isovalue_tolerance = DEFAULT_ISOVALUE_TOLERANCE;
		window_center = DEFAULT_WINDOW_CENTER;
		window_width = DEFAULT_WINDOW_WIDTH;
	}
};

class DicomReader
{
	public:
		static DicomSet ReadSet(std::string _dicomFolder);
		static DicomSingle ReadSingle(std::string _filePath);
};
