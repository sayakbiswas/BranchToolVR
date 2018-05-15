#include "Render.h"

// init static variables
int Render::window_size_x = DEFAULT_WINDOW_SIZE_X;
int Render::window_size_y = DEFAULT_WINDOW_SIZE_Y;
int Render::half_window_size_x = DEFAULT_WINDOW_SIZE_X/2;
int Render::half_window_size_y = DEFAULT_WINDOW_SIZE_Y/2;
float Render::window_aspect = (float)window_size_x / (float)window_size_y;
glm::mat4 Render::window_projection = glm::perspective(90.0f, window_aspect, 0.01f, 100.0f);
const int Render::num_lights = 3; // TODO: variable number of lights. Currently hardcoded in all shaders for 3.
VrData Render::vr_info;
CursorData Render::cursor_info;

Render::Render(GLFWwindow *_window)
{
	// configure glfw vars
	window = _window;
	glfwSetWindowSizeCallback(window, Render::window_size_callback);

	// set clears
	const glm::vec4& c = Constants::CLEAR_COLOR;
	glClearColor(c.x, c.y, c.z, c.w);
	glClearDepthf(1.0f);

	// enable alpha
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);
	
	// culling
	#if ENABLE_BACKFACE_CULLING
		glEnable(GL_CULL_FACE);
		glFrontFace(GL_CCW);
		glCullFace(GL_BACK);
	#endif

	// initialize controller objects
	controller_pointer1 = new ColorObject;
	controller_pointer1->GenerateController();
	AddObjectToScene(controller_pointer1);

	controller_pointer2 = new ColorObject;
	controller_pointer2->GenerateController();
	AddObjectToScene(controller_pointer2);
	controller_pointer2->SetDisplayColor(glm::vec4(0.5f, 0.25f, 0.1f, 1.0f));

	selected_element1 = NULL;
	selected_element2 = NULL;

	// line properties
	glLineWidth(DEFAULT_LINE_WIDTH);
	
	// initializers
	InitVR();
	InitLighting();
	LoadShaders();
	LoadTextures();
}

Render::~Render()
{
	for (int i = 0; i < CURR_NR_TEXTURES; ++i)
	{
		delete textures[i];
	}
	delete textures;
}

void Render::window_size_callback(GLFWwindow* window, int width, int height)
{
	window_size_x = width;
	window_size_y = height;
	half_window_size_x = width/2;
	half_window_size_y = height/2;
	window_aspect = (float)window_size_x / (float)window_size_y;
	window_projection = glm::perspective(90.0f, window_aspect, 0.01f, 100.0f);
}

void Render::InitLighting()
{
	// shadows
	createShadowMap(sm);
	float shadow_width = 5.0f;
	sm.P = glm::ortho(-shadow_width, shadow_width, -shadow_width, shadow_width, -100.0f, 100.0f);
	sm.V = glm::lookAt(glm::vec3(2.5f, 20.0f, 2.5f), glm::vec3(-0.1f, 0.0f, -0.1f), glm::vec3(0.0f, 1.0f, 0.0f));

	// moveable lights
	lights = new Light[num_lights];
	for (int i = 0; i < num_lights; ++i)
	{
		lights[i].marker.is_selectable = true;
		lights[i].marker.SetDisplayColor(glm::vec4(1.0f, 0.8f, 0.3f, 1.0f));
		AddObjectToScene(&lights[i].marker);
	}

	lights[0].position = glm::vec3(0.0f, 0.5f, 0.0f);
	lights[0].marker.SetWorldPosition(lights[0].position);

	lights[1].position = glm::vec3(1.0f, 0.0f, 0.0f);
	lights[1].marker.SetWorldPosition(lights[1].position);

	lights[2].position = glm::vec3(0.0f, 0.0f, 2.0f);
	lights[2].marker.SetWorldPosition(lights[2].position);
}

bool Render::InitVR() 
{
	// Loading the SteamVR Runtime
	vr::EVRInitError eError = vr::VRInitError_None;
	m_pHMD = vr::VR_Init(&eError, vr::VRApplication_Scene);

	if (eError != vr::VRInitError_None)
	{
		m_pHMD = NULL;
		char buf[1024];
		sprintf_s(buf, sizeof(buf), "Unable to init VR runtime: %s", vr::VR_GetVRInitErrorAsEnglishDescription(eError));
		vr_info.hmd_connected = false;
		return false;
	}

	m_pRenderModels = (vr::IVRRenderModels *)vr::VR_GetGenericInterface(vr::IVRRenderModels_Version, &eError);
	
	if (!m_pRenderModels)
	{
		m_pHMD = NULL;
		vr::VR_Shutdown();
		char buf[1024];
		sprintf_s(buf, sizeof(buf), "Unable to get render model interface: %s", vr::VR_GetVRInitErrorAsEnglishDescription(eError));
		return false;
	}

	m_pHMD->GetRecommendedRenderTargetSize(&m_nRenderWidth, &m_nRenderHeight);

	CreateFrameBuffer(m_nRenderWidth, m_nRenderHeight, leftEyeDesc);
	CreateFrameBuffer(m_nRenderWidth, m_nRenderHeight, rightEyeDesc);
	
	vr_info.hmd_connected = true;

	SetupRenderModels();

	return true;
}

void Render::AddObjectToScene(std::vector<AbstractBaseObject*> abstractBaseObjs) 
{
	for (AbstractBaseObject* aobj : abstractBaseObjs) 
	{
		AddObjectToScene(aobj);
	}
}

void Render::AddObjectToScene(AbstractBaseObject* _abso) 
{
	if (_abso == NULL)	return;

	if (_abso->Type() == 0) 
	{
		ColorObject * s = static_cast<ColorObject*>(_abso);
		AddObjectToScene(s);
	}
	else if (_abso->Type() == 1) 
	{
		TextureObject * s = static_cast<TextureObject*>(_abso);
		AddObjectToScene(s);
	}

	all_objects.push_back(_abso);
}

void Render::AddObjectToScene(ColorObject* _co)
{
	if (_co != NULL) 
	{
		color_objects.push_back(_co);
		all_objects.push_back(_co);
	}
}

void Render::AddObjectToScene(DicomPointCloudObject * _dpco)
{
	if (_dpco == NULL) 
		return;

	all_objects.push_back(_dpco); 
	dicom_point_cloud_objects.push_back(_dpco);
}

void Render::AddObjectToScene(LineObject * _l) 
{
	if (_l != NULL)
		line_objects.push_back(_l);
}

void Render::AddObjectToScene(TextureObject * _t) 
{
	if (_t == NULL)
		return;

	texture_objects.push_back(_t);
	all_objects.push_back(_t);
}

void Render::SetOrthosliceTextureReference(Texture* _t)
{
	// not the best solution...
	textures[CURR_ORTHOSLICE_TEXTURE] = _t;
}

void Render::RenderToHMD()
{
	if (!m_pHMD || !hmd_ready)
		return;

	// left eye
	glBindFramebuffer(GL_FRAMEBUFFER, leftEyeDesc.m_nRenderFramebufferId);
	glViewport(0, 0, m_nRenderWidth, m_nRenderHeight);
	RenderSceneInternal(vr_info.left_eye_proj, vr_info.left_eye_transform_inv);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glDisable(GL_MULTISAMPLE);

	glBindFramebuffer(GL_READ_FRAMEBUFFER, leftEyeDesc.m_nRenderFramebufferId);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, leftEyeDesc.m_nResolveFramebufferId);

	glBlitFramebuffer(0, 0, m_nRenderWidth, m_nRenderHeight, 0, 0, m_nRenderWidth, m_nRenderHeight,GL_COLOR_BUFFER_BIT,GL_LINEAR);

	glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
	
	// right eye
	glBindFramebuffer(GL_FRAMEBUFFER, rightEyeDesc.m_nRenderFramebufferId);
	glViewport(0, 0, m_nRenderWidth, m_nRenderHeight);
	RenderSceneInternal(vr_info.right_eye_proj, vr_info.right_eye_transform_inv);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glDisable(GL_MULTISAMPLE);

	glBindFramebuffer(GL_READ_FRAMEBUFFER, rightEyeDesc.m_nRenderFramebufferId);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, rightEyeDesc.m_nResolveFramebufferId);

	glBlitFramebuffer(0, 0, m_nRenderWidth, m_nRenderHeight, 0, 0, m_nRenderWidth, m_nRenderHeight, GL_COLOR_BUFFER_BIT, GL_LINEAR);

	glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

	// send to HMD

	vr::Texture_t leftEyeTexture = { (void*)leftEyeDesc.m_nResolveTextureId, vr::API_OpenGL, vr::ColorSpace_Gamma };
	vr::VRCompositor()->Submit(vr::Eye_Left, &leftEyeTexture);
	
	vr::Texture_t rightEyeTexture = { (void*)rightEyeDesc.m_nResolveTextureId, vr::API_OpenGL, vr::ColorSpace_Gamma };
	vr::VRCompositor()->Submit(vr::Eye_Right, &rightEyeTexture);
}

