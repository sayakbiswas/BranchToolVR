#include "DicomObjectsContainer.h"

ColorObject* debug1 = new ColorObject;
ColorObject* debug2 = new ColorObject;
int first = 0;
int last = 50;
bool pushed, fslide, lslide, sliderHasChanged, exportButtonPressed;

int IsovaluePointCloudSlider::id_counter = 0;
const int max_nr_isovalue_point_cloud_sliders = MAX_NR_POINT_CLOUD_SLIDERS;
float IsovaluePointCloudSlider::min_isovalue = TMP_MIN_ISOVALUE;
float IsovaluePointCloudSlider::max_isovalue = TMP_MAX_ISOVALUE;

DicomObjectsContainer::DicomObjectsContainer()
{
	points = new DicomPointCloudObject;
	viewer = new CoarseDicomViewer;
	
	// set starting locations
	float initial_scale = 0.5f;
	glm::vec3 initial_position = glm::vec3(0.5f, 0.25f, 0.5f);
	glm::mat4 tmp_initial_model_matrix = glm::translate(glm::mat4(1.0f), initial_position) * glm::scale(glm::mat4(1.0f), glm::vec3(initial_scale));
	//viewer->SetMasterAppendPose(tmp_initial_model_matrix);
	//SetCoarseViewerAppendPose(tmp_initial_model_matrix);
	
	initial_scale = 0.5f;
	initial_position = glm::vec3(-0.5f, 0.25f, 0.5f);
	tmp_initial_model_matrix = glm::translate(glm::mat4(1.0f), initial_position) * glm::scale(glm::mat4(1.0f), glm::vec3(initial_scale));
	points->SetMasterAppendPose(tmp_initial_model_matrix);

	debug1->GenerateSphere(10, 0.05f, false);
	debug2->GenerateSphere(10, 0.05f, false);


	for (int i = 0; i < max_nr_isovalue_point_cloud_sliders; ++i)
	{
		isovalue_point_cloud_sliders.push_back(new IsovaluePointCloudSlider);
	}
}

DicomObjectsContainer::~DicomObjectsContainer()
{
	delete points;
	delete viewer;
}

static void FileMenu() {
	if (ImGui::MenuItem("New", "CTRL+N")) {
		// dialog file selection
	}
	if (ImGui::MenuItem("Open", "CTRL+O")) {
		// dialog file selection

	}
	if (ImGui::BeginMenu("Open Recent")) {
		// ---- populate 3 recent files ----
		ImGui::Separator();
		if (ImGui::MenuItem("More...", NULL)) {
			// dialog file selection, call open file method
		}
		ImGui::EndMenu();
	}
	ImGui::Separator();
	if (ImGui::MenuItem("Save", "Ctrl+S")) {
	}
	if (ImGui::MenuItem("Save As...", "SHIFT+CTRL+S")) {
	}
	ImGui::Separator();
	if (ImGui::MenuItem("Close", "CTRL+Q")) {

	}
}
static void MainMenuBar() {
	if (ImGui::BeginMainMenuBar()) {
		if (ImGui::BeginMenu("File")) {
			FileMenu();
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Edit")) {
			if (ImGui::MenuItem("Undo Draw", "CTRL+Z")) {}
			if (ImGui::MenuItem("Redo Draw", "CTRL+Y")) {}
			ImGui::Separator();
			if (ImGui::MenuItem("Clean Slate", "CTRL+E")) {}
			if (ImGui::MenuItem("Clear First Slide", NULL)){}
			if (ImGui::MenuItem("Clear Last Slide", NULL)){}
			ImGui::Separator();
			if (ImGui::MenuItem("Delete Values", "CTRL+D")) {}
			if (ImGui::MenuItem("Wipe Field", "CTRL+W")) {}
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Window")) {
			/*---- Potential View features:
					transparency slider
					text size slider
					Show CT ui window
					show mini 3D CT setup */
			ImGui::EndMenu();
		}
		ImGui::EndMainMenuBar();
	}	
}

