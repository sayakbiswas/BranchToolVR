#include "DicomObjectsContainer.h"

//ColorObject* debug1 = new ColorObject;
//ColorObject* debug2 = new ColorObject;
bool pushed, fslide, lslide, ready, decimate, sliderHasChanged, exportButtonPressed;
bool IGuide = false;
bool VRGuide = false;
bool undo_curve = false;
std::string folder = "";

int IsovaluePointCloudSlider::id_counter = 0;
const int max_nr_isovalue_point_cloud_sliders = MAX_NR_POINT_CLOUD_SLIDERS;
float IsovaluePointCloudSlider::min_isovalue = TMP_MIN_ISOVALUE;
float IsovaluePointCloudSlider::max_isovalue = TMP_MAX_ISOVALUE;

DicomObjectsContainer::DicomObjectsContainer()
{
	points = new DicomPointCloudObject;
	viewer = new CoarseDicomViewer;
/*
	ControllerDiagram = new TextureObject;
	ControllerImage = new Texture;

	ControllerDiagram->is_clickable = false;
	ControllerDiagram->GenerateXYPlane(1.0f, 1.0f, 0.0f, glm::vec3(0.0f));
	ControllerDiagram->texture_level = CONTROLLER;
	ControllerImage->Load("Controller");*/

	float initial_scale = 0.5f;
	glm::vec3 initial_position = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::mat4 tmp_initial_model_matrix = glm::translate(glm::mat4(1.0f), initial_position) * glm::scale(glm::mat4(1.0f), glm::vec3(initial_scale));
	points->SetMasterAppendPose(tmp_initial_model_matrix);
	viewer->SetMasterAppendPose(tmp_initial_model_matrix);

	//debug1->GenerateSphere(10, 0.05f, false);
	//debug2->GenerateSphere(10, 0.05f, false);

	for (int i = 0; i < max_nr_isovalue_point_cloud_sliders; ++i)
	{
		isovalue_point_cloud_sliders.push_back(new IsovaluePointCloudSlider);
	}
}

DicomObjectsContainer::~DicomObjectsContainer()
{
	delete points;
	delete viewer;
	/*delete ControllerDiagram;
	delete ControllerImage;*/
}

// Changed from static function outside of .h file to member function
void DicomObjectsContainer::FileMenu() {
	/*if (ImGui::MenuItem("New", "CTRL+N")) {
		// dialog file selection
	}*/
	if (ImGui::MenuItem("Open Data Set")) {
		//dialog folder selection
		char filename[MAX_PATH];
		OPENFILENAME ofn;
		ZeroMemory(&filename, sizeof(filename));
		ZeroMemory(&ofn, sizeof(ofn));
		ofn.lStructSize = sizeof(ofn);
		ofn.hwndOwner = NULL;
		ofn.lpstrFile = filename;
		ofn.nMaxFile = MAX_PATH;
		ofn.lpstrTitle = "Select Staring Scan File:";
		ofn.Flags = OFN_FILEMUSTEXIST ;

		if (GetOpenFileNameA(&ofn)) {

			// Cutting filename to leave directory structure
			unsigned int j = sizeof(filename);
			while (filename[j] != '\\') {
				j--;
			}
			// Copying file directory structure to a string
			for (unsigned int i = 0; i < j; i++) {
				folder += filename[i];
			}
			std::cout << "You selected: " << folder << std::endl;
						
			this->Load(folder);
		}

		else
		{
			// Useful Sample Errors for Debugging
			switch (CommDlgExtendedError())
			{
			case CDERR_DIALOGFAILURE: std::cout << "CDERR_DIALOGFAILURE\n";   break;
			case CDERR_FINDRESFAILURE: std::cout << "CDERR_FINDRESFAILURE\n";  break;
			case CDERR_INITIALIZATION: std::cout << "CDERR_INITIALIZATION\n";  break;
			case CDERR_LOADRESFAILURE: std::cout << "CDERR_LOADRESFAILURE\n";  break;
			case CDERR_LOADSTRFAILURE: std::cout << "CDERR_LOADSTRFAILURE\n";  break;
			case CDERR_LOCKRESFAILURE: std::cout << "CDERR_LOCKRESFAILURE\n";  break;
			case CDERR_MEMALLOCFAILURE: std::cout << "CDERR_MEMALLOCFAILURE\n"; break;
			case CDERR_MEMLOCKFAILURE: std::cout << "CDERR_MEMLOCKFAILURE\n";  break;
			case CDERR_NOHINSTANCE: std::cout << "CDERR_NOHINSTANCE\n";     break;
			case CDERR_NOHOOK: std::cout << "CDERR_NOHOOK\n";          break;
			case CDERR_NOTEMPLATE: std::cout << "CDERR_NOTEMPLATE\n";      break;
			case CDERR_STRUCTSIZE: std::cout << "CDERR_STRUCTSIZE\n";      break;
			case FNERR_BUFFERTOOSMALL: std::cout << "FNERR_BUFFERTOOSMALL\n";  break;
			case FNERR_INVALIDFILENAME: std::cout << "FNERR_INVALIDFILENAME\n"; break;
			case FNERR_SUBCLASSFAILURE: std::cout << "FNERR_SUBCLASSFAILURE\n"; break;
			default: std::cout << "You cancelled.\n";
			}
		}
		folder = "";
	}
	/*ImGui::Separator();
	if (ImGui::MenuItem("Save", "Ctrl+S")) {
	}
	if (ImGui::MenuItem("Save As...", "SHIFT+CTRL+S")) {
	}
	ImGui::Separator();
	if (ImGui::MenuItem("Close", "CTRL+Q")) {

	}*/
}

