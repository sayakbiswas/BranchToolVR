#include "CoarseDicomViewer.h"

CoarseDicomViewer::CoarseDicomViewer()
{
	static_mesh = new ColorObject;
	static_mesh->SetDisplayColor(Constants::STATIC_DICOM_MESH_COLOR);

	#if !ENABLE_STATIC_MESH
		static_mesh->is_hidden = true;
	#endif
	
	// Removed commented out TexObj additions; structure mimiced that of below code
	selector = new TextureObject;
	selector->readObjFromFile(DirectoryInfo::POINT_CLOUD_SELECTOR_MODEL, BASE_SELECTOR_SCALE, glm::vec3(0.0f));
	selector->is_selectable = true;
	selector->is_double_selectable = true;
	selector->texture_level = POINT_CLOUD_SELECTOR_TEXTURE;
	//for (int i = 0; i < selector->positions.size(); i++)
	//	std::cout << selector->positions.at(i).x << " " << selector->positions.at(i).y << " " << selector->positions.at(i).z << std::endl;

	orthoslice = new TextureObject;
	orthoslice->is_clickable = true;
	orthoslice->GenerateXYPlane(1.0f, 1.0f, 0.0f, glm::vec3(0.0f));
	orthoslice->texture_level = CURR_ORTHOSLICE_TEXTURE;
	orthoslice_texture = new Texture;
	selector_changed = false;

	ControllerDiagram = new TextureObject;
	ControllerDiagram->is_clickable = true;
	ControllerDiagram->GenerateXYPlane(1.0f, 1.0f, 0.0f, glm::vec3(0.0f));
	ControllerDiagram->texture_level = CONTROLLER;
	ControllerImage = new Texture;
	ControllerImage->Load("Controller");
}

CoarseDicomViewer::~CoarseDicomViewer()
{
	delete static_mesh;
	delete selector;
	delete orthoslice;
	delete ControllerDiagram;
}

void CoarseDicomViewer::AddObjects(Render * _r) 
{
	_r->AddObjectToScene(static_mesh);
	_r->AddObjectToScene(selector);
}

void CoarseDicomViewer::Load(DicomSet & _dSet) 
{
	std::cout << _dSet.data[0].width << std::endl;
	static_mesh->GenerateIsosurfaceFromDicomSet(_dSet, _dSet.isovalue);
	orthoslice_texture->Load(_dSet.data[0], _dSet.window_width, _dSet.window_center);
	orthoslice->texture_level = CURR_ORTHOSLICE_TEXTURE;
}

void CoarseDicomViewer::SetMasterAppendPose(glm::mat4 _append)
{
	selector->SetAppendPose(_append);
	static_mesh->SetAppendPose(_append);
}

void CoarseDicomViewer::SetMasterAppendScale(float _scale)
{
	SetMasterAppendPose(glm::scale(glm::mat4(1.0f), glm::vec3(_scale))* selector->GetAppendPose());
}
