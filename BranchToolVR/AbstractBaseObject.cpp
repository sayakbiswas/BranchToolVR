#include "AbstractBaseObject.h"

#define TINYOBJLOADER_IMPLEMENTATION 
#include "tiny_obj_loader.h"

int AbstractBaseObject::object_id_counter = 0;
int AbstractBaseObject::type_id_counter = 0;

AbstractBaseObject::AbstractBaseObject() : id(object_id_counter++)
{
	world_position = glm::vec3(0.0, 0.0, 0.0f);
	model_orientation = glm::vec3(0.0f, 0.0f, 0.0f);
	model_position = glm::vec3(0.0f, 0.0f, 0.0f);
	append_pose = glm::mat4(1.0f);
	model_matrix = glm::mat4(1.0f);
	base_model_matrix = glm::mat4(1.0f);
	scale = 1.0f;

	is_loaded = false;
	is_hidden = false;
	is_selectable = false;	
	is_selected = false;
	is_clickable = false;	
	is_clicked = false;
	is_double_selected = false;
	is_double_selectable = false;

	controllerSelectorId = -1;
	controllerSelectorIdPrev = -1;

	CalcModelMatrix();
}

AbstractBaseObject::~AbstractBaseObject()
{
}

glm::mat4 AbstractBaseObject::GetModelMatrix() 
{
	return model_matrix;
}

void AbstractBaseObject::SetClicked() 
{
	is_clicked = true;
}

bool AbstractBaseObject::WasClicked()
{
	if (is_clicked)
	{
		is_clicked = false;
		return true;
	}

	return false;
}

void AbstractBaseObject::CalcModelMatrix()
{
	base_model_matrix = glm::translate(glm::mat4(), model_position) 
		* glm::yawPitchRoll(model_orientation.x, model_orientation.y, model_orientation.z)
		* glm::scale(glm::mat4(1.0f), glm::vec3(scale));
	
	model_matrix = append_pose
		* glm::translate(glm::mat4(), world_position)
		* base_model_matrix;

	center = glm::vec3(model_matrix[0][3], model_matrix[1][3], model_matrix[2][3]);
}

// setters
void AbstractBaseObject::SetWorldPosition(const glm::vec3& v)
{
	world_position = v;
	CalcModelMatrix();
}

void AbstractBaseObject::SetModelOrientation(const glm::vec3& v)
{
	model_orientation = v;
	CalcModelMatrix();
}

void AbstractBaseObject::SetModelPosition(const glm::vec3& v)
{
	model_position = v;
	CalcModelMatrix();
}

void AbstractBaseObject::SetScale(const float & _scale)
{
	scale = _scale;
	CalcModelMatrix();
}

void AbstractBaseObject::SetModelPositionX(const float _x)
{
	model_position.x = _x;
	CalcModelMatrix();
}

void AbstractBaseObject::SetModelPositionY(const float _y)
{
	model_position.y = _y;
	CalcModelMatrix();
}

void AbstractBaseObject::SetModelPositionZ(const float _z)
{
	model_position.z = _z;
	CalcModelMatrix();
}

void AbstractBaseObject::SetAppendPose(const glm::mat4& m)
{
	append_pose = m;
	CalcModelMatrix();
}

void AbstractBaseObject::SetModelMatrixOverride(const glm::mat4& m)
{
	model_matrix  = m;
}

// getters
glm::vec3 AbstractBaseObject::GetWorldPosition() { return world_position; }
glm::vec3 AbstractBaseObject::GetModelOrientation() { return model_orientation; }
glm::vec3 AbstractBaseObject::GetModelPosition() { return model_position; }
float AbstractBaseObject::GetScale() { return scale; }
glm::mat4 AbstractBaseObject::GetAppendPose() { return append_pose; }
GLint AbstractBaseObject::GetVao() { return vao; }
GLint AbstractBaseObject::GetNumVertices() { return num_vertices; }

void AbstractBaseObject::ResetPose()
{
	world_position = glm::vec3(0.0f);
	model_orientation = glm::vec3(0.0f);
	model_position = glm::vec3(0.0f);
	CalcModelMatrix();
}

