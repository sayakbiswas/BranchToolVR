#pragma once

#include <GL\glew.h>
#include <glm\glm.hpp>
#include <vector>

#include "AbstractBaseObject.h"

class LineObject : public AbstractBaseObject
{
	public:
		LineObject();
		~LineObject();
		void GenerateAxis();
		int Type();

	private:
		void Load();
		std::vector<glm::vec3> colors;
		GLuint colors_buffer;
};
