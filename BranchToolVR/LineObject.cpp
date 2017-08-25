#include "LineObject.h"

LineObject::LineObject()
{
}

LineObject::~LineObject()
{
}

int LineObject::Type()
{
	return AbstractBaseObject::get_type_id<LineObject>();
}

void LineObject::GenerateAxis() 
{
	positions.push_back(glm::vec3(0.0f, 0.0f, 0.0f));
	positions.push_back(glm::vec3(1.0f, 0.0f, 0.0f));

	positions.push_back(glm::vec3(0.0f, 0.0f, 0.0f));
	positions.push_back(glm::vec3(0.0f, 1.0f, 0.0f));

	positions.push_back(glm::vec3(0.0f, 0.0f, 0.0f));
	positions.push_back(glm::vec3(0.0f, 0.0f, 1.0f));

	colors.push_back(glm::vec3(1.0f, 0.0f, 0.0f));
	colors.push_back(glm::vec3(1.0f, 0.0f, 0.0f));

	colors.push_back(glm::vec3(0.0f, 1.0f, 0.0f));
	colors.push_back(glm::vec3(0.0f, 1.0f, 0.0f));

	colors.push_back(glm::vec3(0.0f, 0.0f, 1.0f));
	colors.push_back(glm::vec3(0.0f, 0.0f, 1.0f));

	Load();
}

void LineObject::Load() 
{
	num_vertices = positions.size();

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glGenBuffers(1, &positions_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, positions_buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3)*positions.size(), &positions[0], GL_STATIC_DRAW);

	glGenBuffers(1, &colors_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, colors_buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3)*colors.size(), &colors[0], GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, positions_buffer);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, colors_buffer);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

	glBindVertexArray(0);
	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
}