void Render::UpdateLights() 
{
	for (int i = 0; i < num_lights; ++i) 
	{
		if (lights[i].marker.is_selected) 
		{
			lights[i].marker.SetAppendPose(lights[i].marker.cache.controller_pose_updated *lights[i].marker.cache.to_controller_space_initial);
			lights[i].position = glm::vec3(lights[i].marker.GetModelMatrix()*glm::vec4(0.0f,0.0f,0.0f,1.0f));
		}
	}
}

void Render::UpdateCursor() 
{	
	// position
	double xpos, ypos;
	glfwGetCursorPos(window, &xpos, &ypos);

	cursor_info.normalized_cursor_position.x = 2.0f*xpos / (float)window_size_x - 1.0f;
	cursor_info.normalized_cursor_position.y = 2.0f*(window_size_y - ypos) / (float)window_size_y - 1.0f;

	// buttons
	static bool first_press = true;

	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_1)) 
	{
		cursor_info.is_pressed = true;
		
		if (first_press) 
		{
			cursor_info.first_press = true;
			first_press = false;
		}
		else 
		{
			cursor_info.first_press = false;
		}
	}
	else 
	{
		first_press = true;
		cursor_info.first_press = false;
		cursor_info.is_pressed = false;
	}
}

void Render::RenderShadows() 
{
	// cull front faces
	glEnable(GL_CULL_FACE);
	glFrontFace(GL_CCW);
	glCullFace(GL_FRONT);

	glBindFramebuffer(GL_FRAMEBUFFER, sm.depth);
	glDisable(GL_SCISSOR_TEST);

	glViewport(0, 0, DEFAULT_SHADOW_RES, DEFAULT_SHADOW_RES);
	glClear(GL_DEPTH_BUFFER_BIT);

	glUseProgram(shadow.id);
	glUniformMatrix4fv(shadow.uniforms[0], 1, GL_FALSE, glm::value_ptr(sm.P));
	glUniformMatrix4fv(shadow.uniforms[1], 1, GL_FALSE, glm::value_ptr(sm.V));

	// render objects that cast shadow below:

	for (ColorObject* & cObj : color_objects) 
	{
		if (cObj->is_hidden || !cObj->is_loaded)
			continue;

		glUniformMatrix4fv(shadow.uniforms[2], 1, GL_FALSE, glm::value_ptr(cObj->GetModelMatrix()));

		glBindVertexArray(cObj->GetVao());
		glEnableVertexAttribArray(0);

		glDrawArrays(GL_TRIANGLES, 0, cObj->GetNumVertices());
	}

	//////////////////////////////////////////////////////////////////////////////////////////////////

	for (TextureObject* & tObj : texture_objects) 
	{
		if (tObj->is_hidden || !tObj->is_loaded)
			continue;

		glUniformMatrix4fv(shadow.uniforms[2], 1, GL_FALSE, glm::value_ptr(tObj->GetModelMatrix()));

		glBindVertexArray(tObj->GetVao());
		glEnableVertexAttribArray(0);

		glDrawArrays(GL_TRIANGLES, 0, tObj->GetNumVertices());
	}

	////////////////////////////////////////////////////////////////////////////////////////////////

	glUseProgram(dicom_point_cloud.id);

	glUniformMatrix4fv(dicom_point_cloud.uniforms[0], 1, GL_FALSE, glm::value_ptr(sm.P));
	glUniformMatrix4fv(dicom_point_cloud.uniforms[1], 1, GL_FALSE, glm::value_ptr(sm.V));
	glUniform3fv(dicom_point_cloud.uniforms[4], 1, glm::value_ptr(lights[0].position));
	glUniform3fv(dicom_point_cloud.uniforms[5], 1, glm::value_ptr(lights[1].position));
	glUniform3fv(dicom_point_cloud.uniforms[6], 1, glm::value_ptr(lights[2].position));
	glUniform3fv(dicom_point_cloud.uniforms[12], 1, glm::value_ptr(vr_info.head_position));

	for (DicomPointCloudObject*& dpco : dicom_point_cloud_objects) 
	{
		if (!dpco->first_load) continue;

		glUniformMatrix4fv(dicom_point_cloud.uniforms[2], 1, GL_FALSE, glm::value_ptr(dpco->GetModelMatrix()));
		glUniform1i(dicom_point_cloud.uniforms[3], dpco->curr_tolerance);
		glUniform3fv(dicom_point_cloud.uniforms[7], 1, glm::value_ptr(glm::vec3(dpco->GetScale())));
		glUniform3fv(dicom_point_cloud.uniforms[8], 1, glm::value_ptr(dpco->lower_bounds));
		glUniform3fv(dicom_point_cloud.uniforms[9], 1, glm::value_ptr(dpco->upper_bounds));

		glBindVertexArray(dpco->GetVao());

		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glEnableVertexAttribArray(2);
		glEnableVertexAttribArray(3);
		glEnableVertexAttribArray(4);
		glVertexAttribDivisor(0, 0);
		glVertexAttribDivisor(1, 0);
		glVertexAttribDivisor(2, 1);
		glVertexAttribDivisor(3, 1);
		glVertexAttribDivisor(4, 1);

		glDrawArraysInstanced(GL_TRIANGLES, 0, dpco->GetNumVertices(), dpco->GetNumInstances());
	}

	////////////////////////////////////////////////////////////////////////////////////////////////

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glEnable(GL_SCISSOR_TEST);
	glDisable(GL_CULL_FACE);
}

void Render::RenderToWindow()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, window_size_x, window_size_y);
	RenderSceneInternal(window_projection, vr_info.head_pose_inv);
}

void Render::UpdateScene()
{
	UpdateCursor();
	UpdateLights();	
	UpdateHMDMatrixPose();
	//RenderShadows(); // TEMP: shadows are disabled
	Interact();
}

void Render::SceneLoop() 
{
	UpdateScene();
	RenderToWindow();
	RenderToHMD();
}

void Render::BindTextures() 
{
	for (int i = 0; i < CURR_NR_TEXTURES; ++i)
	{	
		textures[i]->Bind(i);
	}
}

void Render::LoadTextures()
{
	textures = new Texture*[CURR_NR_TEXTURES];
	for (int i = 0; i < CURR_NR_TEXTURES; ++i)
	{
		textures[i] = new Texture;
	}

	textures[WOOD_TEXTURE]->Load("Wood");
	textures[FONT_TEXTURE]->Load("fontGlyph4096");
	textures[COARSE_VIEWER_SLICE_HANDLE_TEXTURE]->Load("coarseViewerTexture");
	textures[POINT_CLOUD_FRAME_TEXTURE]->Load("pointCloudFrame");
	textures[POINT_CLOUD_SELECTOR_TEXTURE]->Load("pointCloudSelectorCube");
}

