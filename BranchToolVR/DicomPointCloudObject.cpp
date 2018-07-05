#include "DicomPointCloudObject.h"
#include "DicomObjectsContainer.h"

int BranchPoint::id_counter = 0;
std::vector <float> distinct_z;

DicomPointCloudObject::DicomPointCloudObject()
{
	// default values
	is_selectable = true;
	curr_tolerance = DEFAULT_ISOVALUE_TOLERANCE;
	has_generated = false;
	current_bp_selection = -1;
	lower_bounds = glm::vec3(0.5f, 0.5f, 0.5f);
	upper_bounds = lower_bounds + glm::vec3(DEFAULT_SELECTOR_SCALE);

	branch_point_marker = new ColorObject;
	branch_point_marker->SetDisplayColor(glm::vec4(1.0f, 0.1f, 0.2f, 1.0f));
}

DicomPointCloudObject::~DicomPointCloudObject()
{
	delete branch_point_marker;
}

void DicomPointCloudObject::Clear()
{
	num_instances = 0;
}

int DicomPointCloudObject::Type()
{
	return AbstractBaseObject::get_type_id<DicomPointCloudObject>();
}

void DicomPointCloudObject::Load()
{
	num_vertices = positions.size();
	num_instances = instanced_positions.size();
	// Added envelope statement to stop breaking on empty cloud (can happen when adjusting sliders to extreme values)
	if (num_instances != 0) {
		// Orients point cloud according to the CT scan representation and z position of center slice
		float off_z = distinct_z.at(distinct_z.size() / 2);
		float range_z = abs(distinct_z.at(distinct_z.size() - 1) - distinct_z.at(0));
		for (int i = 0; i < num_instances; i++) {
			instanced_positions.at(i).x = -instanced_positions.at(i).x;
			instanced_positions.at(i).x += ((upper_bounds.x - lower_bounds.x) / 2);
			instanced_positions.at(i).y = -instanced_positions.at(i).y;
			instanced_positions.at(i).y += ((upper_bounds.y - lower_bounds.y) / 2);
			instanced_positions.at(i).z -= off_z;
		}
		if (!has_generated)
		{
			glGenVertexArrays(1, &vao);
			glBindVertexArray(vao);

			glGenBuffers(1, &positions_buffer);
			glBindBuffer(GL_ARRAY_BUFFER, positions_buffer);
			glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3)*positions.size(), &positions[0], GL_STATIC_DRAW);

			glGenBuffers(1, &normals_buffer);
			glBindBuffer(GL_ARRAY_BUFFER, normals_buffer);
			glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3)*normals.size(), &normals[0], GL_STATIC_DRAW);

			glGenBuffers(1, &instanced_positions_buffer);
			glGenBuffers(1, &instanced_colors_buffer);
			glGenBuffers(1, &instanced_isovalue_differences_buffer);

			if (instanced_positions.size() > 0)
			{
				glBindBuffer(GL_ARRAY_BUFFER, instanced_positions_buffer);
				glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3)*instanced_positions.size(), &instanced_positions[0], GL_STATIC_DRAW);
			}

			if (instanced_colors.size() > 0)
			{
				glBindBuffer(GL_ARRAY_BUFFER, instanced_colors_buffer);
				glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3)*instanced_colors.size(), &instanced_colors[0], GL_STATIC_DRAW);
			}

			if (instanced_isovalue_differences.size() > 0)
			{
				glBindBuffer(GL_ARRAY_BUFFER, instanced_isovalue_differences_buffer);
				glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*instanced_isovalue_differences.size(), &instanced_isovalue_differences[0], GL_STATIC_DRAW);
			}

			glEnableVertexAttribArray(0);
			glBindBuffer(GL_ARRAY_BUFFER, positions_buffer);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

			glEnableVertexAttribArray(1);
			glBindBuffer(GL_ARRAY_BUFFER, normals_buffer);
			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

			glEnableVertexAttribArray(2);
			glBindBuffer(GL_ARRAY_BUFFER, instanced_positions_buffer);
			glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

			glEnableVertexAttribArray(3);
			glBindBuffer(GL_ARRAY_BUFFER, instanced_colors_buffer);
			glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

			glEnableVertexAttribArray(4);
			glBindBuffer(GL_ARRAY_BUFFER, instanced_isovalue_differences_buffer);
			glVertexAttribPointer(4, 1, GL_FLOAT, GL_FALSE, 0, (void*)0);

			glBindVertexArray(0);
			glDisableVertexAttribArray(0);
			glDisableVertexAttribArray(1);
			glDisableVertexAttribArray(2);
			glDisableVertexAttribArray(3);
			glDisableVertexAttribArray(4);
		}
		else
		{
			if (instanced_positions.size() > 0)
			{
				glBindBuffer(GL_ARRAY_BUFFER, instanced_positions_buffer);
				glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3)*instanced_positions.size(), &instanced_positions[0], GL_STATIC_DRAW);
			}

			if (instanced_colors.size() > 0)
			{
				glBindBuffer(GL_ARRAY_BUFFER, instanced_colors_buffer);
				glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3)*instanced_colors.size(), &instanced_colors[0], GL_STATIC_DRAW);
			}

			if (instanced_isovalue_differences.size() > 0)
			{
				glBindBuffer(GL_ARRAY_BUFFER, instanced_isovalue_differences_buffer);
				glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*instanced_isovalue_differences.size(), &instanced_isovalue_differences[0], GL_STATIC_DRAW);
			}
		}
	}
}