void ShowUserGuide(bool _IGuide) {
	if (!_IGuide) return;
	ImGui::Begin("Interface Guide", &_IGuide);
	ImGui::BulletText("Double-click on title bar to collapse window");
	ImGui::BulletText("Click and drag on lower right corner to resize window");
	ImGui::BulletText("Click and drag on any empty space to move window");
	ImGui::BulletText("Mouse Wheel to scroll");
	ImGui::BulletText("CTRL+Click on a slider or drag box to input value directly");
	ImGui::BulletText("Window Center and Width adjust display for the data set");
	ImGui::BulletText(
		"Slice Index navigates through 'slices' in data set\n"
		"- First Focus Slice selects the starting boundary slice\n"
		"- Last Focus Slice selects the ending boundary slice\n");
	ImGui::BulletText("Click and drag with Right Mouse Button in data set display to select region");
	ImGui::BulletText("Click Middle Mouse Button to undo region drawing");
	ImGui::BulletText("Click a point in the set to select an isovalue for 3D point cloud");
	ImGui::BulletText("Adjust value and tolerance as necessary");
	ImGui::BulletText("'Decimate' is a toggle option that cuts down the number of points in the cloud, sacrificing detail for memory");
	ImGui::BulletText(
		"Controls for viewing cloud without VR:\n"
		"- WASD: Move forward, left, back, right\n"
		"- Move mouse to look around");
	ImGui::BulletText(
		"When cloud is ready, click Headset Ready to enter VR interface\n"
		"-This will 'lock in' certain values' representations, so be sure everything looks ready\n"
		"-Isovalues can still be created/changed/decimated without weirdness");
	ImGui::End();
	return;
}

// Labeled controller diagram (based on DicomSet display in interface)
// Stopped displaying image at some point without code change, still unfixed
void DicomObjectsContainer::ShowControllerDiagram(bool _VRGuide) {
	if (!_VRGuide) return;
	ImGui::Begin("VR Guide", &_VRGuide);
	ImGui::BulletText("Draw Control Points: Main controller (Green) Trigger");
	ImGui::BulletText("Start New Curve: Press Right Side of Touchpad");
	ImGui::BulletText("Undo Curve: Press Left Side of Touchpad");
	ImGui::BulletText(
		"Move Objects (Lights and Point Cloud):\n"
		"-Select: Alt (palm/ring finger) Button\n"
		"-Movement follows hand motion"
	);
	ImGui::BulletText("Scale Cloud: Select with both hands and move apart/together");
	ImGui::Image((void*)viewer->ControllerImage->GetGlId(), ImVec2(390, 390), ImVec2(0, 0), ImVec2(1, 1));
	//ImGui::EndChild();
	//ImGui::PopStyleVar();
	ImGui::End();
	return;
}

