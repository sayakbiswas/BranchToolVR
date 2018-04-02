#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <string>
#include <fstream>
#include <streambuf>
#include <vector>
#include <openvr.h>
#include <stdio.h>
#include <cstdlib>

#include "ColorObject.h"
#include "LineObject.h"
#include "DicomPointCloudObject.h"
#include "AbstractBaseObject.h"
#include "TextureObject.h"
#include "Texture.h"
#include "RenderModel.h"
#include "MiscFunctions.h"
#include "InputHelpers.h"
#include "Constants.h"
#include "Ui.h"
#include "Curve.h"

struct Light 
{
	glm::vec3 position;
	ColorObject marker;
	float intensity;

	Light()
	{
		marker.GenerateSphere(11, 0.2f, true);
	}

	Light(float _radius) 
	{
		marker.GenerateSphere(11, _radius, true);
	}
};

struct ShadowMap
{
	GLuint depth;
	GLuint fbo;
	glm::mat4 P;
	glm::mat4 V;
};

enum TexturesEnum 
{
	WOOD_TEXTURE,
	FONT_TEXTURE,
	CURR_ORTHOSLICE_TEXTURE,
	POINT_CLOUD_FRAME_TEXTURE,
	COARSE_VIEWER_SLICE_HANDLE_TEXTURE,
	POINT_CLOUD_SELECTOR_TEXTURE,
	CURR_NR_TEXTURES, // keep as last element of enum
};

struct ShaderProgram 
{
	GLuint id;
	GLuint* uniforms;
	GLuint num_uniforms;
};

struct FramebufferDesc 
{
	GLuint m_nDepthBufferId;
	GLuint m_nRenderTextureId;
	GLuint m_nRenderFramebufferId;
	GLuint m_nResolveTextureId;
	GLuint m_nResolveFramebufferId;
};

class Render
{
	public:
		Render(GLFWwindow* _window);
		~Render();
		bool hmd_ready;
		void SceneLoop();		
		void AddObjectToScene(std::vector<AbstractBaseObject*> bsos);
		void AddObjectToScene(AbstractBaseObject* abso);
		void AddObjectToScene(ColorObject * bso);
		void AddObjectToScene(DicomPointCloudObject * dpco);
		void AddObjectToScene(LineObject * l);
		void AddObjectToScene(TextureObject * t);
		void SetOrthosliceTextureReference(Texture* _t);
		const CursorData& GetCursorData() { return cursor_info; };
		const VrData& GetVrData() { return vr_info; };

	private:
		void Interact();
		void UpdateHMDMatrixPose();		
		void UpdateCursor();
		void SpoofInput(int controllerIndex);
		void DetectCollision(VrMotionController & _controller);
		void BindTextures();
		void LoadTextures();
		void LoadShaders();		
		void InitLighting();
		void RenderToWindow();
		void UpdateScene();
		bool InitVR();
		void RenderToHMD();
		bool createShadowMap(ShadowMap &sm);
		void RenderSceneInternal(glm::mat4 _P, glm::mat4 _V);

		// glfw reference (must be initialized before constructor)
		GLFWwindow* window;

		// Textures
		Texture** textures;

		// Lights
		static const int num_lights;
		Light * lights;
		void UpdateLights();

		// shadows
		ShadowMap sm;
		void RenderShadows();

		// shader programs
		ShaderProgram color;
		ShaderProgram texture;
		ShaderProgram line;
		ShaderProgram dicom_point_cloud;
		ShaderProgram branch_point;
		ShaderProgram branch_line;
		ShaderProgram shadow;
		ShaderProgram recieve_shadow_color;
		ShaderProgram curve;
		
		// object containers
		std::vector<AbstractBaseObject*> all_objects;
		std::vector<ColorObject*> color_objects;
		std::vector<TextureObject*> texture_objects;
		std::vector<LineObject*> line_objects;
		std::vector<DicomPointCloudObject*> dicom_point_cloud_objects;

		// VR variables
		FramebufferDesc leftEyeDesc;
		FramebufferDesc rightEyeDesc;
		vr::IVRSystem* m_pHMD;
		vr::IVRRenderModels* m_pRenderModels;
		vr::TrackedDevicePose_t m_rTrackedDevicePose[vr::k_unMaxTrackedDeviceCount];
		bool m_rbShowTrackedDevice[vr::k_unMaxTrackedDeviceCount];
		glm::mat4 m_rmat4DevicePose[vr::k_unMaxTrackedDeviceCount];
		glm::mat4 m_mat4HMDPose;
		uint32_t m_nRenderWidth;
		uint32_t m_nRenderHeight;
		
		// motion controller
		ColorObject* controller_pointer1;
		ColorObject* controller_pointer2;
		AbstractBaseObject* selected_element1;
		AbstractBaseObject* selected_element2;
		glm::mat4 spoofControllerView;
		
		// valve models
		std::vector<CGLRenderModel*> m_vecRenderModels;
		CGLRenderModel* m_rTrackedDeviceToRenderModel[vr::k_unMaxTrackedDeviceCount];
		CGLRenderModel* FindOrLoadRenderModel(const char *pchRenderModelName);
		void SetupRenderModels();
		void SetupRenderModelForTrackedDevice(vr::TrackedDeviceIndex_t unTrackedDeviceIndex);
		
		// static vars
		static int window_size_x;
		static int window_size_y;
		static int half_window_size_x;
		static int half_window_size_y;
		static float window_aspect;
		static glm::mat4 window_projection;
		static VrData vr_info;
		static CursorData cursor_info;

		// static functions
		static void window_size_callback(GLFWwindow* window, int width, int height); // glfw callback
		static glm::mat4 ValveMat34ToGlmMat4Inv(vr::HmdMatrix34_t _mIN);
		static glm::mat4 ValveMat34ToGlmMat4(vr::HmdMatrix34_t _mIN);
		static glm::mat4 ValveMat4ToGlmMat4(vr::HmdMatrix44_t _mIN);
		static glm::mat4 ValveMat4ToGlmMat4Inv(vr::HmdMatrix44_t _mIN);
		static bool CreateFrameBuffer(int nWidth, int nHeight, FramebufferDesc &framebufferDesc);
		static std::string ReadFile(std::string _filePath);
		static GLuint CompileGLShader(std::string programName);
		static GLuint CreateShader(GLint target, std::string& src);
};