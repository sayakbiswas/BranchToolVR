#pragma once

#include <vector>
#include <string>

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "MiscFunctions.h"
#include "Constants.h"

struct InteractionCache
{
	glm::mat4 to_controller_space_initial;
	glm::mat4 controller_pose_initial;
	glm::mat4 controller_pose_updated;
	glm::mat4 append_pose_initial;
	glm::vec3 controller_world_position_initial;

	// dual gesture variables	
	glm::vec3 primary_collision_point_world_initial;
	glm::vec3 primary_collision_point_world_current;
	glm::vec3 primary_collision_point_controller_space_initial;
	glm::vec3 secondary_collision_point_world_initial;
	glm::vec3 secondary_collision_point_world_current;
	glm::vec3 secondary_collision_point_controller_space_initial;
	glm::vec3 primary_to_secondary_collision_point_initial;
	glm::vec3 primary_to_secondary_collision_point_current;
	float initial_scale;
};

class AbstractBaseObject
{
	public:
		AbstractBaseObject();
		~AbstractBaseObject();

		virtual bool TestCollision(glm::vec3 _ray, glm::vec3 _pos, glm::vec3 & _collisionPoint);
		virtual void Load() = 0;
		virtual int Type() = 0;
		virtual void SetSelected(bool _selected) { is_selected = _selected; };		

		void SetWorldPosition(const glm::vec3& v);
		void SetModelOrientation(const glm::vec3& v);
		void SetModelPositionX(float _x);
		void SetModelPositionY(const float _y);
		void SetModelPositionZ(const float _z);
		void SetModelPosition(const glm::vec3& v);
		void SetScale(const float & _scale);
		void SetAppendPose(const glm::mat4& m);
		void SetModelMatrixOverride(const glm::mat4 & m);
		
		glm::vec3 GetWorldPosition();
		glm::vec3 GetModelOrientation();
		glm::vec3 GetModelPosition();
		float GetScale();
		glm::mat4 GetAppendPose();
		glm::mat4 GetModelMatrix();		
		GLint GetVao();
		GLint GetNumVertices();
		glm::mat4 GetDoubleSelectionTransform();
		glm::mat4 GetDoubleSelectionScaleDifference();

		void ResetPose();	
		void SetClicked();
		bool WasClicked();
		bool TestBoundingSphere(glm::vec3 _ray, glm::vec3 _pos);
				
		bool is_loaded;
		bool is_hidden;
		bool is_selectable;
		bool is_clickable;
		bool is_clicked;
		bool is_selected;
		bool is_double_selected;
		bool is_double_selectable;
		const int id;
		int controllerSelectorId;
		int controllerSelectorIdPrev;
		int texture_level;		
		InteractionCache cache;		

	protected:
		static int object_id_counter;
		static int type_id_counter;		
		template<typename T> int get_type_id();

		// model matrix components
		glm::vec3 world_position;
		glm::vec3 model_orientation;
		glm::vec3 model_position;
		float scale;
		glm::mat4 append_pose;
		void CalcModelMatrix();		
		glm::mat4 model_matrix;
		glm::mat4 base_model_matrix;

		// common attributes
		GLuint vao;
		GLuint num_vertices;
		GLuint positions_buffer;
		std::vector<glm::vec3> positions;

		// bounding sphere variables
		glm::vec3 model_max;
		glm::vec3 center;
		float model_max_length;
		void SetBoundingSphere();
};

template<typename T>
int AbstractBaseObject::get_type_id()
{
	static bool exists = false;
	static int type_id;
	
	if (!exists)
	{
		type_id = type_id_counter++;
		exists = true;
	}
	
	return type_id;
}