// Changed from static function outside of .h file to member function
void DicomObjectsContainer::MainMenuBar() {
	if (ImGui::BeginMainMenuBar()) {
		if (ImGui::BeginMenu("File")) {
			FileMenu();
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Edit")) {
			if (ImGui::MenuItem("Undo Curve")) {
				points->curves.pop_back();
			}
			if (ImGui::MenuItem("Clear all Curves")) {
				points->curves.clear();
			}
			ImGui::Separator();
			if (ImGui::MenuItem("Clear Isovalues", NULL)) {
				for (int i = 0; i < isovalue_point_cloud_sliders.size(); ++i) {
					isovalue_point_cloud_sliders[i]->SetInUse(false);
				}
				points->MarkForRegeneration();
			}
			if (ImGui::MenuItem("Clear all Fields")) {
				points->curves.clear();
				first = 0;
				last = 100;
				for (int i = 0; i < isovalue_point_cloud_sliders.size(); ++i) {
					isovalue_point_cloud_sliders[i]->SetInUse(false);
				}
				points->MarkForRegeneration();
			}
			ImGui::EndMenu();
		}
		/*if (ImGui::BeginMenu("Window")) {
			---- Potential View features:
					transparency slider
					text size slider
					Show CT ui window
					show mini 3D CT setup 
			ImGui::EndMenu();
		}*/
		if (ImGui::BeginMenu("Help")) {
			if (ImGui::MenuItem("Interface Guide")) {
				IGuide = true;
			}
			if (ImGui::MenuItem("VR Guide")) {
				VRGuide = true;
			}
			ImGui::EndMenu();
		}

		ShowUserGuide(IGuide);
		ShowControllerDiagram(VRGuide);

		ImGui::EndMainMenuBar();
	}
}