void Render::LoadShaders()
{
	color.id = CompileGLShader("color");
	color.num_uniforms = 11;
	color.uniforms = new GLuint[color.num_uniforms];
	color.uniforms[0] = glGetUniformLocation(color.id, "P");
	color.uniforms[1] = glGetUniformLocation(color.id, "V");
	color.uniforms[2] = glGetUniformLocation(color.id, "M");
	color.uniforms[3] = glGetUniformLocation(color.id, "color");
	color.uniforms[4] = glGetUniformLocation(color.id, "lights[0].pos");
	color.uniforms[5] = glGetUniformLocation(color.id, "lights[1].pos");
	color.uniforms[6] = glGetUniformLocation(color.id, "lights[2].pos");
	color.uniforms[7] = glGetUniformLocation(color.id, "shadow");
	color.uniforms[8] = glGetUniformLocation(color.id, "shadowP");
	color.uniforms[9] = glGetUniformLocation(color.id, "shadowV");
	color.uniforms[10] = glGetUniformLocation(color.id, "ambient");

	recieve_shadow_color.id = CompileGLShader("recieve_shadow_color");
	recieve_shadow_color.num_uniforms = 11;
	recieve_shadow_color.uniforms = new GLuint[color.num_uniforms];
	recieve_shadow_color.uniforms[0] = glGetUniformLocation(color.id, "P");
	recieve_shadow_color.uniforms[1] = glGetUniformLocation(color.id, "V");
	recieve_shadow_color.uniforms[2] = glGetUniformLocation(color.id, "M");
	recieve_shadow_color.uniforms[3] = glGetUniformLocation(color.id, "color");
	recieve_shadow_color.uniforms[4] = glGetUniformLocation(color.id, "lights[0].pos");
	recieve_shadow_color.uniforms[5] = glGetUniformLocation(color.id, "lights[1].pos");
	recieve_shadow_color.uniforms[6] = glGetUniformLocation(color.id, "lights[2].pos");
	recieve_shadow_color.uniforms[7] = glGetUniformLocation(color.id, "shadow");
	recieve_shadow_color.uniforms[8] = glGetUniformLocation(color.id, "shadowP");
	recieve_shadow_color.uniforms[9] = glGetUniformLocation(color.id, "shadowV");
	recieve_shadow_color.uniforms[10] = glGetUniformLocation(color.id, "ambient");

	texture.id = CompileGLShader("texture");
	texture.num_uniforms = 5;
	texture.uniforms = new GLuint[texture.num_uniforms];
	texture.uniforms[0] = glGetUniformLocation(texture.id, "P");
	texture.uniforms[1] = glGetUniformLocation(texture.id, "V");
	texture.uniforms[2] = glGetUniformLocation(texture.id, "M");
	texture.uniforms[3] = glGetUniformLocation(texture.id, "diffuse_texture");
	texture.uniforms[4] = glGetUniformLocation(texture.id, "ambient");

	line.id = CompileGLShader("line");
	line.num_uniforms = 3;
	line.uniforms = new GLuint[color.num_uniforms];
	line.uniforms[0] = glGetUniformLocation(line.id, "P");
	line.uniforms[1] = glGetUniformLocation(line.id, "V");
	line.uniforms[2] = glGetUniformLocation(line.id, "M");

	dicom_point_cloud.id = CompileGLShader("dicom_point_cloud");
	dicom_point_cloud.num_uniforms = 14;
	dicom_point_cloud.uniforms = new GLuint[dicom_point_cloud.num_uniforms];
	dicom_point_cloud.uniforms[0] = glGetUniformLocation(dicom_point_cloud.id, "P");
	dicom_point_cloud.uniforms[1] = glGetUniformLocation(dicom_point_cloud.id, "V");
	dicom_point_cloud.uniforms[2] = glGetUniformLocation(dicom_point_cloud.id, "M");
	dicom_point_cloud.uniforms[3] = glGetUniformLocation(dicom_point_cloud.id, "tolerance");
	dicom_point_cloud.uniforms[4] = glGetUniformLocation(dicom_point_cloud.id, "lights[0].pos"); // change to uniform buffer
	dicom_point_cloud.uniforms[5] = glGetUniformLocation(dicom_point_cloud.id, "lights[1].pos");
	dicom_point_cloud.uniforms[6] = glGetUniformLocation(dicom_point_cloud.id, "lights[2].pos");
	dicom_point_cloud.uniforms[7] = glGetUniformLocation(dicom_point_cloud.id, "scale");
	dicom_point_cloud.uniforms[8] = glGetUniformLocation(dicom_point_cloud.id, "lower_bounds");
	dicom_point_cloud.uniforms[9] = glGetUniformLocation(dicom_point_cloud.id, "upper_bounds");
	dicom_point_cloud.uniforms[11] = glGetUniformLocation(dicom_point_cloud.id, "scale");
	dicom_point_cloud.uniforms[12] = glGetUniformLocation(dicom_point_cloud.id, "eye_pos");
	dicom_point_cloud.uniforms[13] = glGetUniformLocation(dicom_point_cloud.id, "ambient");

	branch_point.id = CompileGLShader("branch_point");
	branch_point.num_uniforms = 7;
	branch_point.uniforms = new GLuint[branch_point.num_uniforms];
	branch_point.uniforms[0] = glGetUniformLocation(branch_point.id, "P");
	branch_point.uniforms[1] = glGetUniformLocation(branch_point.id, "V");
	branch_point.uniforms[2] = glGetUniformLocation(branch_point.id, "M");
	branch_point.uniforms[3] = glGetUniformLocation(branch_point.id, "instanced_position");
	branch_point.uniforms[4] = glGetUniformLocation(branch_point.id, "lower_bounds");
	branch_point.uniforms[5] = glGetUniformLocation(branch_point.id, "scale");
	branch_point.uniforms[6] = glGetUniformLocation(branch_point.id, "color");

	branch_line.id = CompileGLShader("branch_line");
	branch_line.num_uniforms = 10;
	branch_line.uniforms = new GLuint[branch_line.num_uniforms];
	branch_line.uniforms[0] = glGetUniformLocation(branch_line.id, "P");
	branch_line.uniforms[1] = glGetUniformLocation(branch_line.id, "V");
	branch_line.uniforms[2] = glGetUniformLocation(branch_line.id, "M");
	branch_line.uniforms[3] = glGetUniformLocation(branch_line.id, "lower_bounds");
	branch_line.uniforms[4] = glGetUniformLocation(branch_line.id, "scale");
	branch_line.uniforms[5] = glGetUniformLocation(branch_line.id, "pos1");
	branch_line.uniforms[6] = glGetUniformLocation(branch_line.id, "pos2");
	branch_line.uniforms[7] = glGetUniformLocation(branch_line.id, "lights[0].pos"); // change to uniform buffer
	branch_line.uniforms[8] = glGetUniformLocation(branch_line.id, "lights[1].pos");
	branch_line.uniforms[9] = glGetUniformLocation(branch_line.id, "lights[2].pos");

	shadow.id = CompileGLShader("shadow");
	shadow.num_uniforms = 3;
	shadow.uniforms = new GLuint[branch_line.num_uniforms];
	shadow.uniforms[0] = glGetUniformLocation(shadow.id, "P");
	shadow.uniforms[1] = glGetUniformLocation(shadow.id, "V");
	shadow.uniforms[2] = glGetUniformLocation(shadow.id, "M");

	curve.id = CompileGLShader("curve");
	curve.num_uniforms = 6;
	curve.uniforms = new GLuint[curve.num_uniforms];
	curve.uniforms[0] = glGetUniformLocation(curve.id, "P");
	curve.uniforms[1] = glGetUniformLocation(curve.id, "V");
	curve.uniforms[2] = glGetUniformLocation(curve.id, "M");
	curve.uniforms[3] = glGetUniformLocation(curve.id, "pos");
	curve.uniforms[4] = glGetUniformLocation(curve.id, "lower_bounds");
	curve.uniforms[5] = glGetUniformLocation(curve.id, "instanced_position");
}