bool AbstractBaseObject::TestCollision(glm::vec3 _ray, glm::vec3 _pos, glm::vec3 & _collisionPoint)
{
	bool found_collision = false;
	float collision_dist = 0.0f;
	
	if (true || TestBoundingSphere(_ray, _pos)) // TODO: fix bounding sphere generation and test
	{
		glm::mat4 mm = GetModelMatrix();
		glm::vec3 cg;

		for (int i = 0; i < positions.size(); i += 3)
		{
			glm::vec4 tri[3];
			tri[0] = mm * glm::vec4(positions[i + 0], 1.0f);
			tri[1] = mm * glm::vec4(positions[i + 1], 1.0f);
			tri[2] = mm * glm::vec4(positions[i + 2], 1.0f);

			MiscFunctions::ray_triangle_intersection(glm::value_ptr(tri[0]), glm::value_ptr(tri[1]), glm::value_ptr(tri[2]), glm::value_ptr(_pos), glm::value_ptr(_ray), glm::value_ptr(cg));
			
			if (cg.x >= 0 && cg.y >= 0 && cg.z > 0)
			{
				glm::vec3 tmp = glm::vec3(cg.x * tri[0] + cg.y * tri[1] + cg.z * tri[2]);
				float distTMP = glm::length(_pos - tmp);

				if (glm::dot(_ray, tmp - _pos) > 0 && (!found_collision || collision_dist > distTMP))
				{
					collision_dist = distTMP;
					_collisionPoint = tmp;
					found_collision = true;
				}
			}
		}
	}

	if (found_collision) 
	{
		return true;
	}

	_collisionPoint = glm::vec3(0.0f, 0.0f, 0.0f);
	return false;
}

bool AbstractBaseObject::TestBoundingSphere(glm::vec3 _ray, glm::vec3 _pos) 
{
	if (_ray == Constants::ZERO_VEC3)
		return false;

	_ray = glm::normalize(_ray);
	glm::vec3 to_center = center - _pos;
	float factor1 = glm::dot(_ray, to_center);

	glm::vec3 perpendicular_vec = factor1 * _ray + _pos - center;

	return glm::length(perpendicular_vec) < glm::length(model_max*scale);
}

glm::mat4 AbstractBaseObject::GetDoubleSelectionTransform()
{
	/*
		1) translate to controller space
		2) center the intersection point to the origin
		3) rotate the old interesection ray to the new one
		4) translate back to intersection point in controller space
		5) apply updated controller pose
	*/

	return cache.controller_pose_updated
		* glm::translate(glm::mat4(1.0f), cache.primary_collision_point_controller_space_initial)
		* MiscFunctions::RotateTo(cache.primary_to_secondary_collision_point_initial, cache.primary_to_secondary_collision_point_current)
		* glm::translate(glm::mat4(1.0f), -cache.primary_collision_point_controller_space_initial)
		* cache.to_controller_space_initial;
}

glm::mat4 AbstractBaseObject::GetDoubleSelectionScaleDifference()
{
	// scale the object based on the ratio of initial distance between controllers to current

	float start_interesection_ray_len = glm::length(cache.primary_collision_point_world_initial - cache.secondary_collision_point_world_initial);
	float curr_intersection_ray_len = glm::length(cache.primary_collision_point_world_current - cache.secondary_collision_point_world_current);
	float intersection_ray_ratio = curr_intersection_ray_len / start_interesection_ray_len;

	return glm::translate(glm::mat4(1.0f), cache.primary_collision_point_controller_space_initial)
		* glm::scale(glm::mat4(1.0f), glm::vec3(intersection_ray_ratio))
		* glm::translate(glm::mat4(1.0f), -cache.primary_collision_point_controller_space_initial)
		* cache.to_controller_space_initial;
}

void AbstractBaseObject::SetBoundingSphere() // TODO: make this work
{
	float curr_max = 0.0f;
	for (int i = 0; i < positions.size(); ++i) 
	{
		// find length
		float curr_length;
		if (positions[i] != Constants::ZERO_VEC3) 
		{
			curr_length = glm::length(positions[i]);
		}
		else 
		{
			curr_length = 0.0f;
		}

		// test length
		if (curr_length > curr_max || i == 0)
		{
			curr_max = curr_length;
			model_max = positions[i];
		}
	}
}