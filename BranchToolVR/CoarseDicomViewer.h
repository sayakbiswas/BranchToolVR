#pragma once

#include "Render.h"
#include "ColorObject.h"
#include "TextureObject.h"
#include "DicomReader.h"

// TODO: privatize class members (requires const refactor)

class CoarseDicomViewer {
	public:
		CoarseDicomViewer();
		~CoarseDicomViewer();		
		void Load(DicomSet & _dSet);			
		void AddObjects(Render * _r);	
		void SetMasterAppendPose(glm::mat4 _append);
		void SetMasterAppendScale(float _scale);

		ColorObject* static_mesh;		
		//TextureObject* point_cloud_selector;
		TextureObject* selector;
		//TextureObject* orthoslice_handle;
		TextureObject* orthoslice;
		Texture * orthoslice_texture;

		TextureObject * ControllerDiagram;
		Texture * ControllerImage;

		bool selector_changed;
		float point_cloud_selector_scale;
		glm::vec3 selector_lower_bounds;
		glm::vec3 selector_upper_bounds;
};
