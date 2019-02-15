#pragma once

#include <GL/glew.h>
#include <glm/gtc/matrix_access.hpp>
#include <GLFW/glfw3.h>
#include <GLM/glm.hpp>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <stdexcept>
#include <openvr.h>
#include <chrono>

#include "Render.h"
#include "ColorObject.h"
#include "Axis.h"
#include "DicomReader.h"
#include "DicomObjectsContainer.h"
#include "Constants.h"
#include "imgui.h"
#include "imgui_impl_glfw_gl3.h"

class Engine
{
	public:
		Engine();
		~Engine();

	private:
		bool InitGLFW();
		bool InitGLEW();		
		void InitObjects();
		void Loop();
		void Update();

		GLFWwindow * window;		
		Render * renderer; // must be allocated after window is initialized		
		DicomObjectsContainer * doc;

};
