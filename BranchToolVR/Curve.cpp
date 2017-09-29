#include "Curve.h"

Curve::Curve()
{
}


Curve::~Curve()
{
}

Curve::Curve(std::vector<glm::vec3> curvePoints)
{
	this->positions = curvePoints;
}

void Curve::SetPositions(std::vector<glm::vec3> curvePoints)
{
	this->positions = curvePoints;
}

std::vector<glm::vec3> Curve::GetPositions()
{
	return positions;
}

void Curve::SetNormals(std::vector<glm::vec3> normals)
{
	this->normals = normals;
}

std::vector<glm::vec3> Curve::GetNormals()
{
	return normals;
}

void Curve::Load()
{
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glGenBuffers(1, &positions_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, positions_buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * positions.size(), &positions[0], GL_STATIC_DRAW);

	glGenBuffers(1, &normals_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, normals_buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * normals.size(), &normals[0], GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, positions_buffer);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, normals_buffer);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

	glBindVertexArray(0);
	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
}

int Curve::Type()
{
	return AbstractBaseObject::get_type_id<Curve>();
}

void Curve::RenderCurve()
{
	Load();
}