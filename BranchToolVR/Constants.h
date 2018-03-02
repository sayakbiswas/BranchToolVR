#pragma once

#include <string>
#include <glm/glm.hpp>
#include <Windows.h>

// single value default constants
#define DEFAULT_WINDOW_SIZE_X GetSystemMetrics(SM_CXSCREEN)/2
#define DEFAULT_WINDOW_SIZE_Y GetSystemMetrics(SM_CYSCREEN)/2
#define DEFAULT_NR_SAMPLES 0
#define DEFAULT_ISOVALUE 1340
#define DEFAULT_ISOVALUE_TOLERANCE 10
#define DEFAULT_SHADOW_RES 2048
#define DEFAULT_WINDOW_CENTER 1191
#define DEFAULT_WINDOW_WIDTH 1066
#define DEFAULT_SELECTOR_SCALE 0.2f
#define DEFAULT_POINT_CLOUD_SCALE 1.0f
#define DEFAULT_LINE_WIDTH 2.0f
#define MAX_ISOVALUE_TOLERANCE 30
#define MAX_NR_POINT_CLOUD_SLIDERS 10
#define WINDOW_TITLE_STRING "BranchToolVR"
#define TMP_MIN_ISOVALUE 0
#define TMP_MAX_ISOVALUE 3000
#define TMP_MIN_WW 100
#define TMP_MAX_WW 2000

// enablers
#define ENABLE_BACKFACE_CULLING 1
#define ENABLE_STATIC_MESH 0

// math constants
#define TWOPI 6.28318530718f
#define PI 3.14159265359f
#define SQRT3 1.73205080757f
#define RAD_TO_DEGREE 57.2957795131
#define DEGREE_TO_RAD 0.01745329251

// macros
#define RAND_0_TO_1 static_cast <float> (rand()) / static_cast <float> (RAND_MAX)

namespace DirectoryInfo
{
	static const std::string RELATIVE_RESOURCES_PATH = "..//../BranchToolVR/Resources/";
	static const std::string RELATIVE_SHADERS_DIR = RELATIVE_RESOURCES_PATH + "../Shaders/";
	static const std::string RELATIVE_TEXTURES_DIR = RELATIVE_RESOURCES_PATH + "Textures/";
	static const std::string RELATIVE_MODELS_DIR = RELATIVE_RESOURCES_PATH + "Models/";
	static const std::string RELATIVE_DICOM_DIR = RELATIVE_RESOURCES_PATH + "DICOM/";
	static const std::string STARTUP_DICOM_SET = "torso/dcm0";
	static const std::string POINT_CLOUD_HANDLE_MODEL = "pointCloudFrame2";
	static const std::string COARSE_VIEWER_SLICE_HANDLE_MODEL = "coarseViewerHandle2";
	static const std::string POINT_CLOUD_SELECTOR_MODEL = "pointCloudSelector";
	static const std::string IMGUI_FRAME_MODEL = "imguiFrame";
	static const std::string MODEL_EXT = ".wobj"; // rename from obj to avoid conflicts with obj files which are set in gitignore
}

namespace Constants 
{
	static const glm::vec3 DICOM_PANEL_DIMENSIONS = glm::vec3(1.5f, 0.5f, 0.025f);
	static const glm::vec4 UI_SELECTOR_COLOR = glm::vec4(0.25f, 0.25f, 1.0f,0.5f);
	static const glm::vec4 STATIC_DICOM_MESH_COLOR = glm::vec4(0.89f, 0.3f, 0.89f, 1.0f);
	static const glm::vec4 BASE_BRANCH_POINT_COLOR = glm::vec4(1.0f, 0.2f, 0.4f, 1.0f);
	static const glm::vec4 SELECTED_BRANCH_POINT_COLOR = glm::vec4(0.9f, 0.3f, 0.9f, 1.0f);
	static const glm::vec4  HOVER_BRANCH_POINT_COLOR = glm::vec4(0.9f, 0.9f, 0.3f, 1.0f);
	static const glm::vec4 CLEAR_COLOR = glm::vec4(0.5f, 0.5f, 1.0f, 1.0f);
	static const glm::vec3 AMBIENT_LIGHT = glm::vec3(0.1f, 0.1f, 0.1f);
	static const glm::vec3 X_AXIS = glm::vec3(1.0f,0.0f,0.0f);
	static const glm::vec3 Y_AXIS = glm::vec3(0.0f,1.0f,0.0f);
	static const glm::vec3 Z_AXIS = glm::vec3(0.0f,0.0f,1.0f);
	static const glm::vec3 ZERO_VEC3 = glm::vec3(0.0f, 0.0f, 0.0f);
}