#pragma once
#include "AbstractBaseObject.h"

class DicomPointCloudObject;

class Curve :	public AbstractBaseObject
{

private:
	std::vector<glm::vec3> positions;
	std::vector<glm::vec3> normals;
	std::vector<glm::vec3> controlPoints;
	GLuint normals_buffer;
	GLuint instanced_positions_buffer;
	DicomPointCloudObject* pointCloud;
	void Load();
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
	void SetDicomPointCloudObject(DicomPointCloudObject* pointCloud);
	void SetControlPoints(std::vector<glm::vec3> controlPoints);
	std::vector<glm::vec3> GetControlPoints();
};