void Render::RenderSceneInternal(glm::mat4 _P, glm::mat4 _V) 
{	
	BindTextures();

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);

	////////////////////////////////////////////////////////////////////////////////////////////////
	
	glUseProgram(line.id);
	
	glUniformMatrix4fv(line.uniforms[0], 1, GL_FALSE, glm::value_ptr(_P));
	glUniformMatrix4fv(line.uniforms[1], 1, GL_FALSE, glm::value_ptr(_V));
	
	for (LineObject* & l : line_objects) 
	{
		glUniformMatrix4fv(line.uniforms[2], 1, GL_FALSE, glm::value_ptr(glm::mat4()));
		
		glBindVertexArray(l->GetVao());	
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		
		glDrawArrays(GL_LINES, 0, l->GetNumVertices());
	}
	
	////////////////////////////////////////////////////////////////////////////////////////////////
	
	glUseProgram(color.id);
	
	glUniformMatrix4fv(color.uniforms[0], 1, GL_FALSE, glm::value_ptr(_P));
	glUniformMatrix4fv(color.uniforms[1], 1, GL_FALSE, glm::value_ptr(_V));

	glUniform3fv(color.uniforms[4], 1, glm::value_ptr(lights[0].position));
	glUniform3fv(color.uniforms[5], 1, glm::value_ptr(lights[1].position));
	glUniform3fv(color.uniforms[6], 1, glm::value_ptr(lights[2].position));
	glUniform3fv(color.uniforms[10], 1, glm::value_ptr(Constants::AMBIENT_LIGHT));

	glActiveTexture(GL_TEXTURE0 + 8); // TMP: set shadow map texture 
	glBindTexture(GL_TEXTURE_2D, sm.depth);
	glUniform1i(color.uniforms[7], 8);	

	glUniformMatrix4fv(color.uniforms[8], 1, GL_FALSE, glm::value_ptr(sm.P));
	glUniformMatrix4fv(color.uniforms[9], 1, GL_FALSE, glm::value_ptr(sm.V));
	
	for (ColorObject* & cObj : color_objects) 
	{
		if (cObj->is_hidden || !cObj->is_loaded) continue;

		glUniformMatrix4fv(color.uniforms[2], 1, GL_FALSE, glm::value_ptr(cObj->GetModelMatrix()));
		glUniform4fv(color.uniforms[3], 1, glm::value_ptr(cObj->GetDisplayColor()));	

		glBindVertexArray(cObj->GetVao());	
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);

		glDrawArrays(GL_TRIANGLES, 0, cObj->GetNumVertices());
	}
	
	//////////////////////////////////////////////////////////////////////////////////////////////////
	
	glUseProgram(texture.id);

	glUniformMatrix4fv(texture.uniforms[0], 1, GL_FALSE, glm::value_ptr(_P));
	glUniformMatrix4fv(texture.uniforms[1], 1, GL_FALSE, glm::value_ptr(_V));
	glUniform3fv(texture.uniforms[4], 1, glm::value_ptr(Constants::AMBIENT_LIGHT));

	for (TextureObject*& tObj : texture_objects) 
	{
		if (tObj->is_hidden || !tObj->is_loaded)
			continue;

		glUniformMatrix4fv(texture.uniforms[2], 1, GL_FALSE, glm::value_ptr(tObj->GetModelMatrix()));
		glUniform1i(texture.uniforms[3], tObj->texture_level);
		glBindVertexArray(tObj->GetVao());	
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glEnableVertexAttribArray(2);
		glDrawArrays(GL_TRIANGLES, 0, tObj->GetNumVertices());
	}

	////////////////////////////////////////////////////////////////////////////////////////////////	

	glUseProgram(dicom_point_cloud.id);
	
	glUniformMatrix4fv(dicom_point_cloud.uniforms[0], 1, GL_FALSE, glm::value_ptr(_P));
	glUniformMatrix4fv(dicom_point_cloud.uniforms[1], 1, GL_FALSE, glm::value_ptr(_V));

	glUniform3fv(dicom_point_cloud.uniforms[4], 1, glm::value_ptr(lights[0].position));
	glUniform3fv(dicom_point_cloud.uniforms[5], 1, glm::value_ptr(lights[1].position));
	glUniform3fv(dicom_point_cloud.uniforms[6], 1, glm::value_ptr(lights[2].position));
	glUniform3fv(dicom_point_cloud.uniforms[12], 1, glm::value_ptr(vr_info.head_position));
	glUniform3fv(dicom_point_cloud.uniforms[13], 1, glm::value_ptr(Constants::AMBIENT_LIGHT));

	for (DicomPointCloudObject* & dpco : dicom_point_cloud_objects) 
	{
		if (!dpco->first_load)
			continue;

		glUniformMatrix4fv(dicom_point_cloud.uniforms[2], 1, GL_FALSE, glm::value_ptr(dpco->GetModelMatrix()));
		//glUniform1i(dicom_point_cloud.uniforms[3], dpco->curr_tolerance);
		//glUniform3fv(dicom_point_cloud.uniforms[7], 1, glm::value_ptr(glm::vec3(dpco->GetScale())));
		//glUniform3fv(dicom_point_cloud.uniforms[8], 1, glm::value_ptr(dpco->lower_bounds));
		//glUniform3fv(dicom_point_cloud.uniforms[9], 1, glm::value_ptr(dpco->upper_bounds));

		glBindVertexArray(dpco->GetVao());	

		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glEnableVertexAttribArray(2);
		glEnableVertexAttribArray(3);
		glEnableVertexAttribArray(4);
		glVertexAttribDivisor(0, 0);
		glVertexAttribDivisor(1, 0);
		glVertexAttribDivisor(2, 1);
		glVertexAttribDivisor(3, 1);
		glVertexAttribDivisor(4, 1);

		glDrawArraysInstanced(GL_TRIANGLES, 0, dpco->GetNumVertices(), dpco->GetNumInstances());
	
		glUseProgram(branch_point.id);

		glUniformMatrix4fv(branch_point.uniforms[0], 1, GL_FALSE, glm::value_ptr(_P));
		glUniformMatrix4fv(branch_point.uniforms[1], 1, GL_FALSE, glm::value_ptr(_V));
		glUniformMatrix4fv(branch_point.uniforms[2], 1, GL_FALSE, glm::value_ptr(dpco->GetModelMatrix()));
		glUniform3fv(branch_point.uniforms[7], 1, glm::value_ptr(lights[0].position));
		glUniform3fv(branch_point.uniforms[8], 1, glm::value_ptr(lights[1].position));
		glUniform3fv(branch_point.uniforms[9], 1, glm::value_ptr(lights[2].position));

		glUniform3fv(branch_point.uniforms[4], 1, glm::value_ptr(dpco->lower_bounds));
		glUniform3fv(branch_point.uniforms[5], 1, glm::value_ptr(glm::vec3(dpco->GetScale())));

		glBindVertexArray(dpco->branch_point_marker->GetVao());
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);

		for (BranchPoint*& bp : dpco->branch_points) 
		{
			glUniform4fv(branch_point.uniforms[6], 1, glm::value_ptr(bp->getColor()));
			glUniform3fv(branch_point.uniforms[3], 1, glm::value_ptr(bp->position));
			glDrawArrays(GL_TRIANGLES, 0, dpco->branch_point_marker->GetNumVertices());
		}

		//TODO: What does this do? May be redundant?
		glUseProgram(branch_line.id);

		glUniformMatrix4fv(branch_line.uniforms[0], 1, GL_FALSE, glm::value_ptr(_P));
		glUniformMatrix4fv(branch_line.uniforms[1], 1, GL_FALSE, glm::value_ptr(_V));
		glUniformMatrix4fv(branch_line.uniforms[2], 1, GL_FALSE, glm::value_ptr(dpco->GetModelMatrix()));
		glUniform3fv(branch_line.uniforms[3], 1, glm::value_ptr(dpco->lower_bounds));
		glUniform3fv(branch_line.uniforms[4], 1, glm::value_ptr(glm::vec3(dpco->GetScale())));

		for (BranchPoint* & bp : dpco->branch_points) 
		{
			for (int j = 0; j < bp->neighbors.size(); ++j)
			{
				if (bp->neighbors[j] < bp->id)
				{
					BranchPoint* neighbor1 = dpco->GetBranchPointByID(bp->neighbors[j]);
					if (neighbor1 != NULL) 
					{
						glUniform3fv(branch_line.uniforms[5], 1, glm::value_ptr(bp->position));
						glUniform3fv(branch_line.uniforms[6], 1, glm::value_ptr(neighbor1->position));
						glDrawArrays(GL_LINES, 0, 2);
					}
				}
			}
		}

		//Render the branch curves
		glUseProgram(curve.id);

		glUniformMatrix4fv(curve.uniforms[0], 1, GL_FALSE, glm::value_ptr(_P));
		glUniformMatrix4fv(curve.uniforms[1], 1, GL_FALSE, glm::value_ptr(_V));
		glUniformMatrix4fv(curve.uniforms[2], 1, GL_FALSE, glm::value_ptr(dpco->GetModelMatrix()));
		glUniform3fv(curve.uniforms[4], 1, glm::value_ptr(dpco->lower_bounds));
		if (!dpco->curves.empty())
		{
			for (Curve* curve_object : dpco->curves)
			{
				glBindVertexArray(curve_object->GetVao());
				glEnableVertexAttribArray(0);
				glEnableVertexAttribArray(1);
				glDrawArrays(GL_LINE_STRIP, 0, curve_object->GetNumVertices());
			}
		}
	}

	////////////////////////////////////////////////////////////////////////////////////////////////

	// VALVE OBJECT RENDERING

	glUseProgram(texture.id);

	for (uint32_t unTrackedDevice = 0; unTrackedDevice < vr::k_unMaxTrackedDeviceCount; unTrackedDevice++)
	{
		if (!m_rTrackedDeviceToRenderModel[unTrackedDevice] || !m_rbShowTrackedDevice[unTrackedDevice])
			continue;

		const vr::TrackedDevicePose_t & pose = m_rTrackedDevicePose[unTrackedDevice];

		if (!pose.bPoseIsValid)
			continue;

		glUniformMatrix4fv(texture.uniforms[0], 1, GL_FALSE, glm::value_ptr(_P));
		glUniformMatrix4fv(texture.uniforms[1], 1, GL_FALSE, glm::value_ptr(_V));
		glUniform3fv(texture.uniforms[4], 1, glm::value_ptr(Constants::AMBIENT_LIGHT));

		glm::mat4 matDeviceToTracking = m_rmat4DevicePose[unTrackedDevice];
		glUniformMatrix4fv(texture.uniforms[2], 1, GL_FALSE, glm::value_ptr(matDeviceToTracking));

		glBindVertexArray(m_rTrackedDeviceToRenderModel[unTrackedDevice]->m_glVertArray);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, m_rTrackedDeviceToRenderModel[unTrackedDevice]->m_glTexture);

		glUniform1i(texture.uniforms[3], 0);
		glDrawElements(GL_TRIANGLES, m_rTrackedDeviceToRenderModel[unTrackedDevice]->m_unVertexCount, GL_UNSIGNED_SHORT, 0);
	}

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);
	glDisableVertexAttribArray(3);
	glDisableVertexAttribArray(4);
}