std::vector<glm::vec3> DicomPointCloudObject::GetInstancedColor()
{
	return instanced_colors;
}

std::vector<glm::vec3> DicomPointCloudObject::GetInstancedPositions()
{
	return instanced_positions;
}

void DicomPointCloudObject::GenerateCube(glm::vec3 _scale, glm::vec3 _offset)
{
	glm::vec3 cube[8];
	cube[0] = glm::vec3(0.0f, 0.0f, 0.0f);
	cube[1] = glm::vec3(1.0f, 0.0f, 0.0f);
	cube[2] = glm::vec3(0.0f, 0.0f, -1.0f);
	cube[3] = glm::vec3(1.0f, 0.0f, -1.0f);
	cube[4] = glm::vec3(0.0f, 1.0f, 0.0f);
	cube[5] = glm::vec3(1.0f, 1.0f, 0.0f);
	cube[6] = glm::vec3(0.0f, 1.0f, -1.0f);
	cube[7] = glm::vec3(1.0f, 1.0f, -1.0f);

	// front
	positions.push_back(cube[0]);
	positions.push_back(cube[1]);
	positions.push_back(cube[5]);
	positions.push_back(cube[0]);
	positions.push_back(cube[5]);
	positions.push_back(cube[4]);

	// back
	positions.push_back(cube[3]);
	positions.push_back(cube[2]);
	positions.push_back(cube[6]);
	positions.push_back(cube[3]);
	positions.push_back(cube[6]);
	positions.push_back(cube[7]);

	// left side
	positions.push_back(cube[2]);
	positions.push_back(cube[0]);
	positions.push_back(cube[4]);
	positions.push_back(cube[2]);
	positions.push_back(cube[4]);
	positions.push_back(cube[6]);

	// right side
	positions.push_back(cube[1]);
	positions.push_back(cube[3]);
	positions.push_back(cube[7]);
	positions.push_back(cube[1]);
	positions.push_back(cube[7]);
	positions.push_back(cube[5]);

	// top
	positions.push_back(cube[4]);
	positions.push_back(cube[5]);
	positions.push_back(cube[7]);
	positions.push_back(cube[4]);
	positions.push_back(cube[7]);
	positions.push_back(cube[6]);

	// bottom
	positions.push_back(cube[1]);
	positions.push_back(cube[0]);
	positions.push_back(cube[2]);
	positions.push_back(cube[1]);
	positions.push_back(cube[2]);
	positions.push_back(cube[3]);

	normals.push_back(glm::vec3(0.0f, 0.0f, 1.0f));
	normals.push_back(glm::vec3(0.0f, 0.0f, 1.0f));
	normals.push_back(glm::vec3(0.0f, 0.0f, 1.0f));
	normals.push_back(glm::vec3(0.0f, 0.0f, 1.0f));
	normals.push_back(glm::vec3(0.0f, 0.0f, 1.0f));
	normals.push_back(glm::vec3(0.0f, 0.0f, 1.0f));

	normals.push_back(glm::vec3(0.0f, 0.0f, -1.0f));
	normals.push_back(glm::vec3(0.0f, 0.0f, -1.0f));
	normals.push_back(glm::vec3(0.0f, 0.0f, -1.0f));
	normals.push_back(glm::vec3(0.0f, 0.0f, -1.0f));
	normals.push_back(glm::vec3(0.0f, 0.0f, -1.0f));
	normals.push_back(glm::vec3(0.0f, 0.0f, -1.0f));

	normals.push_back(glm::vec3(-1.0f, 0.0f, 0.0f));
	normals.push_back(glm::vec3(-1.0f, 0.0f, 0.0f));
	normals.push_back(glm::vec3(-1.0f, 0.0f, 0.0f));
	normals.push_back(glm::vec3(-1.0f, 0.0f, 0.0f));
	normals.push_back(glm::vec3(-1.0f, 0.0f, 0.0f));
	normals.push_back(glm::vec3(-1.0f, 0.0f, 0.0f));

	normals.push_back(glm::vec3(1.0f, 0.0f, 0.0f));
	normals.push_back(glm::vec3(1.0f, 0.0f, 0.0f));
	normals.push_back(glm::vec3(1.0f, 0.0f, 0.0f));
	normals.push_back(glm::vec3(1.0f, 0.0f, 0.0f));
	normals.push_back(glm::vec3(1.0f, 0.0f, 0.0f));
	normals.push_back(glm::vec3(1.0f, 0.0f, 0.0f));

	normals.push_back(glm::vec3(0.0f, 1.0f, 0.0f));
	normals.push_back(glm::vec3(0.0f, 1.0f, 0.0f));
	normals.push_back(glm::vec3(0.0f, 1.0f, 0.0f));
	normals.push_back(glm::vec3(0.0f, 1.0f, 0.0f));
	normals.push_back(glm::vec3(0.0f, 1.0f, 0.0f));
	normals.push_back(glm::vec3(0.0f, 1.0f, 0.0f));

	normals.push_back(glm::vec3(0.0f, -1.0f, 0.0f));
	normals.push_back(glm::vec3(0.0f, -1.0f, 0.0f));
	normals.push_back(glm::vec3(0.0f, -1.0f, 0.0f));
	normals.push_back(glm::vec3(0.0f, -1.0f, 0.0f));
	normals.push_back(glm::vec3(0.0f, -1.0f, 0.0f));
	normals.push_back(glm::vec3(0.0f, -1.0f, 0.0f));

	for (unsigned int i = 0; i < positions.size(); ++i)
	{
		positions[i].z += 1.0f;
		positions[i] += _offset;
		positions[i] *= _scale;
	}
}

