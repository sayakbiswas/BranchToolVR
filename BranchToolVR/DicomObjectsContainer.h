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
	int point_size = 0;
	int iso_tolerance = DEFAULT_ISOVALUE_TOLERANCE;
	static float min_isovalue;
	static float max_isovalue;
	bool dec = false;
	glm::vec3 color;

	IsovaluePointCloudSlider(int _isovalue) : curr_isovalue(_isovalue), id(id_counter++)
	{
		Init();
	}

	IsovaluePointCloudSlider() : id(id_counter++)
	{
		Init();
	}

	void ToggleDec() {
		dec = !dec;
	}

	void Init()
	{
		this->SetInUse(false);
		point_size = 0;
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
		bool toleranceHasChanged;
		void RenderUi(Render* _r);
		void Update(const VrData& _vr, const CursorData& _crsr, Render* _r);
		void Load(std::string _dicomDir);		
		void AddObjects(Render * _r);
		void AddIsovaluePointCloudSlider(const int _isovalue);
		std::vector<IsovaluePointCloudSlider*> isovalue_point_cloud_sliders;
		// IMPORTANT: Changed from static function outside of .h file
		// to member functions
		void FileMenu();
		void MainMenuBar();

	private:
		void UpdateDicomPointCloud(int _isovalue);
		void UpdateDicomPointCloud(int _isovalue, int _tolerance);
		DicomSet imaging_data;
		DicomPointCloudObject * points;
		CoarseDicomViewer * viewer;
		Curve* curve;
		bool newCurve = false;
		int sliderCount = 0;
		int pointsToFitCount = 0;
		int pointsAlreadyFitCount = 0;
		int curr_iso_tolerance = DEFAULT_ISOVALUE_TOLERANCE;
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