// DetectCollision helpers

struct foundCollision
{
	AbstractBaseObject* obj;
	glm::vec3 intersection_point;
	float distance;

	foundCollision(AbstractBaseObject* abo, glm::vec3 _ip, float _dist)
	{
		obj = abo;
		intersection_point = _ip;
		distance = _dist;
	}
};

struct less_than_key
{
	inline bool operator() (const foundCollision& struct1, const foundCollision& struct2)
	{
		return (struct1.distance < struct2.distance);
	}
};

void Render::DetectCollision(VrMotionController & _controller)
{
	// current controllers selected object and other controllers selected object
	AbstractBaseObject*& curr = _controller.id == 0 ? selected_element1 : selected_element2;
	AbstractBaseObject*& oth = _controller.id != 0 ? selected_element1 : selected_element2;

	// controller alternate button is held down with previous selection
	if (_controller.alt_is_pressed && curr != NULL)
	{			
		// other controller has just released previously shared object
		if (curr->controllerSelectorId == -1)
		{
			curr->is_double_selected = false;
			curr->SetSelected(true);
			curr->controllerSelectorId = _controller.id;

			glm::mat4 inverse_controller_pose = glm::inverse(_controller.pose);

			curr->cache.to_controller_space_initial = inverse_controller_pose * curr->GetAppendPose();
			curr->cache.controller_pose_initial = _controller.pose;
			curr->cache.controller_pose_updated = _controller.pose;
			curr->cache.append_pose_initial = curr->GetAppendPose();

			if (curr->controllerSelectorIdPrev == _controller.id)
			{
				// primary controller regains sole ownership of object, no need to set:
				//curr->cache.primary_collision_point_world_initial
				//curr->cache.primary_collision_point_world_current
				//curr->cache.primary_collision_point_controller_space_initial
			}
			else
			{
				// switch previously secondary controller to primary
				// rotate previous point to new position
				curr->cache.to_controller_space_initial = inverse_controller_pose * curr->GetAppendPose();
				curr->cache.primary_collision_point_world_initial = curr->cache.secondary_collision_point_world_current;
				curr->cache.primary_collision_point_world_current = curr->cache.secondary_collision_point_world_current;
				curr->cache.primary_collision_point_controller_space_initial = curr->cache.secondary_collision_point_controller_space_initial;
				curr->controllerSelectorIdPrev = _controller.id;
				curr->cache.primary_collision_point_world_current = glm::vec3(_controller.pose * glm::vec4(curr->cache.primary_collision_point_controller_space_initial, 1.0f));
				curr->cache.controller_pose_updated = _controller.pose;
			}
		}
		// two controllers with same object selected
		else if (curr == oth)
		{
			// first clicker of selection
			if (curr->controllerSelectorId == _controller.id) 
			{
				curr->cache.primary_collision_point_world_current = glm::vec3(_controller.pose * glm::vec4(curr->cache.primary_collision_point_controller_space_initial,1.0f));
				curr->cache.controller_pose_updated = _controller.pose;
			}
			// second clicker
			else
			{
				curr->cache.secondary_collision_point_world_current = glm::vec3(_controller.pose * glm::vec4(curr->cache.secondary_collision_point_controller_space_initial, 1.0f));
			}

			// double update
			//curr->cache.primary_to_secondary_collision_point_current = curr->cache.secondary_collision_point_world_current - curr->cache.primary_collision_point_world_current;
			curr->cache.primary_to_secondary_collision_point_current = curr->cache.primary_collision_point_world_current - curr->cache.secondary_collision_point_world_current;
		}
		// single selection
		else
		{
			curr->cache.primary_collision_point_world_current = glm::vec3(_controller.pose * glm::vec4(curr->cache.primary_collision_point_controller_space_initial, 1.0f));
			curr->cache.controller_pose_updated = _controller.pose;
		}
	}
	// controller alternate button pressed for the first time without previous selection
	else if (_controller.alt_first_press && curr == NULL)
	{
		std::vector<foundCollision> found_collisions;

		for (AbstractBaseObject * absObj : all_objects) 
		{
			glm::vec3 collision_point;
			if(!absObj->is_hidden && (absObj->is_selectable || absObj->is_clickable) && absObj->TestCollision(_controller.ray, _controller.position, collision_point))
			{
				std::cout << "collisions" << std::endl;
				found_collisions.push_back(foundCollision(absObj, collision_point, glm::length(collision_point - _controller.position)));
			}
		}

		if (found_collisions.size() > 0) 
		{
			// find closest element to controller
			std::sort(found_collisions.begin(), found_collisions.end(), less_than_key());
			
			// calculate length of pointer intersection
			glm::mat4 inverse_controller_pose = glm::inverse(_controller.pose);
			glm::vec3 intersection_point_controller_space = glm::vec3(inverse_controller_pose * glm::vec4(found_collisions[0].intersection_point, 1.0f));

			// selected element already selected by other controller
			if (found_collisions[0].obj == oth)
			{
				if (!found_collisions[0].obj->is_double_selectable)
					return;

				curr = found_collisions[0].obj;
				curr->is_double_selected = true;
				curr->cache.initial_scale = curr->GetScale();
				curr->cache.secondary_collision_point_controller_space_initial = intersection_point_controller_space;
				curr->cache.secondary_collision_point_world_initial = found_collisions[0].intersection_point;
				curr->cache.secondary_collision_point_world_current = found_collisions[0].intersection_point;
				curr->cache.primary_to_secondary_collision_point_initial = curr->cache.primary_collision_point_world_current - found_collisions[0].intersection_point;
				curr->cache.primary_to_secondary_collision_point_current = curr->cache.primary_collision_point_world_current - found_collisions[0].intersection_point;
			}
			else 
			{
				curr = found_collisions[0].obj;
				curr->controllerSelectorId = _controller.id;
				curr->controllerSelectorIdPrev = _controller.id;
				curr->cache.to_controller_space_initial = inverse_controller_pose * curr->GetAppendPose();
				curr->cache.controller_pose_initial = _controller.pose;
				curr->cache.controller_pose_updated = _controller.pose;
				curr->cache.append_pose_initial = curr->GetAppendPose();
				curr->cache.primary_collision_point_world_initial = found_collisions[0].intersection_point;
				curr->cache.primary_collision_point_world_current = found_collisions[0].intersection_point;
				curr->cache.primary_collision_point_controller_space_initial = intersection_point_controller_space;			
				curr->cache.controller_world_position_initial = _controller.position;
				
				if (curr->is_clickable)
				{
					curr->SetClicked();
					curr = NULL;
				}
				else
				{
					curr->SetSelected(true);
				}
			}
		}
	}
	// alternate button released with a selection
	else if (!_controller.alt_is_pressed && curr != NULL)
	{
		if (curr == oth)
		{
			// first release of a double selected object
		}
		else
		{
			// single ownership
			curr->SetSelected(false);
		}

		curr->controllerSelectorId = -1;
		curr = NULL;
	}
}