void DicomObjectsContainer::RenderUi(Render* _r)
{
	// ========== Main Menu Bar ===========

	bool b = true;

	ImGui::Begin("Dicom Set", &b);
	MainMenuBar();

	ImGui::Columns(2, "mixed");
	ImGui::Separator();
	ImGui::PushStyleVar(ImGuiStyleVar_ChildWindowRounding, 5.0f);
	ImGui::BeginChild("slides", ImVec2(0, 700), true, 1);
	ImGui::Text("");
	//ImVec2 tex_screen_pos = ImGui::GetCursorScreenPos();
	//float tex_w = (float)viewer->orthoslice_texture->width;
	//float tex_h = (float)viewer->orthoslice_texture->height;
	ImGui::ImageButton((void*)viewer->orthoslice_texture->GetGlId(), ImVec2(viewer->orthoslice_texture->width, viewer->orthoslice_texture->height), ImVec2(0, 0), ImVec2(1, 1), -1, ImVec4(0, 0, 0, 1), ImVec4(1, 1, 1, 1));
	
	// Isovalue Magnifier
	//if (ImGui::IsItemHovered())
	//{
	//	//enlarged view for isovalue selection
	//	ImGui::BeginTooltip();
	//	float focus_sz = 32.0f;
	//	float focus_x = ImGui::GetMousePos().x - tex_screen_pos.x - focus_sz * 0.5f; if (focus_x < 0.0f) focus_x = 0.0f; else if (focus_x > tex_w - focus_sz) focus_x = tex_w - focus_sz;
	//	float focus_y = ImGui::GetMousePos().y - tex_screen_pos.y - focus_sz * 0.5f; if (focus_y < 0.0f) focus_y = 0.0f; else if (focus_y > tex_h - focus_sz) focus_y = tex_h - focus_sz;
	//	//ImGui::Text("Min: (%.2f, %.2f)", focus_x, focus_y);
	//	//ImGui::Text("Max: (%.2f, %.2f)", focus_x + focus_sz, focus_y + focus_sz);
	//	ImVec2 uv0 = ImVec2((focus_x) / tex_w, (focus_y) / tex_h);
	//	ImVec2 uv1 = ImVec2((focus_x + focus_sz) / tex_w, (focus_y + focus_sz) / tex_h);
	//	ImGui::Image((void*)7, ImVec2(128, 128), uv0, uv1);
	//	ImGui::EndTooltip();
	//}

	if (ImGui::IsItemClicked(0)) { //select isovalue
		ImVec2 mousePos = ImGui::GetMousePos();
		ImVec2 imageCorner = ImGui::GetItemRectMin();
		float x = mousePos.x - imageCorner.x;
		float y = mousePos.y - imageCorner.y;
		imaging_data.isovalue = imaging_data.data[imaging_data.current_index].isovalues.at(imaging_data.data[imaging_data.current_index].width * y + x);

		AddIsovaluePointCloudSlider(imaging_data.isovalue);
		points->MarkForRegeneration();
		points->Generate(imaging_data, imaging_data.isovalue, 30, first, last, isovalue_point_cloud_sliders);
		IsovaluePointCloudSlider::min_isovalue = imaging_data.isovalue - 300;
		IsovaluePointCloudSlider::max_isovalue = imaging_data.isovalue + 300;
	}


	// ========== draw bounding rectangle =============
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
		// Print exact isovalue at mouse location below set display 
		float x = (mousePos.x - imageCorner.x);
		float y = (mousePos.y - imageCorner.y);
		if (imaging_data.data[imaging_data.current_index].width * y + x < imaging_data.data[imaging_data.current_index].isovalues.size())
			ImGui::Text("Value: (%i)", imaging_data.data[imaging_data.current_index].isovalues.at(imaging_data.data[imaging_data.current_index].width * y + x));
		if (!adding_selec && ImGui::IsMouseClicked(1)) {
			corners.push_back(local_pos);
			bounds.push_back(bound_pos);
			adding_selec = true;
		}
		if (ImGui::IsMouseClicked(2) && !corners.empty()) {
			//std::cout << "pressed" << std::endl;
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

		if (points->lower_bounds.x > points->upper_bounds.x) {
			float temp_x = points->lower_bounds.x;
			points->lower_bounds.x = points->upper_bounds.x;
			points->upper_bounds.x = temp_x;
		}

		if (points->lower_bounds.y > points->upper_bounds.y) {
			float temp_y = points->lower_bounds.y;
			points->lower_bounds.y = points->upper_bounds.y;
			points->upper_bounds.y = temp_y;
		}
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

	ImGui::SameLine();

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
	ImGui::PushStyleColor(ImGuiCol_Button, ImColor::HSV(1 / 15.0f, 0.6f, 0.6f));
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImColor::HSV(1 / 15.0f, 0.7f, 0.7f));
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImColor::HSV(1 / 15.0f, 0.8f, 0.8f));
	exportButtonPressed = ImGui::Button("Export");
	if (exportButtonPressed) {
		std::cout << "Exporting trace" << std::endl;
		std::ofstream curvesFile("curves.dat", std::ios::out);
		if (curvesFile.is_open())
		{
			int curveCount = 0;
			for (Curve* curve : points->curves)
			{
				curvesFile << "### Curve " << curveCount++ << " ###\n";
				for (glm::vec3 controlPoint : curve->GetControlPoints())
				{
					// X seems right, look into swapping Y/Z (Blender up and Branch Tool up are different)
					curvesFile << controlPoint.x + ((points->upper_bounds.x - points->lower_bounds.x) / 4)
						<< " " << controlPoint.y + ((points->upper_bounds.y - points->lower_bounds.y) / 4)
						<< " " << -1.0 * controlPoint.z  * abs(controlPoint.z - points->getZoffset()) << "\n";
				}
			}
			curvesFile.close();
		}
		else
		{
			std::cout << "Unable to open file curves.dat" << std::endl;
		}

		// separate points by color into different files
		std::vector<glm::vec3> colors = points->GetInstancedColor();
		for (int slider_i = 0; slider_i < sliderCount; ++slider_i) {
			int index = 0;
			int indices = 0;
			std::ostringstream ply;
			ply << "tumor" << slider_i << ".ply";
			std::ofstream pointCloudPLYFile(std::string(ply.str()), std::ios::out);
			
			if (pointCloudPLYFile.is_open())
			{
				unsigned int i_count = 0;
				
				for (glm::vec3 instanced_position : points->GetInstancedPositions())
				{
					if (i_count == 0) {
						pointCloudPLYFile << "ply\n";
						pointCloudPLYFile << "format ascii 1.0\n";
						pointCloudPLYFile << "element vertex " << isovalue_point_cloud_sliders[slider_i]->point_size << "\n";
						pointCloudPLYFile << "property float x\n";
						pointCloudPLYFile << "property float y\n";
						pointCloudPLYFile << "property float z\n";
						pointCloudPLYFile << "end_header\n";
					}
					if (colors.at(i_count) == isovalue_point_cloud_sliders[slider_i]->color) {
						pointCloudPLYFile << instanced_position.x << " " << instanced_position.y << " " << instanced_position.z << "\n";						
					}
					i_count++;
				}				
					pointCloudPLYFile.close();
					std::cout << "end ply for loop" << std::endl;
			}
			else
			{
				std::cout << "Unable to open file tumor.ply" << std::endl;
			}
			std::ostringstream xyz;
			xyz << "tumor" << slider_i << ".xyz";
			std::ofstream pointCloudXYZFile(std::string(xyz.str()), std::ios::out);
			if (pointCloudXYZFile.is_open())
			{
				unsigned int i_count = 0;
				for (glm::vec3 instanced_position : points->GetInstancedPositions())
				{
					if (colors.at(i_count) == isovalue_point_cloud_sliders[slider_i]->color) {
						pointCloudXYZFile << instanced_position.x << " " << instanced_position.y << " " << instanced_position.z << "\n";	
					}
					i_count++;
				}
				pointCloudXYZFile.close();
				std::cout << "end xyz for loop" << std::endl;
			}
			else
			{
				std::cout << "Unable to open file tumor.xyz" << std::endl;
			}

			std::ostringstream h;
			h << "hexmesh" << slider_i << ".obj";
			std::ofstream hexMeshFile(h.str(), std::ios::out);
			
			if (hexMeshFile.is_open()) {
				unsigned int i_count = 0;
				hexMeshFile << std::fixed << std::setprecision(8);
				hexMeshFile << "# OBJ file describing the hex mesh of the organ to be carved\n";
				hexMeshFile << "# List of geometric vertices, w defaults to 1.0\n";
				
				for (glm::vec3 instanced_position : points->GetInstancedPositions()) {
					if (colors.at(i_count) == isovalue_point_cloud_sliders[slider_i]->color) {
						glm::vec3 hex_vert_0 = glm::vec3(instanced_position.x - 0.5f * points->voxel_scale.x, instanced_position.y - 0.5f * points->voxel_scale.y, instanced_position.z + 0.5f * points->voxel_scale.z);
						glm::vec3 hex_vert_1 = glm::vec3(instanced_position.x + 0.5f * points->voxel_scale.x, instanced_position.y - 0.5f * points->voxel_scale.y, instanced_position.z + 0.5f * points->voxel_scale.z);
						glm::vec3 hex_vert_2 = glm::vec3(instanced_position.x + 0.5f * points->voxel_scale.x, instanced_position.y + 0.5f * points->voxel_scale.y, instanced_position.z + 0.5f * points->voxel_scale.z);
						glm::vec3 hex_vert_3 = glm::vec3(instanced_position.x - 0.5f * points->voxel_scale.x, instanced_position.y + 0.5f * points->voxel_scale.y, instanced_position.z + 0.5f * points->voxel_scale.z);
						glm::vec3 hex_vert_4 = glm::vec3(instanced_position.x - 0.5f * points->voxel_scale.x, instanced_position.y - 0.5f * points->voxel_scale.y, instanced_position.z - 0.5f * points->voxel_scale.z);
						glm::vec3 hex_vert_5 = glm::vec3(instanced_position.x + 0.5f * points->voxel_scale.x, instanced_position.y - 0.5f * points->voxel_scale.y, instanced_position.z - 0.5f * points->voxel_scale.z);
						glm::vec3 hex_vert_6 = glm::vec3(instanced_position.x + 0.5f * points->voxel_scale.x, instanced_position.y + 0.5f * points->voxel_scale.y, instanced_position.z - 0.5f * points->voxel_scale.z);
						glm::vec3 hex_vert_7 = glm::vec3(instanced_position.x - 0.5f * points->voxel_scale.x, instanced_position.y + 0.5f * points->voxel_scale.y, instanced_position.z - 0.5f * points->voxel_scale.z);

						// Blender OBJ import defaults to -Z forward
						hexMeshFile << "v " << hex_vert_0.x << " " << hex_vert_0.y << " " << -1.0 * hex_vert_0.z << "\n";
						hexMeshFile << "v " << hex_vert_1.x << " " << hex_vert_1.y << " " << -1.0 * hex_vert_1.z << "\n";
						hexMeshFile << "v " << hex_vert_2.x << " " << hex_vert_2.y << " " << -1.0 * hex_vert_2.z << "\n";
						hexMeshFile << "v " << hex_vert_3.x << " " << hex_vert_3.y << " " << -1.0 * hex_vert_3.z << "\n";
						hexMeshFile << "v " << hex_vert_4.x << " " << hex_vert_4.y << " " << -1.0 * hex_vert_4.z << "\n";
						hexMeshFile << "v " << hex_vert_5.x << " " << hex_vert_5.y << " " << -1.0 * hex_vert_5.z << "\n";
						hexMeshFile << "v " << hex_vert_6.x << " " << hex_vert_6.y << " " << -1.0 * hex_vert_6.z << "\n";
						hexMeshFile << "v " << hex_vert_7.x << " " << hex_vert_7.y << " " << -1.0 * hex_vert_7.z << "\n";
						indices++;
					}
					i_count++;
				}

				hexMeshFile << "# Polygonal face element\n";
				while (index < indices) {
					int ind_0 = 8 * index + 1; // Wavefront OBJ indices start from 1
					int ind_1 = 8 * index + 2;
					int ind_2 = 8 * index + 3;
					int ind_3 = 8 * index + 4;
					int ind_4 = 8 * index + 5;
					int ind_5 = 8 * index + 6;
					int ind_6 = 8 * index + 7;
					int ind_7 = 8 * index + 8;

					// Front
					hexMeshFile << "f " << ind_0 << " " << ind_1 << " " << ind_2 << "\n";
					hexMeshFile << "f " << ind_0 << " " << ind_2 << " " << ind_3 << "\n";

					// Back
					hexMeshFile << "f " << ind_5 << " " << ind_4 << " " << ind_7 << "\n";
					hexMeshFile << "f " << ind_5 << " " << ind_7 << " " << ind_6 << "\n";

					// Left
					hexMeshFile << "f " << ind_4 << " " << ind_0 << " " << ind_3 << "\n";
					hexMeshFile << "f " << ind_4 << " " << ind_3 << " " << ind_7 << "\n";

					// Right
					hexMeshFile << "f " << ind_1 << " " << ind_5 << " " << ind_6 << "\n";
					hexMeshFile << "f " << ind_1 << " " << ind_6 << " " << ind_2 << "\n";

					// Top
					hexMeshFile << "f " << ind_3 << " " << ind_2 << " " << ind_6 << "\n";
					hexMeshFile << "f " << ind_3 << " " << ind_6 << " " << ind_7 << "\n";

					// Bottom
					hexMeshFile << "f " << ind_1 << " " << ind_0 << " " << ind_4 << "\n";
					hexMeshFile << "f " << ind_1 << " " << ind_4 << " " << ind_5 << "\n";

					index++;
				}
				
				hexMeshFile.close();
				std::cout << "end obj for loop" << std::endl;
			}
			else {
				std::cout << "Unable to open file hexmesh.obj" << std::endl;
			} 
			std::cout << "end of slider loop cycle" << std::endl;
		}
		std::cout << "finished Exporting" << std::endl;
	}

	ImGui::PopStyleColor(3);
	ImGui::PopID();
	ImGui::SameLine();
	// Button to ready headset
	ImGui::PushID(4);
	ImGui::PushStyleColor(ImGuiCol_Button, ImColor::HSV(4 / 6.0f, 0.6f, 0.6f));
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImColor::HSV(4 / 6.0f, 0.7f, 0.7f));
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImColor::HSV(4 / 6.0f, 0.8f, 0.8f));
	if (ImGui::Button("Headset Ready")) {
		ready = true;
		_r->hmd_ready = true;

		_r->AddObjectToScene(_r->controller_pointer1);
		_r->AddObjectToScene(_r->controller_pointer2);

		viewer->selector->SetSelectorScale(points->getXYZrange());
	}

	ImGui::PopStyleColor(3);
	ImGui::PopID();

	// ======================== NEED TO SWITCH MOUSE BUTTONS TO VR BUTTONS ==================================

	ImGui::NextColumn();
	bool wc_changed = ImGui::SliderInt("Window Center", &imaging_data.window_center, TMP_MIN_ISOVALUE, TMP_MAX_ISOVALUE);
	bool ww_changed = ImGui::SliderInt("Window Width", &imaging_data.window_width, TMP_MIN_WW, TMP_MAX_WW);
	bool less = ImGui::Button("<"); ImGui::SameLine(); bool more = ImGui::Button(">"); ImGui::SameLine();
	bool slice_index_changed = ImGui::SliderInt("Slice Index", &imaging_data.current_index, 0, imaging_data.data.size() - 1);
	ImGui::SameLine(); ShowHelpMarker("CTRL + click to input value.");
	if (less) {
		slice_index_changed = true;
		imaging_data.current_index--;
	}
	if (more) {
		slice_index_changed = true;
		imaging_data.current_index++;
	}

	if (wc_changed || ww_changed || slice_index_changed)
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
			if (ImGui::Button("Close")) {
				ImGui::CloseCurrentPopup();
				points->MarkForRegeneration();
			}
			ImGui::EndPopup();
			isovalue_point_cloud_sliders[i]->SetColor(glm::vec3(color.x, color.y, color.z));
		}
		ImGui::SameLine();
		sliderHasChanged = ImGui::SliderFloat(("" + std::to_string(i)).c_str(), &isovalue_point_cloud_sliders[i]->curr_isovalue, IsovaluePointCloudSlider::min_isovalue, IsovaluePointCloudSlider::max_isovalue);

		ImGui::SameLine(); 
		ShowHelpMarker("right-click color square to change");

		toleranceHasChanged = ImGui::SliderInt(("Slider " + std::to_string(i) + " tolerance").c_str(), &isovalue_point_cloud_sliders[i]->iso_tolerance, 0, 30);
		if (sliderHasChanged || toleranceHasChanged) {
			points->MarkForRegeneration();
		}
		UpdateDicomPointCloud(isovalue_point_cloud_sliders[i]->curr_isovalue, isovalue_point_cloud_sliders[i]->iso_tolerance);

		ImGui::SameLine();
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1.0, 0, 0, 1));
		pushed = ImGui::Button(("X##" + std::to_string(isovalue_point_cloud_sliders[i]->id)).c_str(), ImVec2(20, 20));
		if (pushed) {
			isovalue_point_cloud_sliders[i]->SetInUse(false);
			points->MarkForRegeneration();
		}
		ImGui::PopStyleColor(1);

		ImGui::PushStyleColor(ImGuiCol_Button, ImColor::HSV(1 / 7.0f, 0.6f, 0.6f));
		decimate = ImGui::Button(("Decimate##" + std::to_string(isovalue_point_cloud_sliders[i]->id)).c_str());
		if (decimate) {
			isovalue_point_cloud_sliders[i]->ToggleDec();
			points->MarkForRegeneration();
		}
		ImGui::SameLine();
		ShowHelpMarker("Removes excess elements from generated mesh");
		ImGui::PopStyleColor(1);
	}
	ImGui::EndChild();
	ImGui::PopStyleVar();
	ImGui::End();
	// add custom rendering app to draw "magnification square" over area of interest
}

