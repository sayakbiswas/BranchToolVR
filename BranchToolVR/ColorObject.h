#pragma once

#include <GL\glew.h>
#include <glm\glm.hpp>
#include <glm\gtc\quaternion.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtx\euler_angles.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>

#include "AbstractBaseObject.h"
#include "DicomReader.h"
#include "MiscFunctions.h"

class ColorObject : public AbstractBaseObject {
	public:
		ColorObject();
		~ColorObject();
		void GenerateXYPlane(float _scaleX, float _scaleY, glm::vec2 _padding, glm::vec3 _offset);
		void GenerateXYPlane(float _scaleX, float _scaleY, float _padding, glm::vec3 _offset);
		void GenerateXZPlane(float _scaleX, float _scaleZ, float _padding, glm::vec3 _offset);
		void GenerateXYPrism(float _scaleX, float _scaleY, float _scaaleZ, glm::vec2 _padding, glm::vec3 _offset);
		void GenerateXYPrism(glm::vec3 _scale, glm::vec2 _padding, glm::vec3 _offset);
		void GenerateSphere(int _res, float _radius, bool _invNormals);
		void GenerateIsosurfaceFromDicomSet(DicomSet & _dSet, int _isovalue);
		void GenerateController();
		void GenerateGround();
		void SetDisplayColor(const glm::vec4 & _inColor);
		void SetSelected(bool _isSelected);
		glm::vec4 GetDisplayColor();		
		int Type();

	private:
		void AddRectangularPrism(glm::vec3 _scale, glm::vec3 _offset);
		void Finalize();
		void Load();

		glm::vec4 display_color;
		glm::vec4 selected_color_additive;
		float display_color_modifier;
		float selection_modifier;

		std::vector<glm::vec3> normals;
		GLuint normals_buffer;
};
