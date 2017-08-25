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

struct IsovaluePointCloudSlider
{
	ColorObject* frame;
	ColorObject* knob;
	ColorObject* x_button;
	TextureObject* value_label;
	float curr_isovalue;
	bool in_use;
	float knob_offset;
	bool knob_first_select;
	const int id;
	static int id_counter;
	static glm::vec2 frame_scale;
	static glm::vec2 knob_scale;
	static float knob_travel_dist;
	static float min_isovalue;
	static float max_isovalue;
	glm::vec3 color;

	// coarse viewer tags
	ColorObject* tag_frame;
	ColorObject* tag_x_button;
	TextureObject* tag_label;


	IsovaluePointCloudSlider(int _isovalue) : curr_isovalue(_isovalue), id(id_counter++)
	{
		Init();
	}

	IsovaluePointCloudSlider() : id(id_counter++)
	{
		Init();
	}

	void Init()
	{
		frame = new ColorObject;
		knob = new ColorObject;
		x_button = new ColorObject;
		value_label = new TextureObject;		

		tag_frame = new ColorObject;
		tag_x_button = new ColorObject;
		tag_label = new TextureObject;
		
		this->SetInUse(false);

		frame->GenerateIsovaluePointSliderFrame(frame_scale);
		frame->SetDisplayColor(glm::vec4(0.5f, 0.5f, 0.5f, 1.0f));
		frame->is_hidden = true;

		knob->GenerateIsovaluePointSliderKnob(knob_scale);
		knob->SetDisplayColor(glm::vec4(0.0f, 0.0f, 1.0f, 1.0f));
		knob->is_hidden = true;
		knob->is_selectable = true;

		x_button->GenerateIsovaluePointSliderButton(knob_scale*0.75f,glm::vec3(frame_scale.x,knob_scale.x*0.25f*0.5f,-knob_scale.y*0.25f));
		x_button->SetDisplayColor(glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
		x_button->is_hidden = true;
		x_button->is_clickable = true;

		SetWorldPosition(glm::vec3(1.0f, 0.0f, 0.0f));

		SetColor(glm::vec3(RAND_0_TO_1, RAND_0_TO_1, RAND_0_TO_1));
	}

	void MoveSliderModelPosXAndCalcIsovalue(float _x)
	{
		_x = ClampX(_x);

		knob->SetModelPositionX(_x);
		value_label->SetModelPositionX(_x);

		float curr_percent = _x / knob_travel_dist;
		curr_isovalue = curr_percent*(max_isovalue - min_isovalue) + min_isovalue;
		value_label->GenerateText(MiscFunctions::to_string_with_precision(curr_isovalue, 0), knob_scale.x, glm::vec2(0.0f), glm::vec3(0.0f, knob_scale.x*0.5f,knob_scale.x*0.5f + 0.01f));
	}

	void MoveSliderModelPosX(int _isovalue)
	{
		int iso_range = max_isovalue - min_isovalue;
		int iso_dist = _isovalue - min_isovalue;

		float iso_percent = (float)iso_dist / (float)iso_range;

		std::cout << "isoval: " << _isovalue << std::endl;
		std::cout << iso_percent << "  " << iso_percent*iso_range + min_isovalue << std::endl;
		std::cout << std::endl;

		MoveSliderModelPosXAndCalcIsovalue(iso_percent*IsovaluePointCloudSlider::knob_travel_dist);
	}

	void SetColor(const glm::vec3 _color)
	{
		color = _color;
		knob->SetDisplayColor(glm::vec4(_color, 1.0f));
	}

	void SetModelPosition(const glm::vec3& _pos)
	{
		frame->SetModelPosition(_pos);
		knob->SetModelPosition(_pos);
		x_button->SetModelPosition(_pos);
		value_label->SetModelPosition(_pos);
	}

	void SetWorldPosition(const glm::vec3& _pos)
	{
		frame->SetWorldPosition(_pos);
		knob->SetWorldPosition(_pos);
		x_button->SetWorldPosition(_pos);
		value_label->SetWorldPosition(_pos);
	}

	void SetModelPositionY(float _y)
	{
		frame->SetModelPositionY(_y);
		knob->SetModelPositionY(_y);
		x_button->SetModelPositionY(_y);
		value_label->SetModelPositionY(_y);
	}

	void SetAppendPose(glm::mat4 _in)
	{
		frame->SetAppendPose(_in);
		knob->SetAppendPose(_in);
		x_button->SetAppendPose(_in);
		value_label->SetAppendPose(_in);
	}

	void SetTagsAppendPose(glm::mat4 _in)
	{
		tag_frame->SetAppendPose(_in);
		tag_x_button->SetAppendPose(_in);
		tag_label->SetAppendPose(_in);
	}

	void SetInUse(bool _in_use)
	{
		in_use = _in_use;
		_in_use = !_in_use;
		frame->is_hidden = _in_use;
		knob->is_hidden = _in_use;
		x_button->is_hidden = _in_use;
		value_label->is_hidden = _in_use;

		tag_frame->is_hidden = _in_use;
		tag_label->is_hidden = _in_use;
		tag_x_button->is_hidden = _in_use;
	}

	float ClampX(float _x)
	{
		return glm::clamp(_x, 0.0f, IsovaluePointCloudSlider::knob_travel_dist);
	}

	~IsovaluePointCloudSlider()
	{
		delete frame;
		delete knob;
		delete x_button;
		delete value_label;
	}
};

class DicomPointCloudObject : public AbstractBaseObject
{
	public:
		DicomPointCloudObject();
		~DicomPointCloudObject();
		void Generate(DicomSet & _ds, int isovalue, int max_tolerance);
		void Generate(DicomSet & _ds, int isovalue, int max_tolerance, int first, int last);
		void SetMasterAppendPose(glm::mat4 _in);
		void SetIsovalueSliderTagAppendPose(const glm::mat4 _in);
		void Clear();
		void AddNewIsovaluePointCloudSlider(int _isovalue);
		void SetIsovaluePointCloudSlidersHeights();
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
		std::vector<IsovaluePointCloudSlider*> isovalue_point_cloud_sliders;
		static const int max_nr_isovalue_point_cloud_sliders;

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
