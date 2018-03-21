#include "CoarseDicomViewer.h"

CoarseDicomViewer::CoarseDicomViewer()
{
	static_mesh = new ColorObject;
	static_mesh->SetDisplayColor(Constants::STATIC_DICOM_MESH_COLOR);

	#if !ENABLE_STATIC_MESH
		static_mesh->is_hidden = true;
	#endif
	
	float base_handle_scale = 1.0f;
	selector = new TextureObject;
	selector->readObjFromFile(DirectoryInfo::POINT_CLOUD_SELECTOR_MODEL, base_handle_scale, glm::vec3(0.0f));
	selector->is_selectable = true;
	selector->is_double_selectable = true;
	selector->texture_level = POINT_CLOUD_SELECTOR_TEXTURE;

	/*orthoslice_handle = new TextureObject;
	orthoslice_handle->readObjFromFile(DirectoryInfo::COARSE_VIEWER_SLICE_HANDLE_MODEL,1.0f, glm::vec3(0.5f, 0.5f, 0.0f));
	orthoslice_handle->is_selectable = true;
	orthoslice_handle->texture_level = COARSE_VIEWER_SLICE_HANDLE_TEXTURE;*/

	orthoslice = new TextureObject;
	orthoslice->is_clickable = true;
	orthoslice->GenerateXYPlane(1.0f, 1.0f, 0.0f, glm::vec3(0.0f));
	orthoslice->texture_level = CURR_ORTHOSLICE_TEXTURE;
	orthoslice_texture = new Texture;
	selector_changed = false;

	/*point_cloud_selector = new TextureObject;
	point_cloud_selector->is_selectable = true;
	point_cloud_selector->is_double_selectable = true;
	//point_cloud_selector->GenerateXYPrism(glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2(0.0f), glm::vec3(-0.5f)); // OLD CUBE BEFORE NEW MODEL
	point_cloud_selector->readObjFromFile(DirectoryInfo::POINT_CLOUD_SELECTOR_MODEL, 1.0f*0.6f); // TODO: FIX MODEL SCALE WITH LOWER BOUND AND UPPER BOUNDS
	point_cloud_selector->texture_level = 5; // POINT_CLOUD_SELECTOR_TEXTURE
	point_cloud_selector->SetScale(DEFAULT_SELECTOR_SCALE);
	point_cloud_selector_scale = DEFAULT_SELECTOR_SCALE;*/
}

CoarseDicomViewer::~CoarseDicomViewer()
{
	delete static_mesh;
	delete selector;
	//delete orthoslice_handle;
	delete orthoslice;
	//delete point_cloud_selector;
}

void CoarseDicomViewer::AddObjects(Render * _r) 
{
	_r->AddObjectToScene(static_mesh);
	_r->AddObjectToScene(selector);
	//_r->AddObjectToScene(orthoslice_handle);
	//_r->AddObjectToScene(point_cloud_selector);
	//_r->AddObjectToScene(orthoslice);
	//_r->SetOrthosliceTextureReference(orthoslice_texture);
}

//glm::vec3 CoarseDicomViewer::GetCollisionPointWithOrthosliceHandlePlane(glm::vec3 _pos, glm::vec3 _ray, glm::vec3 & _cg) 
//{
//	glm::mat4 mm = base_handle->GetModelMatrix();
//	glm::vec4 tri[3];
//	tri[0] = mm * glm::vec4(glm::vec3(0.0f,0.0f,0.0f),1.0f);
//	tri[1] = mm * glm::vec4(glm::vec3(1.0f,0.0f,0.0f),1.0f);
//	tri[2] = mm * glm::vec4(glm::vec3(1.0f,0.0f,1.0f),1.0f);
//	MiscFunctions::ray_triangle_intersection(glm::value_ptr(tri[0]), glm::value_ptr(tri[1]), glm::value_ptr(tri[2]), glm::value_ptr(_pos), glm::value_ptr(_ray), glm::value_ptr(_cg));
//	_cg = glm::vec3(_cg.x * tri[0] + _cg.y * tri[1] + _cg.z * tri[2]);
//	
//	return glm::vec3(glm::inverse(mm) * glm::vec4(_cg, 1.0f));
//}
//
//void CoarseDicomViewer::SetMasterAppendTranslate(glm::vec3 & _inWorldPos) 
//{
//	SetMasterAppendPose(glm::translate(glm::mat4(1.0f), _inWorldPos));
//}

void CoarseDicomViewer::Load(DicomSet & _dSet) 
{
	static_mesh->GenerateIsosurfaceFromDicomSet(_dSet, _dSet.isovalue);
	orthoslice_texture->Load(_dSet.data[0], _dSet.window_width, _dSet.window_center);
	orthoslice->texture_level = CURR_ORTHOSLICE_TEXTURE;
}

void CoarseDicomViewer::SetMasterAppendPose(glm::mat4 _append)
{
	selector->SetAppendPose(_append);
	static_mesh->SetAppendPose(_append);
	//orthoslice_handle->SetAppendPose(_append);
	//orthoslice->SetAppendPose(_append);
	//point_cloud_selector->SetAppendPose(_append);
}

void CoarseDicomViewer::SetMasterAppendScale(float _scale)
{
	SetMasterAppendPose(glm::scale(glm::mat4(1.0f), glm::vec3(_scale))* selector->GetAppendPose());
}