void DicomObjectsContainer::RenderUi()
{
	// ========== Main File Menu Bar ===========
	MainMenuBar();
	// demo imgui window
	//bool b = true;
	//ImGui::ShowTestWindow(&b);
	// orthoslice 


	ImGui::Columns(2, "mixed");
	ImGui::Separator();
	ImGui::PushStyleVar(ImGuiStyleVar_ChildWindowRounding, 5.0f);
	ImGui::BeginChild("slides", ImVec2(0, 700), true, 1);
	ImGui::Text("");
	ImVec2 tex_screen_pos = ImGui::GetCursorScreenPos();
	float tex_w = (float)viewer->orthoslice_texture->width;
	float tex_h = (float)viewer->orthoslice_texture->height;
	ImGui::ImageButton((void*)viewer->orthoslice_texture->GetGlId(), ImVec2(viewer->orthoslice_texture->width, viewer->orthoslice_texture->height), ImVec2(0, 0), ImVec2(1, 1), -1, ImVec4(0, 0, 0, 1), ImVec4(1, 1, 1, 1));
	//ImTextureID tex_id = viewer->orthoslice_texture->id;
	
	// Refactor
	/*if (ImGui::IsItemHovered())
	{
		//enlarged view for isovalue selection
		ImGui::BeginTooltip();
		float focus_sz = 32.0f;
		float focus_x = ImGui::GetMousePos().x - tex_screen_pos.x - focus_sz * 0.5f; if (focus_x < 0.0f) focus_x = 0.0f; else if (focus_x > tex_w - focus_sz) focus_x = tex_w - focus_sz;
		float focus_y = ImGui::GetMousePos().y - tex_screen_pos.y - focus_sz * 0.5f; if (focus_y < 0.0f) focus_y = 0.0f; else if (focus_y > tex_h - focus_sz) focus_y = tex_h - focus_sz;
		//ImGui::Text("Min: (%.2f, %.2f)", focus_x, focus_y);
		//ImGui::Text("Max: (%.2f, %.2f)", focus_x + focus_sz, focus_y + focus_sz);
		ImVec2 uv0 = ImVec2((focus_x) / tex_w, (focus_y) / tex_h);
		ImVec2 uv1 = ImVec2((focus_x + focus_sz) / tex_w, (focus_y + focus_sz) / tex_h);
		ImGui::Image((void*)7, ImVec2(128, 128), uv0, uv1);
		ImGui::EndTooltip();
	}*/
	if (ImGui::IsItemClicked(0)) { //select isovalue
		ImVec2 mousePos = ImGui::GetMousePos();
		ImVec2 imageCorner = ImGui::GetItemRectMin();
		//std::cout << (mousePos.x) << " , " << (mousePos.y) << " ; " << (imageCorner.x) << " , " << (imageCorner.y) << std::endl;
		float x = mousePos.x - imageCorner.x;
		float y = mousePos.y - imageCorner.y;
		//std::cout << (imaging_data.scale.x) << std::endl;
		imaging_data.isovalue = imaging_data.data[imaging_data.current_index].isovalues.at(imaging_data.data[imaging_data.current_index].width * y + x);

		//points->lower_bounds.x = x/imaging_data.data[0].width - 0.1f; points->lower_bounds.y = y/imaging_data.data[0].height - 0.1f;
		//points->upper_bounds.x = x/imaging_data.data[0].width + 0.1f; points->upper_bounds.y = y/imaging_data.data[0].height + 0.1f;

		AddIsovaluePointCloudSlider(imaging_data.isovalue);
		points->MarkForRegeneration();
		points->Generate(imaging_data, imaging_data.isovalue, 10, first, last, isovalue_point_cloud_sliders);
		//std::cout << (imaging_data.isovalue) << std::endl;
		//std::cout << " You clicked the thing pt1! ";
	}


	// ========== draw magnification square =============
	ImDrawList* list = ImGui::GetWindowDrawList();
	ImVec2 mousePos = ImGui::GetMousePos();
	ImVec2 imageCorner = ImGui::GetItemRectMin();
	ImVec2 imageSize = ImGui::GetItemRectMax();
	ImVec2 canvas_pos = ImGui::GetCursorScreenPos();
	ImVec2 canvas_size;
	canvas_size.x = imageSize.x - imageCorner.x;
	canvas_size.y = imageSize.y - imageCorner.y;

	static ImVector<ImVec2> corners;
	static ImVector<ImVec2> bounds;
	static bool adding_selec = false;
	list->AddRect(imageCorner, imageSize, ImColor(255, 255, 255));
	bool selec_prev = false;
	//ImGui::InvisibleButton("slides", canvas_size);
	ImVec2 local_pos = ImVec2(ImGui::GetIO().MousePos.x - canvas_pos.x, ImGui::GetIO().MousePos.y - canvas_pos.y);
	ImVec2 bound_pos = ImVec2(ImGui::GetIO().MousePos.x - imageCorner.x, ImGui::GetIO().MousePos.y - imageCorner.y);

	if (adding_selec) {
		selec_prev = true;
		corners.push_back(local_pos);
		bounds.push_back(bound_pos);
		if (!ImGui::GetIO().MouseDown[1])
			adding_selec = selec_prev = false;
	}
	if (ImGui::IsItemHovered()) {
		//std::cout << "hover" << std::endl;
		if (!adding_selec && ImGui::IsMouseClicked(1)) {
			corners.push_back(local_pos);
			bounds.push_back(bound_pos);
			adding_selec = true;
		}
		//std::cout << (ImGui::IsKeyDown(ImGuiKey_Backspace)) << std::endl;
		//std::cout << (ImGui::IsMouseClicked(2)) << std::endl;
		if (ImGui::IsMouseClicked(2) && !corners.empty()) {
			std::cout << "pressed" << std::endl;
			adding_selec = selec_prev = false;
			corners.pop_back();
			corners.pop_back();
			bounds.pop_back();
			bounds.pop_back();
		}
	}
	for (int i = 0; i < corners.Size - 1; i += 2) {
		list->AddRect(ImVec2(canvas_pos.x + corners[i].x, canvas_pos.y + corners[i].y), ImVec2(canvas_pos.x + corners[i + 1].x, canvas_pos.y + corners[i + 1].y), IM_COL32(255, 0, 0, 255), 2.0f);

		points->lower_bounds.x = (bounds[i].x) / imaging_data.data[0].width;
		points->lower_bounds.y = (bounds[i].y) / imaging_data.data[0].height;
		points->upper_bounds.x = (bounds[i + 1].x) / imaging_data.data[0].width;
		points->upper_bounds.y = (bounds[i + 1].y) / imaging_data.data[0].height;
		//std::cout << (points->upper_bounds.x) << " , " << (points->upper_bounds.y) << "   :   " << (points->lower_bounds.x) << " , " << (points->lower_bounds.y) << "  :  " << (first) << " , " << (last) << std::endl;
	}
	if (selec_prev) {
		corners.pop_back();
		bounds.pop_back();
	}
	ImGui::EndChild();
	ImGui::PopStyleVar();
	ImGui::Spacing();
	ImGui::PushID(1);
	ImGui::PushStyleColor(ImGuiCol_Button, ImColor::HSV(0 / 7.0f, 0.6f, 0.6f));
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImColor::HSV(0 / 7.0f, 0.7f, 0.7f));
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImColor::HSV(0 / 7.0f, 0.8f, 0.8f));
	fslide = ImGui::Button("First Focus Slice");
	if (fslide) {
		first = imaging_data.current_index;
		std::cout << "first " << (first) << std::endl;
		points->lower_bounds.z = -0.5 + (first / imaging_data.data.size());
	}
	
	ImGui::PopStyleColor(3);
	ImGui::PopID();

	ImGui::SameLine(0, 150);
	
	ImGui::PushID(2);
	ImGui::PushStyleColor(ImGuiCol_Button, ImColor::HSV(1 / 7.0f, 0.6f, 0.6f));
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImColor::HSV(1 / 7.0f, 0.7f, 0.7f));
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImColor::HSV(1 / 7.0f, 0.8f, 0.8f));
	lslide = ImGui::Button("Last Focus Slice");
	if (lslide) {
		last = imaging_data.current_index;
		points->upper_bounds.z = -0.5 + (last / imaging_data.data.size());
		std::cout << "last " << (last) << std::endl;
	}
	ImGui::PopStyleColor(3);
	ImGui::PopID();

	// Button to export
	ImGui::PushID(3);
	ImGui::PushStyleColor(ImGuiCol_Button, ImColor::HSV(1 / 7.0f, 0.6f, 0.6f));
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImColor::HSV(1 / 7.0f, 0.7f, 0.7f));
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImColor::HSV(1 / 7.0f, 0.8f, 0.8f));
	exportButtonPressed = ImGui::Button("Export");
	if (exportButtonPressed) {
		std::cout << "Exporting trace" << std::endl;
		for (int i = 0; i < points->branch_points.size(); i++) {
			std::cout << "branch point " << i << points->branch_points[i]->position.x << " "
				<< points->branch_points[i]->position.y << " " << points->branch_points[i]->position.z << std::endl;
		}
	}

	ImGui::PopStyleColor(3);
	ImGui::PopID();

	// ======================== NEED TO SWITCH MOUSE BUTTONS TO VR BUTTONS ==================================
	
	ImGui::NextColumn();
	bool wc_changed = ImGui::SliderInt("Window Center", &imaging_data.window_center, TMP_MIN_ISOVALUE, TMP_MAX_ISOVALUE);
	bool ww_changed = ImGui::SliderInt("Window Width", &imaging_data.window_width, TMP_MIN_WW, TMP_MAX_WW);	
	bool less = ImGui::Button("<"); ImGui::SameLine(); bool more = ImGui::Button(">"); ImGui::SameLine();
	bool slice_index_changed = ImGui::SliderInt("Slice Index", &imaging_data.current_index, 0, imaging_data.data.size()-1);
	ImGui::SameLine(); ShowHelpMarker("CTRL + click to input value.");
	if (less) {
		slice_index_changed = true;
		imaging_data.current_index--;
	}
	if (more) {
		slice_index_changed = true;
		imaging_data.current_index++;
	}
	// todo, correct calculation going from index to model space 
	//ALTERNATIVE: make isovalue and slide interaction all 2D ui that can be called up or confirmed to minimize
	float sliceZPos = imaging_data.current_index / (imaging_data.data.size() - 1);
	//viewer->orthoslice_handle->SetModelPositionZ(sliceZPos);
	
	if(wc_changed || ww_changed || slice_index_changed) 
		viewer->orthoslice_texture->Load(imaging_data.data[imaging_data.current_index], imaging_data.window_width, imaging_data.window_center);

	// isovalue point cloud sliders
	ImGui::PushStyleVar(ImGuiStyleVar_ChildWindowRounding, 5.0f);
	ImGui::BeginChild("values", ImVec2(0, 300), true);
	ImGui::Text("Isovalue Point Cloud Sliders");

	// ==================== create slider struct in header ====================================

	for (int i = 0; i < isovalue_point_cloud_sliders.size(); ++i) {
		if (!isovalue_point_cloud_sliders[i]->in_use) continue;
		ImVec4 color = ImColor(isovalue_point_cloud_sliders[i]->color.x, isovalue_point_cloud_sliders[i]->color.y, isovalue_point_cloud_sliders[i]->color.z);
		ImGui::ColorButton(color);
		if (ImGui::BeginPopupContextItem(""))
		{
			ImGui::Text("Edit color");
			ImGui::ColorEdit3("##edit" + i, (float*)&color);
			if (ImGui::Button("Close"))
				ImGui::CloseCurrentPopup();
			ImGui::EndPopup();
			isovalue_point_cloud_sliders[i]->SetColor(glm::vec3(color.x, color.y, color.z));
		}
		//ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(col.x, col.y, col.z, 1));
		ImGui::SameLine();
		sliderHasChanged = ImGui::SliderFloat(("" + std::to_string(i)).c_str(), &isovalue_point_cloud_sliders[i]->curr_isovalue, TMP_MIN_ISOVALUE, TMP_MAX_ISOVALUE);
		ImGui::SameLine(); ShowHelpMarker("right-click color square to change");
		if (sliderHasChanged) {
			points->MarkForRegeneration();
		}
		UpdateDicomPointCloud(isovalue_point_cloud_sliders[i]->curr_isovalue);
		ImGui::SameLine();
		//ImGui::PopStyleColor(1);
		

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1.0,0,0,1));
		pushed = ImGui::Button(("X##" + std::to_string(isovalue_point_cloud_sliders[i]->id)).c_str(), ImVec2(50, 20));
		if (pushed) {
			isovalue_point_cloud_sliders[i]->SetInUse(false);
		}
		
		ImGui::PopStyleColor(1);
	}
	ImGui::EndChild();
	ImGui::PopStyleVar();
	// add custom rendering app to draw "magnification square" over area of interest
}