void Render::Interact() 
{
	DetectCollision(vr_info.controller1);
	DetectCollision(vr_info.controller2);
}

void Render::SpoofInput(int controllerIndex) 
{
	float mov_rate = 0.0125f;
	float rot_rate = 0.5f;

	VrMotionController& currController = controllerIndex == 0 ? vr_info.controller1 : vr_info.controller2;	
	glm::vec3 strafe = glm::normalize(glm::cross(Constants::Y_AXIS, currController.ray));

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) 
	{
		currController.SetPositionSpoof(currController.position + mov_rate*currController.ray);
	}
	
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) 
	{
		currController.SetPositionSpoof(currController.position - mov_rate*currController.ray);
	}

	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
	{
		currController.SetPositionSpoof(currController.position + mov_rate*strafe);
	}
	
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
	{
		currController.SetPositionSpoof(currController.position - mov_rate*strafe);
	}

	// the position of the cursor is the distance moved since last reset to center of the screen (0,0)
	
	currController.SetOrientationSpoof(currController.orientation + 
		glm::vec3
			(
			rot_rate*-cursor_info.normalized_cursor_position.x, 
			glm::clamp(rot_rate*cursor_info.normalized_cursor_position.y, -1.2f, 1.2f), 
			0.0f
			)
	);

	// set to center of the screen
	glfwSetCursorPos(window, half_window_size_x, half_window_size_y);
	/*
	static bool trigger_once[] = { true, true };
	static bool alt_once[] = { true, true };
	static bool appmenu_once[] = { true, true };
	static bool touchpad_once[] = { true, true };
	
	// trigger spoof LMB
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_1))
	{
		if (trigger_once[controllerIndex]) 
		{
			currController.trigger_first_press = !currController.trigger_is_pressed;
			currController.trigger_is_pressed = !currController.trigger_is_pressed;
			trigger_once[controllerIndex] = false;
		}
		else 
		{
			currController.trigger_first_press = false;
		}
	}
	else if(!glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_1))
	{
		trigger_once[controllerIndex] = true;
	}

	// alternate button spoof RMB
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_2))
	{
		if (alt_once[controllerIndex])
		{
			currController.alt_first_press = !currController.alt_is_pressed;
			currController.alt_is_pressed = !currController.alt_is_pressed;
			alt_once[controllerIndex] = false;
		}
		else
		{
			currController.alt_first_press = false;
		}
	}
	else if (!glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_2))
	{
		alt_once[controllerIndex] = true;
	}

	// appmenu button spoof MMB
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_3))
	{
		if (appmenu_once[controllerIndex])
		{
			currController.appmenu_first_press = !currController.appmenu_is_pressed;
			currController.appmenu_is_pressed = !currController.appmenu_is_pressed;
			appmenu_once[controllerIndex] = false;
		}
		else
		{
			currController.appmenu_first_press = false;
			currController.appmenu_is_pressed = false;
		}
	}
	else if (!glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_3))
	{
		appmenu_once[controllerIndex] = true;
	}

	//touchpad button spoof keyboard N
	if (glfwGetKey(window, GLFW_KEY_N) == GLFW_RELEASE)
	{
		if (touchpad_once[controllerIndex])
		{
			currController.touchpad_first_press = !currController.touchpad_is_pressed;
			currController.touchpad_is_pressed = !currController.touchpad_is_pressed;
			touchpad_once[controllerIndex] = false;
		}
		else
		{
			currController.touchpad_first_press = false;
			currController.touchpad_is_pressed = false;
		}
	}
	else if (!glfwGetKey(window, GLFW_KEY_N) == GLFW_RELEASE)
	{
		touchpad_once[controllerIndex] = true;
	}
	*/
	vr_info.head_pose_inv = glm::inverse(currController.pose);

	
	controller_pointer1->SetModelMatrixOverride(vr_info.controller1.pose);
	controller_pointer2->SetModelMatrixOverride(vr_info.controller2.pose);

	controller_pointer1->SetSelected(vr_info.controller1.trigger_is_pressed);
	controller_pointer2->SetSelected(vr_info.controller2.trigger_is_pressed);
	
}