void DicomPointCloudObject::GenerateSphere(float _scale)
{
	int resolution = 5;
	int num_vertices = resolution * resolution;
	float t_inc1 = 3.145 / (float)(resolution - 1);
	float t_inc2 = 6.28 / (float)(resolution);
	glm::vec3* vertices = new glm::vec3[num_vertices];

	for (int i = 0; i < num_vertices; ++i)
	{
		float layer = i / resolution;
		float curr_radius = sin(t_inc1*layer);
		float t = t_inc2 * (float)((resolution - 1) - (i % resolution));
		vertices[i] = _scale * glm::vec3(curr_radius*cos(t), -1.0f*cos(t_inc1*layer), curr_radius*sin(t));
	}

	int num_patches = resolution * (resolution - 1);
	for (int i = 0; i < num_patches; ++i)
	{
		int x = i % resolution;
		int y = i / resolution;

		if (y != 0)
		{
			positions.push_back(vertices[i]);
			positions.push_back(vertices[(x + 1) % resolution + y * resolution]);
			positions.push_back(vertices[(x + 1) % resolution + (y + 1)*resolution]);

			normals.push_back(vertices[i]);
			normals.push_back(vertices[(x + 1) % resolution + y * resolution]);
			normals.push_back(vertices[(x + 1) % resolution + (y + 1)*resolution]);
		}

		if (y != resolution - 1)
		{
			positions.push_back(vertices[i]);
			positions.push_back(vertices[(x + 1) % resolution + (y + 1)*resolution]);
			positions.push_back(vertices[x % resolution + (y + 1)*resolution]);

			normals.push_back(vertices[i]);
			normals.push_back(vertices[(x + 1) % resolution + (y + 1)*resolution]);
			normals.push_back(vertices[x % resolution + (y + 1)*resolution]);
		}
	}

	Load();
}