void DicomObjectsContainer::Update(const VrData& _vr, const CursorData& _crsr)
{
	RenderUi();

	glm::mat4 curr_pose;

	// Refactor
	/*if (viewer->base_handle->is_double_selected)
	{
		//viewer->SetMasterAppendPose(viewer->base_handle->GetDoubleSelectionTransform());
		//SetCoarseViewerAppendPose(viewer->base_handle->GetDoubleSelectionScaleDifference());
	}	
	else if(viewer->base_handle->is_selected)
	{
		curr_pose = viewer->base_handle->cache.controller_pose_updated * viewer->base_handle->cache.to_controller_space_initial;
		//viewer->SetMasterAppendPose(curr_pose);
		//SetCoarseViewerAppendPose(curr_pose);
	}*/

	static bool once = true;
	static glm::vec3 offset;

	// Refactor
	/*if (viewer->point_cloud_selector->is_selected)
	{
		if (viewer->point_cloud_selector->is_double_selected)
		{
			// TODO: fix position jump when secondary controller becomes primary
			float start_interesection_ray_len = glm::length(viewer->point_cloud_selector->cache.primary_to_secondary_collision_point_initial);
			float curr_intersection_ray_len = glm::length(viewer->point_cloud_selector->cache.primary_to_secondary_collision_point_current);
			float intersection_ray_ratio = last / first;// curr_intersection_ray_len / start_interesection_ray_len;

			viewer->point_cloud_selector->SetScale(intersection_ray_ratio * viewer->point_cloud_selector->cache.initial_scale);
			viewer->point_cloud_selector_scale = viewer->point_cloud_selector->GetScale();
			once = true;
		}
		else
		{
			glm::vec3 pointer_model_space = glm::vec3(glm::inverse(viewer->base_handle->GetModelMatrix()) 
			* glm::vec4(viewer->point_cloud_selector->cache.primary_collision_point_world_current, 1.0f));

			if (once)
			{
				offset = viewer->point_cloud_selector->GetModelPosition() - pointer_model_space;
				once = false;
			}

			viewer->point_cloud_selector->SetModelPosition(pointer_model_space + offset);
		}
	}
	else
	{
		if (!once)
		{
			// point cloud selector was moved and released
			//viewer->selector_lower_bounds = viewer->point_cloud_selector->GetModelPosition() - glm::vec3(0.5f*viewer->point_cloud_selector_scale);
			//viewer->selector_upper_bounds = viewer->point_cloud_selector->GetModelPosition() + glm::vec3(0.5f*viewer->point_cloud_selector_scale);
			//viewer->selector_lower_bounds = glm::vec3(first);
			//viewer->selector_upper_bounds = glm::vec3(last);
			//points->lower_bounds = viewer->selector_lower_bounds;
			//points->upper_bounds = viewer->selector_upper_bounds;
			points->curr_tolerance = 10;
			points->Generate(imaging_data, imaging_data.isovalue, 10, first, last, isovalue_point_cloud_sliders);
		}

		once = true;
	}*/

	// TODO: round down model position to nearest slice
	// Refactor
	/*if (viewer->orthoslice_handle->is_selected)
	{
		glm::vec4 colp_to_model_space = glm::inverse(viewer->base_handle->GetModelMatrix()) * glm::vec4(viewer->orthoslice_handle->cache.primary_collision_point_world_current, 1.0f);
		colp_to_model_space.z = glm::clamp(colp_to_model_space.z, 0.0f, imaging_data.scale.z);

		imaging_data.current_index = (float)(imaging_data.data.size() - 1) * (colp_to_model_space.z / imaging_data.scale.z);
		viewer->orthoslice_texture->Load(imaging_data.data[imaging_data.current_index], imaging_data.window_width, imaging_data.window_center);

		viewer->orthoslice_handle->SetModelPositionZ(colp_to_model_space.z);
		viewer->orthoslice->SetModelPositionZ(colp_to_model_space.z);
		//std::cout << std::endl << "modelZ: " << (colp_to_model_space.z) << "  , imageScaleZ: " << (imaging_data.scale.z) << "  , index: " << (imaging_data.current_index) << std::endl;
	}*/

	if (points->handle->is_double_selected)
	{
		curr_pose = points->handle->GetDoubleSelectionTransform();
		points->SetMasterAppendPose(curr_pose);
	}
	else if (points->handle->is_selected) 
	{
		curr_pose = points->handle->cache.controller_pose_updated * points->handle->cache.to_controller_space_initial;
		points->SetMasterAppendPose(curr_pose);
	}

	// Refactor
	/*if (viewer->orthoslice->WasClicked())
	{
		glm::vec4 colp_to_model_space = glm::inverse(viewer->base_handle->GetModelMatrix()) * glm::vec4(viewer->orthoslice->cache.primary_collision_point_world_current, 1.0f);

		// should be 1by1 in model space
		colp_to_model_space.x = glm::clamp(colp_to_model_space.x, 0.0f, imaging_data.scale.x);
		colp_to_model_space.y = glm::clamp(colp_to_model_space.y, 0.0f, imaging_data.scale.y);

		int index_x = (float)imaging_data.data[imaging_data.current_index].width * colp_to_model_space.x;
		int index_y = (float)imaging_data.data[imaging_data.current_index].height * colp_to_model_space.y;
		imaging_data.isovalue = imaging_data.data[imaging_data.current_index].isovalues.at(imaging_data.data[imaging_data.current_index].width * index_y + index_x);

		AddIsovaluePointCloudSlider(imaging_data.isovalue);
		points->MarkForRegeneration();
	}*/
	

	// only does work if needed
	points->Generate(imaging_data, -1, MAX_ISOVALUE_TOLERANCE, isovalue_point_cloud_sliders);

	// drawing branches in VR
	if (_vr.controller1.touchpad_is_pressed && !newCurve)
	{
		std::cout << "dpad is pressed" << std::endl;
		newCurve = true;
		points->branch_points.clear();
		curve = new Curve();
	}
	static BranchPoint* prev = NULL;
	static const float dist_threshold_to_existing = 0.1f;

	// test if controller is close to old branch point
	if (_vr.controller1.trigger_first_press)
	{
		// find  the closest branch point		
		
		int closest_index = -1;
		float curr_min = -1.0f;
		bool found = false;

		for (int i = 0; i < points->branch_points.size(); ++i)
		{
			float len = glm::length(points->branch_points[i]->position - _vr.controller1.position);

			if (len <= dist_threshold_to_existing && len <= (curr_min || !found))
			{
				curr_min = len;
				closest_index = i;
				found = true;
			}
		}

		// if so, start a new branch from close branch point
		if (found)
		{
			prev = points->branch_points[closest_index];
		}
		// else new disconnected branch
		else
		{
			prev = NULL;
		}
	}
	else if (_vr.controller1.trigger_is_pressed)
	{
		if (prev != NULL)
		{
			static const float new_bp_dist_threshold = 0.1f;

			if (glm::length(prev->position - _vr.controller1.position) >= new_bp_dist_threshold)
			{
				glm::vec4 controller_pos_in_point_space = glm::inverse(points->GetModelMatrix()) * glm::vec4(_vr.controller1.position, 1.0f);
				glm::vec4 tmp = points->GetModelMatrix() * glm::inverse(points->GetModelMatrix()) * glm::vec4(_vr.controller1.position, 1.0f);
				BranchPoint* newBP = new BranchPoint(glm::vec3(controller_pos_in_point_space) - points->lower_bounds);
				points->branch_points.push_back(newBP);
				prev->neighbors.push_back(newBP->id);
				prev = newBP;
			}
		}
		// first point of disconnected branch
		else
		{
			glm::vec4 controller_pos_in_point_space = glm::inverse(points->GetModelMatrix()) * glm::vec4(_vr.controller1.position, 1.0f);
			BranchPoint* newBP = new BranchPoint(glm::vec3(controller_pos_in_point_space) - points->lower_bounds);
			points->branch_points.push_back(newBP);
			prev = newBP;
		}
	}
	
	if (points->branch_points.size() >= 4)
	{
		std::vector<BranchPoint*> pointsToFit;
		BranchPoint* prevBranchPoint = points->branch_points[0];
		//std::cout << "branch points to fit" << std::endl;
		for (int i = 1; i < points->branch_points.size(); i++) //TODO: For each click, duplicate branch points keep getting added, probably a bug in SpoofInput?!
		{
			if (glm::notEqual(points->branch_points[i]->position, prevBranchPoint->position).b)
			{
				/*std::cout << i << " :: " << points->branch_points[i]->position.x << " "
					<< points->branch_points[i]->position.y << " "
					<< points->branch_points[i]->position.z << " "
					<< std::endl;*/
				pointsToFit.push_back(points->branch_points[i]);
				prevBranchPoint = points->branch_points[i];
			}
		}
		
		int currPointsToFitCount = pointsToFit.size();
		if (currPointsToFitCount != pointsToFitCount && (currPointsToFitCount == 4 
			|| currPointsToFitCount > 4 && currPointsToFitCount - pointsAlreadyFitCount == 3))
		{
			//TODO: Put write to file in a DEBUG mode
			std::ofstream inputFile("input.dat", std::ios::out);
			if (inputFile.is_open())
			{
				for (BranchPoint* inputPoint : pointsToFit)
				{
					inputFile << inputPoint->position.x << " " << inputPoint->position.y << " " << inputPoint->position.z << "\n";
				}
				inputFile.close();
			}
			else
			{
				std::cout << "Unable to open file input.dat" << std::endl;
			}

			//std::cout << "fitting points" << std::endl;
			LeastSquaresFit leastSquaresFit(pointsToFit, 1);
			leastSquaresFit.Fit();
			std::vector<glm::vec3> curvePoints = leastSquaresFit.GetCurvePoints();

			//TODO: Put write to file in a DEBUG mode
			std::ofstream outputFile("output.dat", std::ios::out);
			if (outputFile.is_open())
			{
				for (glm::vec3 outputPoint : curvePoints)
				{
					outputFile << outputPoint.x << " " << outputPoint.y << " " << outputPoint.z << "\n";
				}
				outputFile.close();
			}
			else
			{
				std::cout << "Unable to open file output.dat" << std::endl;
			}

			curve->SetPositions(curvePoints);
			curve->SetNormals(curvePoints);
			//std::cout << "newCurve " << newCurve << std::endl;
			if (newCurve || points->curves.empty())
			{
				//std::cout << "Pushing new curve" << std::endl;
				points->curves.push_back(curve);
			}
			else
			{
				//std::cout << "Updating old curve" << std::endl;
				points->curves.back() = curve;
			}
			//std::cout << "curves :: " << points->curves.size() << std::endl;
			for (Curve* curveInstance : points->curves)
			{
				curveInstance->RenderCurve();
			}
			pointsToFitCount = currPointsToFitCount;
			pointsAlreadyFitCount = currPointsToFitCount;
			newCurve = false;
		}
	}

	// Refactor
	/*debug1->SetAppendPose(viewer->base_handle->GetAppendPose());
	debug2->SetAppendPose(viewer->base_handle->GetAppendPose());*/
	//debug1->SetWorldPosition(points->lower_bounds);
	//debug2->SetWorldPosition(points->upper_bounds);
}

void DicomObjectsContainer::AddObjects(Render* _r) 
{
	_r->AddObjectToScene(points);
	viewer->AddObjects(_r);

	_r->AddObjectToScene(debug1);
	_r->AddObjectToScene(debug2);
}

void DicomObjectsContainer::AddIsovaluePointCloudSlider(const int _isovalue)
{
		bool unused_slider_slot_found = false;

		for (int i = 0; i < isovalue_point_cloud_sliders.size(); ++i)
		{
			if (!isovalue_point_cloud_sliders[i]->in_use)
			{
				isovalue_point_cloud_sliders[i]->SetInUse(true);

				unused_slider_slot_found = true;
				break;
			}
		}
}

void DicomObjectsContainer::Load(std::string _dicomDir)
{
	imaging_data = DicomReader::ReadSet(_dicomDir);
	viewer->Load(imaging_data);
	UpdateDicomPointCloud(DEFAULT_ISOVALUE);
}

void DicomObjectsContainer::UpdateDicomPointCloud(int _isovalue)
{
	imaging_data.isovalue = _isovalue;
	points->Generate(imaging_data, _isovalue, MAX_ISOVALUE_TOLERANCE, first, last, isovalue_point_cloud_sliders);
}