void Render::UpdateHMDMatrixPose()
{
	if (!m_pHMD || !hmd_ready) 
	{
		// without the HMD connected, default to keyboard control over spoofed motion controllers		
		
		vr_info.hmd_connected = false;

		static int curr_controller_index = 0;
		static bool once_per_press = true;			
		
		controller_pointer1->is_hidden = curr_controller_index == 0;
		controller_pointer2->is_hidden = curr_controller_index == 1;

		if (glfwGetKey(window, GLFW_KEY_Q) && once_per_press)
		{
			curr_controller_index = !curr_controller_index;
			once_per_press = false;
		}
		else if(!glfwGetKey(window, GLFW_KEY_Q))
		{
			once_per_press = true;
		}

		if (glfwGetKey(window, GLFW_KEY_SPACE)) {
			if (glfwGetWindowAttrib(window, GLFW_FOCUSED))
			{
				SpoofInput(curr_controller_index);
			}
		//	else if (glfwGetWindowAttrib())
		}
		
		return;
	}
	vr_info.hmd_connected = true;

	controller_pointer1->is_hidden = false;
	controller_pointer2->is_hidden = false;

	vr::VRCompositor()->WaitGetPoses(m_rTrackedDevicePose, vr::k_unMaxTrackedDeviceCount, NULL, 0);

	// update HMD variables
	if (m_rTrackedDevicePose[vr::k_unTrackedDeviceIndex_Hmd].bPoseIsValid)
	{
		vr_info.head_pose_inv = ValveMat34ToGlmMat4Inv(m_rTrackedDevicePose[vr::k_unTrackedDeviceIndex_Hmd].mDeviceToAbsoluteTracking);
		vr_info.left_eye_proj = ValveMat4ToGlmMat4(m_pHMD->GetProjectionMatrix(vr::Eye_Left, 0.1f, 1000.0f, vr::API_OpenGL));
		vr_info.left_eye_transform_inv = ValveMat34ToGlmMat4Inv(m_pHMD->GetEyeToHeadTransform(vr::Eye_Left)) *vr_info.head_pose_inv;
		vr_info.right_eye_proj = ValveMat4ToGlmMat4(m_pHMD->GetProjectionMatrix(vr::Eye_Right, 0.1f, 1000.0f, vr::API_OpenGL));
		vr_info.right_eye_transform_inv = ValveMat34ToGlmMat4Inv(m_pHMD->GetEyeToHeadTransform(vr::Eye_Right)) *vr_info.head_pose_inv;
	}

	// update controllers
	int controllerIndex = 0;

	for (vr::TrackedDeviceIndex_t unTrackedDevice = vr::k_unTrackedDeviceIndex_Hmd + 1; unTrackedDevice < vr::k_unMaxTrackedDeviceCount; ++unTrackedDevice)
	{
		if (!m_pHMD->IsTrackedDeviceConnected(unTrackedDevice))
			continue;

		if (m_pHMD->GetTrackedDeviceClass(unTrackedDevice) != vr::TrackedDeviceClass_Controller)
			continue;


		if (!m_rTrackedDevicePose[unTrackedDevice].bPoseIsValid)
			continue;

		m_rmat4DevicePose[unTrackedDevice] = ValveMat34ToGlmMat4(m_rTrackedDevicePose[unTrackedDevice].mDeviceToAbsoluteTracking);
		
		VrMotionController & currController = (controllerIndex++ == 0) ? vr_info.controller1 : vr_info.controller2;
		currController.SetPose(ValveMat34ToGlmMat4(m_rTrackedDevicePose[unTrackedDevice].mDeviceToAbsoluteTracking));
		
		vr::VRControllerState_t state;

		if (m_pHMD->GetControllerState(unTrackedDevice, &state))
		{


			if (state.ulButtonPressed == 8589934592)
			{
				//std::cout << "trigger pressed" << std::endl;
				currController.trigger_first_press = !currController.trigger_is_pressed;
				currController.trigger_is_pressed = true;
			}
			else 
			{
				currController.trigger_first_press = false;
				currController.trigger_is_pressed = false;
			}


			if (state.ulButtonPressed == 4)
			{
				//std::cout << "alt pressed" << std::endl;
				currController.alt_first_press = !currController.alt_is_pressed;
				currController.alt_is_pressed = true;
			}
			else
			{
				currController.alt_first_press = false;
				currController.alt_is_pressed = false;
			}

			if (state.ulButtonPressed & vr::ButtonMaskFromId(vr::k_EButton_ApplicationMenu))
			{
				//std::cout << "app menu" << std::endl;
				currController.appmenu_first_press = !currController.appmenu_is_pressed;
				currController.appmenu_is_pressed = true;
			}
			else
			{
				currController.appmenu_first_press = false;
				currController.appmenu_is_pressed = false;
			}

			if (state.ulButtonPressed & vr::ButtonMaskFromId(vr::k_EButton_SteamVR_Touchpad))
			{
				//std::cout << "touchpad" << std::endl;
				currController.touchpad_first_press = !currController.touchpad_is_pressed;
				currController.touchpad_is_pressed = true;
			}
			else
			{
				currController.touchpad_first_press = false;
				currController.touchpad_is_pressed = false;
			}
			if (state.ulButtonTouched & vr::ButtonMaskFromId(vr::k_EButton_SteamVR_Touchpad))
			{
				std::cout << "touchpad" << std::endl;
				vr::VREvent_TouchPadMove_t tracking = vr::VREvent_TouchPadMove_t();
				if (tracking.bFingerDown && tracking.flSecondsFingerDown > 0.5f) {
					currController.touchpad_is_touched = true;
					currController.touch_rotate = glm::vec3(tracking.fValueXRaw - tracking.fValueXFirst, tracking.fValueYRaw - tracking.fValueYFirst, 0.0f);
				}
				else
					currController.touchpad_is_touched = false; 
			}
			else
				currController.touchpad_is_touched = false;
		}
	}

	controller_pointer1->SetModelMatrixOverride(vr_info.controller1.pose);
	controller_pointer2->SetModelMatrixOverride(vr_info.controller2.pose);

	controller_pointer1->SetSelected(vr_info.controller1.trigger_is_pressed);
	controller_pointer2->SetSelected(vr_info.controller2.trigger_is_pressed);
}

glm::mat4 Render::ValveMat34ToGlmMat4Inv(vr::HmdMatrix34_t _mIN) 
{
	return glm::inverse(glm::mat4(
		_mIN.m[0][0], _mIN.m[1][0], _mIN.m[2][0], 0.0,
		_mIN.m[0][1], _mIN.m[1][1], _mIN.m[2][1], 0.0,
		_mIN.m[0][2], _mIN.m[1][2], _mIN.m[2][2], 0.0,
		_mIN.m[0][3], _mIN.m[1][3], _mIN.m[2][3], 1.0f));
}

glm::mat4 Render::ValveMat34ToGlmMat4(vr::HmdMatrix34_t _mIN) 
{
	return glm::mat4(
		_mIN.m[0][0], _mIN.m[1][0], _mIN.m[2][0], 0.0,
		_mIN.m[0][1], _mIN.m[1][1], _mIN.m[2][1], 0.0,
		_mIN.m[0][2], _mIN.m[1][2], _mIN.m[2][2], 0.0,
		_mIN.m[0][3], _mIN.m[1][3], _mIN.m[2][3], 1.0f);
}

glm::mat4 Render::ValveMat4ToGlmMat4Inv(vr::HmdMatrix44_t _mIN) 
{
	return glm::inverse(glm::mat4(
		_mIN.m[0][0], _mIN.m[1][0], _mIN.m[2][0], _mIN.m[3][0],
		_mIN.m[0][1], _mIN.m[1][1], _mIN.m[2][1], _mIN.m[3][1],
		_mIN.m[0][2], _mIN.m[1][2], _mIN.m[2][2], _mIN.m[3][2],
		_mIN.m[0][3], _mIN.m[1][3], _mIN.m[2][3], _mIN.m[3][3]));
}

glm::mat4 Render::ValveMat4ToGlmMat4(vr::HmdMatrix44_t _mIN) 
{
	return glm::mat4(
		_mIN.m[0][0], _mIN.m[1][0], _mIN.m[2][0], _mIN.m[3][0],
		_mIN.m[0][1], _mIN.m[1][1], _mIN.m[2][1], _mIN.m[3][1],
		_mIN.m[0][2], _mIN.m[1][2], _mIN.m[2][2], _mIN.m[3][2],
		_mIN.m[0][3], _mIN.m[1][3], _mIN.m[2][3], _mIN.m[3][3]);
}

std::string Render::ReadFile(std::string _filePath) 
{
	std::ifstream t(_filePath);
	std::string str((std::istreambuf_iterator<char>(t)), std::istreambuf_iterator<char>());
	return str;
}

GLuint Render::CompileGLShader(std::string programName)
{
	std::string shader_dir = MiscFunctions::relativeToAbsolutePath(DirectoryInfo::RELATIVE_SHADERS_DIR);
	std::string vs_src = Render::ReadFile(shader_dir + programName + ".vs.glsl");
	std::string fs_src = Render::ReadFile(shader_dir + programName + ".fs.glsl");

	GLuint unProgramID = glCreateProgram();

	GLuint nSceneVertexShader = Render::CreateShader(GL_VERTEX_SHADER, vs_src);
	glAttachShader(unProgramID, nSceneVertexShader);
	glDeleteShader(nSceneVertexShader); 

	GLuint  nSceneFragmentShader = Render::CreateShader(GL_FRAGMENT_SHADER, fs_src);
	glAttachShader(unProgramID, nSceneFragmentShader);
	glDeleteShader(nSceneFragmentShader);

	if (nSceneFragmentShader == 0 || nSceneVertexShader == 0)
	{
		std::cout << "failed to load shader: " << programName << std::endl;
	}

	glLinkProgram(unProgramID);

	GLint programSuccess = GL_TRUE;
	glGetProgramiv(unProgramID, GL_LINK_STATUS, &programSuccess);

	if (programSuccess != GL_TRUE)
	{
		printf("%s - Error linking program %d!\n", programName.c_str(), unProgramID);
		
		GLint maxLength = 0;
		glGetProgramiv(unProgramID, GL_INFO_LOG_LENGTH, &maxLength);

		//The maxLength includes the NULL character
		std::vector<GLchar> infoLog(maxLength);
		glGetProgramInfoLog(unProgramID, maxLength, &maxLength, &infoLog[0]);
		printf("%s\n", &infoLog[0]);

		glDeleteProgram(unProgramID);
		return 0;
	}

	glUseProgram(unProgramID);
	glUseProgram(0);

	return unProgramID;
}