float distance(glm::vec3 a, glm::vec3 b){
	float d = sqrt(pow(a.x - b.x, 2) + pow(a.y - b.y, 2) + pow(a.z - b.z, 2));
	//std::cout << d << std::endl;
	return d;
}

// Currently regenerates entire cloud for all active iso-sliders rather than just the specific one that's value was changed
void DicomPointCloudObject::Generate(DicomSet & _ds, int _isovalue, int max_tolerance, int first, int last, std::vector<IsovaluePointCloudSlider*>& isovalue_point_cloud_sliders)
{
	if (!has_changed) {
		return;
	}

	has_changed = false;

	if (&_ds == NULL || _ds.data.size() < 1)
	{
		return;
	}

	//sets diagonal 3d world space
	float box_diag_len = glm::length(upper_bounds - lower_bounds);
	SetScale(SQRT3 / box_diag_len);

	int f = 0;

	// Voxel z scale for base set: 0.00246914
	// This changes if loading sets with different distance between slices, causing voxels to elongate
	voxel_scale = glm::vec3(_ds.scale.x / (float)(_ds.data[0].width + f), _ds.scale.y / (float)(_ds.data[0].height + f), /*_ds.scale.z / ((float)_ds.data.size() + f)*/ 0.00246914);
	//voxel_scale *= (1 / (0.00246914*(last - first)));
	branch_point_marker->GenerateSphere(10, voxel_scale.x * 0.5f, false);

	if (!has_generated)
	{
		// Adjust offset values so Cloud is within Selector object
		GenerateCube(voxel_scale, glm::vec3(0.0f));
	}
	else
	{
		// resetti spaghetti
		instanced_positions.clear();
		instanced_isovalue_differences.clear();
		instanced_colors.clear();
		distinct_z.clear();
	}

	/*------TODO:-----*/
	// Look into cause for changing values when decimate is toggled after importing DICOM set 
	// - Apparently alters/resets first and last values; maybe isovalues
	// - Workaround is to shift variables in interface until cloud comes back
	// Figure out cause for generation of horizontal strips of points when decimated (may not be a problem per se)

	std::cout << "last " << last << std::endl;
	glm::vec3 col(1.0f);

	// Moved this stuff around for ply output correction and speed
	for (int k = 0; k < isovalue_point_cloud_sliders.size(); k++)
	{
		isovalue_point_cloud_sliders[k]->point_size = 0;
		for (int i = first; i <= last; i++)
		{
			for (int j = 0; j < _ds.data[i].isovalues.size(); ++j)
			{
				int slider_count = 0;
				if (isovalue_point_cloud_sliders[k]->in_use) {
					glm::vec3 instanced_position = glm::vec3((float)(j % _ds.data[0].width), float(j / _ds.data[0].width), (float)i) * voxel_scale;

					//test if in magnification area
					if (instanced_position.x > lower_bounds.x && instanced_position.y > lower_bounds.y
						&& instanced_position.x < upper_bounds.x && instanced_position.y < upper_bounds.y)
					{
						if (_ds.data[i].isovalues[j] <= 3000) {
							if (slider_count < k) slider_count = k;
							col = isovalue_point_cloud_sliders[slider_count]->color;
							short iso_abs_check = abs(_ds.data[i].isovalues[j] - (short)isovalue_point_cloud_sliders[k]->curr_isovalue);
							int tolerance = isovalue_point_cloud_sliders[k]->iso_tolerance;

							if (isovalue_point_cloud_sliders[slider_count]->dec) {
								int num_neighbors = 0;
								int slice = (i > 0) ? (i - 1) : 0;
								int slice_bound = (i < last) ? (i + 1) : last;
								glm::vec3 neighbor;
								//std::cout << ((j % _ds.data[0].width) * voxel_scale.x) << "\t" << ((j / _ds.data[0].width) * voxel_scale.y) << std::endl;
								int val = (j > 0) ? (j - tolerance) : 0;
								int val_bound = (j < (_ds.data[i].isovalues.size() - (tolerance + 1))) ? (j + tolerance) : (_ds.data[i].isovalues.size() - 1);
								for (slice; slice <= slice_bound; slice++) {
									for (val; val <= val_bound; val++) {
										//for (float v1 = ((j % _ds.data[0].width) - 1) * voxel_scale.x; v1 <= (float(j % _ds.data[0].width) + 1) * voxel_scale.x; v1 += voxel_scale.x) {
											//for (float v2 = ((j / _ds.data[0].width) - 1) * voxel_scale.y; v2 <= (float(j / _ds.data[0].width) + 1) * voxel_scale.y; v2 += voxel_scale.y) {
												neighbor = glm::vec3(instanced_position.x, instanced_position.y, (float)slice * voxel_scale.z);
												//std::cout << distance(instanced_position, neighbor) << std::endl;
												//if (distance(instanced_position, neighbor) - (2 * voxel_scale.z) <= 0.0f) num_neighbors++;
												iso_abs_check = abs(_ds.data[slice].isovalues[val] - (short)isovalue_point_cloud_sliders[k]->curr_isovalue);
												if (iso_abs_check <= tolerance && distance(instanced_position, neighbor) - (2 * voxel_scale.z) < 0.0f) num_neighbors++;
											//}
										//}
									}
								}
								//std::cout << num_neighbors << std::endl;
								if (num_neighbors > 5) {
									isovalue_point_cloud_sliders[slider_count]->point_size++;

									// Tracking distinct z-values for the sake of recentering the cloud later; only need to do so for first slider
									if (instanced_positions.size() == 0 && k == 0)
										distinct_z.push_back((instanced_position - lower_bounds).z);
									else if ((instanced_position - lower_bounds).z - instanced_positions.back().z > 0.000001 && k == 0)
										distinct_z.push_back((instanced_position - lower_bounds).z);

									instanced_positions.push_back(instanced_position - lower_bounds);
									instanced_isovalue_differences.push_back(iso_abs_check);
									instanced_colors.push_back(col);
								}
							}
							else if (iso_abs_check <= tolerance)
							{
								isovalue_point_cloud_sliders[slider_count]->point_size++;

								// Tracking distinct z-values for the sake of recentering the cloud later; only need to do so for first slider
								if (instanced_positions.size() == 0 && k == 0)
									distinct_z.push_back((instanced_position - lower_bounds).z);
								else if ((instanced_position - lower_bounds).z - instanced_positions.back().z > 0.000001 && k == 0)
									distinct_z.push_back((instanced_position - lower_bounds).z);

								instanced_positions.push_back(instanced_position - lower_bounds);
								instanced_isovalue_differences.push_back(iso_abs_check);
								instanced_colors.push_back(col);
							}
						}
					}
				}
			}
		}
	}

	Load();
	has_generated = true;
}
void DicomPointCloudObject::SetMasterAppendPose(glm::mat4 _in)
{
	SetAppendPose(_in);
	branch_point_marker->SetAppendPose(_in);

}

BranchPoint* DicomPointCloudObject::GetBranchPointByID(int _id)  // TODO: switch to id to pointer map
{
	for (int i = 0; i < branch_points.size(); ++i)
	{
		if (branch_points[i]->id == _id)
		{
			return branch_points[i];
		}
	}

	return NULL;
}

void DicomPointCloudObject::MarkForRegeneration()
{
	has_changed = true;
}