void DicomObjectsContainer::Update(const VrData& _vr, const CursorData& _crsr, Render* _r)
{
	RenderUi(_r);
	glm::mat4 curr_pose;
	// Refactor
	if (viewer->selector->is_double_selected)
	{
		//viewer->selector->SetSelectorScale(points->GetInstancedPositions());
		viewer->SetMasterAppendPose(viewer->selector->GetDoubleSelectionTransform());
		points->SetMasterAppendPose(viewer->selector->GetDoubleSelectionTransform());
		//SetCoarseViewerAppendPose(viewer->base_handle->GetDoubleSelectionScaleDifference());
	}
	else if(viewer->selector->is_selected)
	{
		curr_pose = viewer->selector->cache.controller_pose_updated * viewer->selector->cache.to_controller_space_initial;
		//viewer->selector->SetSelectorScale(points->GetInstancedPositions());
		viewer->SetMasterAppendPose(curr_pose);
		points->SetMasterAppendPose(curr_pose);
		//SetCoarseViewerAppendPose(curr_pose);
	}

	static bool once = true;
	static glm::vec3 offset;

	// only does work if needed
	points->Generate(imaging_data, -1, MAX_ISOVALUE_TOLERANCE, 1, 50, isovalue_point_cloud_sliders);

	//rotate cloud with touchpad
	if (_vr.controller2.touchpad_is_touched)
	{
		//viewer->SetMasterAppendPose(_vr.controller2.touch_rotate);
	}

	if (points->curves.empty()) curve = new Curve();

	// drawing branches in VR
	if (_vr.controller1.touchpad_is_pressed && !newCurve)
	{
		//std::cout << "touchpad is pressed" << std::endl;
		// Undo for curve from controller, only has effect on current curve (while red dots are visible)
		if (_vr.controller1.touch_axis.x < 0.0f) points->curves.pop_back();
		newCurve = true;
		points->branch_points.clear();
		curve = new Curve();
	}

	static BranchPoint* prev = NULL;
	static const float dist_threshold_to_existing = 0.1f;

	// test if controller is close to old branch point
	if (_vr.controller1.trigger_first_press)
	{
		// find the closest branch point
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

		if (found)
		{
			prev = points->branch_points[closest_index];
		}
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
				glm::vec4 controller_pos_in_point_space;
				glm::vec4 tmp;
				if (_vr.hmd_connected && ready)
				{
					glm::vec3 arg1 = _vr.controller1.position + _vr.controller1.ray * 0.25f;
					tmp = glm::vec4(arg1, 1.0f);
					glm::mat4 modelPoints = glm::inverse(points->GetModelMatrix());
					controller_pos_in_point_space = modelPoints * tmp;
				}
				else
				{
					controller_pos_in_point_space = glm::inverse(points->GetModelMatrix())
						* glm::vec4(_vr.controller1.position, 1.0f);
					tmp = points->GetModelMatrix() * glm::inverse(points->GetModelMatrix())
						* glm::vec4(_vr.controller1.position, 1.0f);
				}
				BranchPoint* newBP = new BranchPoint(glm::vec3(controller_pos_in_point_space) - points->lower_bounds);
				points->branch_points.push_back(newBP);
				prev->neighbors.push_back(newBP->id);
				prev = newBP;
			}
		}
		// first point of disconnected branch
		else
		{
			glm::vec4 controller_pos_in_point_space;
			if (_vr.hmd_connected && ready)
			{
				controller_pos_in_point_space = glm::inverse(points->GetModelMatrix())
					* glm::vec4(_vr.controller1.position + _vr.controller1.ray * 0.25f, 1.0f);
			}
			else
			{
				controller_pos_in_point_space = glm::inverse(points->GetModelMatrix())
					* glm::vec4(_vr.controller1.position, 1.0f);
			}

			BranchPoint* newBP = new BranchPoint(glm::vec3(controller_pos_in_point_space) - points->lower_bounds);
			points->branch_points.push_back(newBP);
			prev = newBP;
		}
	}

	if (points->branch_points.size() >= 4)
	{
		std::vector<BranchPoint*> pointsToFit;
		BranchPoint* prevBranchPoint = points->branch_points[0];
		for (int i = 1; i < points->branch_points.size(); i++) //TODO: For each click, duplicate branch points keep getting added, probably a bug in SpoofInput
		{
			if (glm::notEqual(points->branch_points[i]->position, prevBranchPoint->position).b)
			{
				pointsToFit.push_back(points->branch_points[i]);
				prevBranchPoint = points->branch_points[i];
			}
		}

		int currPointsToFitCount = pointsToFit.size();
		if (currPointsToFitCount != pointsToFitCount && (currPointsToFitCount == 4
			|| currPointsToFitCount > 4 && currPointsToFitCount - pointsAlreadyFitCount == 3))
		{
			LeastSquaresFit leastSquaresFit(pointsToFit, 1);
			leastSquaresFit.Fit();
			std::vector<glm::vec3> curvePoints = leastSquaresFit.GetCurvePoints();

			curve->SetPositions(curvePoints);
			curve->SetNormals(curvePoints);
			std::vector<glm::vec3> controlPoints;
			controlPoints.push_back(curvePoints.front());
			controlPoints.push_back(leastSquaresFit.GetFittedControlPoints()[0]);
			controlPoints.push_back(leastSquaresFit.GetFittedControlPoints()[1]);
			controlPoints.push_back(curvePoints.back());
			curve->SetControlPoints(controlPoints);
			if (newCurve || points->curves.empty())
			{
				points->curves.push_back(curve);
			}
			else
			{
				points->curves.back() = curve;
			}
			for (Curve* curveInstance : points->curves)
			{
				curveInstance->RenderCurve();
			}
			pointsToFitCount = currPointsToFitCount;
			pointsAlreadyFitCount = currPointsToFitCount;
			newCurve = false;
		}
	}
}