GLuint Render::CreateShader(GLint target, std::string& src) 
{
	GLuint shader = glCreateShader(target);
	const char* c_str = src.c_str();
	GLint len = src.length();

	glShaderSource(shader, 1, &c_str, &len);
	glCompileShader(shader);

	GLint fShaderCompiled = GL_FALSE;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &fShaderCompiled);

	if (fShaderCompiled != GL_TRUE)
	{
		GLint maxLength = 0;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);

		std::vector<GLchar> errorLog(maxLength);
		glGetShaderInfoLog(shader, maxLength, &maxLength, &errorLog[0]);

		for (auto const& value : errorLog) 
		{
			std::cout << value;
		}

		glDeleteShader(shader);
		return 0;
	}

	return shader;
}

bool Render::CreateFrameBuffer(int nWidth, int nHeight, FramebufferDesc &framebufferDesc)
{
	glGenFramebuffers(1, &framebufferDesc.m_nRenderFramebufferId);
	glBindFramebuffer(GL_FRAMEBUFFER, framebufferDesc.m_nRenderFramebufferId);

	glGenRenderbuffers(1, &framebufferDesc.m_nDepthBufferId);
	glBindRenderbuffer(GL_RENDERBUFFER, framebufferDesc.m_nDepthBufferId);
	glRenderbufferStorageMultisample(GL_RENDERBUFFER, 4, GL_DEPTH_COMPONENT, nWidth, nHeight);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, framebufferDesc.m_nDepthBufferId);

	glGenTextures(1, &framebufferDesc.m_nRenderTextureId);
	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, framebufferDesc.m_nRenderTextureId);
	glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, 4, GL_RGBA8, nWidth, nHeight, true);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, framebufferDesc.m_nRenderTextureId, 0);

	glGenFramebuffers(1, &framebufferDesc.m_nResolveFramebufferId);
	glBindFramebuffer(GL_FRAMEBUFFER, framebufferDesc.m_nResolveFramebufferId);

	glGenTextures(1, &framebufferDesc.m_nResolveTextureId);
	glBindTexture(GL_TEXTURE_2D, framebufferDesc.m_nResolveTextureId);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, nWidth, nHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, framebufferDesc.m_nResolveTextureId, 0);

	// check FBO status
	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (status != GL_FRAMEBUFFER_COMPLETE)
	{
		return false;
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	return true;
}

bool Render::createShadowMap(ShadowMap &sm) 
{
	glGenFramebuffers(1, &sm.fbo);

	glGenTextures(1, &sm.depth);
	glBindTexture(GL_TEXTURE_2D, sm.depth);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, DEFAULT_SHADOW_RES, DEFAULT_SHADOW_RES, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	GLfloat borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

	glBindFramebuffer(GL_FRAMEBUFFER, sm.fbo);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, sm.depth, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	return true;
}

std::string GetTrackedDeviceString(vr::IVRSystem *pHmd, vr::TrackedDeviceIndex_t unDevice, vr::TrackedDeviceProperty prop, vr::TrackedPropertyError *peError = NULL) /* valve */
{
	uint32_t unRequiredBufferLen = pHmd->GetStringTrackedDeviceProperty(unDevice, prop, NULL, 0, peError);
	if (unRequiredBufferLen == 0)
		return "";

	char *pchBuffer = new char[unRequiredBufferLen];
	unRequiredBufferLen = pHmd->GetStringTrackedDeviceProperty(unDevice, prop, pchBuffer, unRequiredBufferLen, peError);
	std::string sResult = pchBuffer;
	delete[] pchBuffer;
	return sResult;
}

void Render::SetupRenderModelForTrackedDevice(vr::TrackedDeviceIndex_t unTrackedDeviceIndex) /* valve */
{
	if (unTrackedDeviceIndex >= vr::k_unMaxTrackedDeviceCount)
		return;

	// try to find a model we've already set up
	std::string sRenderModelName = GetTrackedDeviceString(m_pHMD, unTrackedDeviceIndex, vr::Prop_RenderModelName_String);
	CGLRenderModel *pRenderModel = FindOrLoadRenderModel(sRenderModelName.c_str());
	if (!pRenderModel)
	{
		std::string sTrackingSystemName = GetTrackedDeviceString(m_pHMD, unTrackedDeviceIndex, vr::Prop_TrackingSystemName_String);
		MiscFunctions::dprintf("Unable to load render model for tracked device %d (%s.%s)", unTrackedDeviceIndex, sTrackingSystemName.c_str(), sRenderModelName.c_str());
	}
	else
	{
		std::cout << "setting up: " << sRenderModelName << std::endl;
		m_rTrackedDeviceToRenderModel[unTrackedDeviceIndex] = pRenderModel;
		m_rbShowTrackedDevice[unTrackedDeviceIndex] = true;
	}
}

void Render::SetupRenderModels() /* valve */
{
	memset(m_rTrackedDeviceToRenderModel, 0, sizeof(m_rTrackedDeviceToRenderModel));

	if (!m_pHMD)
		return;

	for (uint32_t unTrackedDevice = vr::k_unTrackedDeviceIndex_Hmd + 1; unTrackedDevice < vr::k_unMaxTrackedDeviceCount; unTrackedDevice++)
	{
		if (!m_pHMD->IsTrackedDeviceConnected(unTrackedDevice))
			continue;

		SetupRenderModelForTrackedDevice(unTrackedDevice);
	}

}

CGLRenderModel* Render::FindOrLoadRenderModel(const char *pchRenderModelName) /* valve */
{
	CGLRenderModel *pRenderModel = NULL;
	for (std::vector< CGLRenderModel * >::iterator i = m_vecRenderModels.begin(); i != m_vecRenderModels.end(); i++)
	{
		if (!_stricmp((*i)->GetName().c_str(), pchRenderModelName))
		{
			pRenderModel = *i;
			break;
		}
	}

	// load the model if we didn't find one
	if (!pRenderModel)
	{
		vr::RenderModel_t *pModel;
		vr::EVRRenderModelError error;
		while (1)
		{
			error = vr::VRRenderModels()->LoadRenderModel_Async(pchRenderModelName, &pModel);
			if (error != vr::VRRenderModelError_Loading)
				break;

			MiscFunctions::thread_sleep(1);
		}

		if (error != vr::VRRenderModelError_None)
		{
			MiscFunctions::dprintf("Unable to load render model %s - %s\n", pchRenderModelName, vr::VRRenderModels()->GetRenderModelErrorNameFromEnum(error));
			return NULL; // move on to the next tracked device
		}

		vr::RenderModel_TextureMap_t *pTexture;
		while (1)
		{
			error = vr::VRRenderModels()->LoadTexture_Async(pModel->diffuseTextureId, &pTexture);
			if (error != vr::VRRenderModelError_Loading)
				break;

			MiscFunctions::thread_sleep(1);
		}

		if (error != vr::VRRenderModelError_None)
		{
			MiscFunctions::dprintf("Unable to load render texture id:%d for render model %s\n", pModel->diffuseTextureId, pchRenderModelName);
			vr::VRRenderModels()->FreeRenderModel(pModel);
			return NULL; // move on to the next tracked device
		}

		pRenderModel = new CGLRenderModel(pchRenderModelName);
		if (!pRenderModel->BInit(*pModel, *pTexture))
		{
			MiscFunctions::dprintf("Unable to create GL model from render model %s\n", pchRenderModelName);
			delete pRenderModel;
			pRenderModel = NULL;
		}
		else
		{
			m_vecRenderModels.push_back(pRenderModel);
		}

		vr::VRRenderModels()->FreeRenderModel(pModel);
		vr::VRRenderModels()->FreeTexture(pTexture);
	}
	return pRenderModel;
}
