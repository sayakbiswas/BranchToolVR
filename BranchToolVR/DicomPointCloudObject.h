#pragma once

#include <GL/glew.h>
#include <glm\glm.hpp>
#include <glm\gtc\quaternion.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtx\euler_angles.hpp>
#include <vector>

#include "DicomReader.h"
#include "ColorObject.h"
#include "TextureObject.h"
#include "Constants.h"
#include "LeastSquaresFit.h"
#include "Curve.h"

struct IsovaluePointCloudSlider;
struct BranchPoint 
{
	glm::vec3 position;
	int id;
	std::vector<int> neighbors;
	bool is_selected;
	static int id_counter;

	BranchPoint(glm::vec3 _inPos)
	{
		position = _inPos;
		id = id_counter++;
		is_selected = false;
	}

	BranchPoint()
	{
		position = glm::vec3(0.0f);
		id = id_counter++;
		is_selected = false;
	}

	glm::vec4 getColor() 
	{
		if (is_selected) 
		{
			return Constants::SELECTED_BRANCH_POINT_COLOR;
		}
		return Constants::BASE_BRANCH_POINT_COLOR;
	}
};

class DicomPointCloudObject : public AbstractBaseObject
{
	public:
		DicomPointCloudObject();
		~DicomPointCloudObject();
		void Generate(DicomSet & _ds, int isovalue, int max_tolerance, std::vector<IsovaluePointCloudSlider*>& isovalue_point_cloud_sliders);
		void Generate(DicomSet & _ds, int isovalue, int max_tolerance, int first, int last, std::vector<IsovaluePointCloudSlider*>& isovalue_point_cloud_sliders);
		void SetMasterAppendPose(glm::mat4 _in);
		void Clear();
		int Type();	
		const GLuint GetNumInstances() { return num_instances; }
		BranchPoint* GetBranchPointByID(int id);	
		void MarkForRegeneration();
		bool first_load;	

		int curr_tolerance;
		glm::vec3 lower_bounds;
		glm::vec3 upper_bounds;
		glm::vec3 voxel_scale;		
		
		TextureObject* handle;		
		ColorObject* branch_point_marker;
		std::vector<BranchPoint*> branch_points;
		int current_bp_selection;		

	private:		
		void GenerateCube(glm::vec3 _scale, glm::vec3 _offset);
		void GenerateSphere(float _scale);	
		void Load();		
		bool has_changed; // used to flag if the sliders have changed after each change for regeneration

		unsigned int num_instances;
		std::vector<glm::vec3> normals;
		std::vector<glm::vec3> instanced_positions;
		std::vector<glm::vec3> instanced_colors;
		std::vector<GLfloat> instanced_isovalue_differences;

		GLuint normals_buffer;
		GLuint texture_coords_buffer;
		GLuint instanced_positions_buffer;
		GLuint instanced_colors_buffer;
		GLuint instanced_isovalue_differences_buffer;
};
