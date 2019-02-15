#pragma once
#include "AbstractBaseObject.h"
#include <limits>

class DicomPointCloudObject;

class Curve : public AbstractBaseObject
{

private:
	std::vector<glm::vec3> positions;
	std::vector<glm::vec3> normals;
	std::vector<glm::vec3> controlPoints;
	std::vector<glm::vec3> instanced_colors = { glm::vec3(0.0, 1.0, 1.0), glm::vec3(1.0, 0.0, 0.0) };

	GLuint normals_buffer;
	GLuint instanced_positions_buffer;
	GLuint instanced_colors_buffer;

	DicomPointCloudObject* pointCloud;
	void Load();
	void Load(int i);
	int Type();

public:
	Curve();
	~Curve();
	Curve(std::vector<glm::vec3> curvePoints);
	void SetPositions(std::vector<glm::vec3> curvePoints);
	std::vector<glm::vec3> GetPositions();
	void SetNormals(std::vector<glm::vec3> normals);
	std::vector<glm::vec3> GetNormals();
	void RenderCurve();
	void RenderCurveHighlight();
	void SetDicomPointCloudObject(DicomPointCloudObject* pointCloud);
	void SetControlPoints(std::vector<glm::vec3> controlPoints);
	std::vector<glm::vec3> GetControlPoints();
};