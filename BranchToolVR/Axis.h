#pragma once

#include <GL\glew.h>
#include <glm\glm.hpp>
#include <vector>

#include "AbstractBaseObject.h"

class Axis : public AbstractBaseObject
{
	public:
		Axis();
		~Axis();
		void GenerateAxis();
		int Type();

	private:
		void Load();
		std::vector<glm::vec3> colors;
		GLuint colors_buffer;
};
