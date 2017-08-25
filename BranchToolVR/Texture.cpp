#include "Texture.h"

int Texture::texture_id_counter = 0;

Texture::Texture() : id(texture_id_counter++)
{
	is_loaded = false;
}

Texture::~Texture()
{
	Delete();
}

void Texture::Bind(int _level) 
{
	if (is_loaded) 
	{
		glActiveTexture(GL_TEXTURE0 + _level);
		glBindTexture(GL_TEXTURE_2D, gl_id);
	}
}

void Texture::Delete() 
{
	if(is_loaded)
	glDeleteTextures(1, &gl_id);
}

bool Texture::Load(DicomSingle & _ds, int window_width, int window_center)
{
	if (&_ds == NULL || !_ds.isovalues.size()) 
	{
		return false;
	}

	Delete();

	int half_ww = window_width / 2;
	std::vector<char> tmp;
	for (int i = 0; i < _ds.isovalues.size(); ++i) 
	{
		if (abs((int)_ds.isovalues[i] - (int)window_center) < half_ww) 
		{
			int window_start = window_center - half_ww;
			char scaled_value = 255.0f*((float)((int)_ds.isovalues[i] - window_start) / (float)window_width);
			
			tmp.push_back(scaled_value);
			tmp.push_back(scaled_value);
			tmp.push_back(scaled_value);
			tmp.push_back(240); // set alpha to be slightly transparent
		}
		else 
		{
			tmp.push_back(10);
			tmp.push_back(100);
			tmp.push_back(20);
			tmp.push_back(100);
		}
	}

	width = _ds.width;
	height = _ds.height;

	glGenTextures(1, &gl_id);
	glBindTexture(GL_TEXTURE_2D, gl_id);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, &tmp[0]);

	glGenerateMipmap(GL_TEXTURE_2D);
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	
	GLfloat fLargest;
	glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &fLargest);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, fLargest);

	glBindTexture(GL_TEXTURE_2D, 0);

	is_loaded = gl_id != 0;
	return is_loaded;
}

bool Texture::Load(std::string _textureName) 
{
	std::string strFullPath = MiscFunctions::relativeToAbsolutePath(DirectoryInfo::RELATIVE_TEXTURES_DIR + _textureName + ".png");
	std::vector<unsigned char> imageRGBA;
	unsigned nImageWidth, nImageHeight;

	// read the image
	unsigned nError = lodepng::decode(imageRGBA, nImageWidth, nImageHeight, strFullPath.c_str());

	// flip the image
	unsigned int bytes_per_pixel = 4;
	char * tmp = new char[bytes_per_pixel];
	for (int i = 0; i < nImageHeight / 2; ++i) 
	{
		int cIndex_lower = i * nImageWidth * bytes_per_pixel;
		int cIndex_upper = (nImageHeight - i - 1) * nImageWidth * 4;
		for (int j = 0; j < nImageWidth*bytes_per_pixel; j+= bytes_per_pixel) 
		{
			for (unsigned int k = 0; k < bytes_per_pixel; ++k) 
			{
				tmp[k] = imageRGBA[cIndex_lower + j + k];
				imageRGBA[cIndex_lower + j + k] = imageRGBA[cIndex_upper + j + k];
				imageRGBA[cIndex_upper + j + k] = tmp[k];
			}
		}
	}

	if (nError != 0)
		return false;

	glGenTextures(1, &gl_id);
	glBindTexture(GL_TEXTURE_2D, gl_id);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, nImageWidth, nImageHeight,
		0, GL_RGBA, GL_UNSIGNED_BYTE, &imageRGBA[0]);

	glGenerateMipmap(GL_TEXTURE_2D);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

	GLfloat fLargest;
	glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &fLargest);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, fLargest);

	glBindTexture(GL_TEXTURE_2D, 0);

	is_loaded = gl_id != 0;
	return is_loaded;
}
