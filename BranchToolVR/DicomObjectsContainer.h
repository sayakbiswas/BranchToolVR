#pragma once

#include <string>

#include "DicomReader.h"
#include "CoarseDicomViewer.h"
#include "DicomPointCloudObject.h"
#include "Render.h"
#include "Ui.h"
#include "imgui.h"
#include "imgui_impl_glfw_gl3.h"
#include "Curve.h"
#include "iostream"
#include "fstream"
#include "set"

struct IsovaluePointCloudSlider
{
	float curr_isovalue;
	bool in_use;
	const int id;
	static int id_counter;
	static float min_isovalue;
	static float max_isovalue;
	glm::vec3 color;

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
		this->SetInUse(false);

		SetColor(glm::vec3(RAND_0_TO_1, RAND_0_TO_1, RAND_0_TO_1));
	}

	void SetColor(const glm::vec3 _color)
	{
		color = _color;
	}

	void SetValue(int _isovalue) {
		curr_isovalue = _isovalue;
	}

	void SetInUse(bool _in_use)
	{
		in_use = _in_use;
		_in_use = !_in_use;
	}

	~IsovaluePointCloudSlider()
	{
	}
};

class DicomObjectsContainer
{

	public:
		DicomObjectsContainer();
		~DicomObjectsContainer();
		void RenderUi();
		void Update(const VrData& _vr, const CursorData& _crsr);
		void Load(std::string _dicomDir);		
		void AddObjects(Render * _r);
		void AddIsovaluePointCloudSlider(const int _isovalue);
		std::vector<IsovaluePointCloudSlider*> isovalue_point_cloud_sliders;

	private:
		void UpdateDicomPointCloud(int _isovalue);
		DicomSet imaging_data;
		DicomPointCloudObject * points;
		CoarseDicomViewer * viewer;
		Curve* curve;
		bool newCurve = false;
		int pointsToFitCount = 0;
		int pointsAlreadyFitCount = 0;
		static void ShowHelpMarker(const char* desc)
		{
			ImGui::TextDisabled("(?)");
			if (ImGui::IsItemHovered())
			{
				ImGui::BeginTooltip();
				ImGui::PushTextWrapPos(450.0f);
				ImGui::TextUnformatted(desc);
				ImGui::PopTextWrapPos();
				ImGui::EndTooltip();
			}
		}
		//static void MainMenuBar() {};
		//static void FileMenu() {};
};