void DicomObjectsContainer::AddObjects(Render* _r)
{
	_r->AddObjectToScene(points);
	viewer->AddObjects(_r);

	//_r->AddObjectToScene(debug1);
	//_r->AddObjectToScene(debug2);
}

void DicomObjectsContainer::AddIsovaluePointCloudSlider(const int _isovalue)
{
	bool unused_slider_slot_found = false;
	for (int i = 0; i < isovalue_point_cloud_sliders.size(); ++i)
	{
		if (!isovalue_point_cloud_sliders[i]->in_use)
		{
			isovalue_point_cloud_sliders[i]->SetInUse(true);
			isovalue_point_cloud_sliders[i]->SetValue(_isovalue);
			sliderCount++;
			unused_slider_slot_found = true;
			break;
		}
	}
}

// Changed parameters to accomodate for user folder selection
void DicomObjectsContainer::Load(std::string _dicomDir)
{
	std::cout << _dicomDir << std::endl;
	imaging_data = DicomReader::ReadSet(_dicomDir);
	viewer->Load(imaging_data);
	UpdateDicomPointCloud(DEFAULT_ISOVALUE);
}

void DicomObjectsContainer::UpdateDicomPointCloud(int _isovalue)
{
	imaging_data.isovalue = _isovalue;
	points->Generate(imaging_data, _isovalue, MAX_ISOVALUE_TOLERANCE, first, last, isovalue_point_cloud_sliders);
}

void DicomObjectsContainer::UpdateDicomPointCloud(int _isovalue, int _tolerance)
{
	imaging_data.isovalue = _isovalue;
	points->Generate(imaging_data, _isovalue, _tolerance, first, last, isovalue_point_cloud_sliders);
}
