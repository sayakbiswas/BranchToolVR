#include "ColorObject.h"

ColorObject::ColorObject()
{
	display_color = glm::vec4(0.2f, 0.8f, 0.2f,1.0f);
	selected_color_additive = glm::vec4(1.0f);
	display_color_modifier = 0.0f;
	selection_modifier = 0.5f;
}

ColorObject::~ColorObject()
{
}

glm::vec4 ColorObject::GetDisplayColor() 
{
	return display_color + display_color_modifier*selected_color_additive;
}

void ColorObject::SetDisplayColor(const glm::vec4 & _inColor) 
{
	display_color = _inColor;
}

void ColorObject::SetSelected(bool _isSelected) 
{
	if (_isSelected)
	{
		is_selected = true;
		display_color_modifier = selection_modifier;
		return;
	}

	is_selected = false;
	display_color_modifier = 0.0f;
}

void ColorObject::Finalize() 
{
	SetBoundingSphere();
	num_vertices = positions.size();
	Load();
}

int ColorObject::Type() 
{
	return AbstractBaseObject::get_type_id<ColorObject>();
}

void ColorObject::Load() 
{
	is_loaded = true;
	
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glGenBuffers(1, &positions_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, positions_buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3)*positions.size(), &positions[0], GL_STATIC_DRAW);

	glGenBuffers(1, &normals_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, normals_buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3)*normals.size(), &normals[0], GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, positions_buffer);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, normals_buffer);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

	glBindVertexArray(0);
	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);

}

void ColorObject::GenerateGround() 
{
	glm::vec3 scale(10.0f, 0.0f, 10.0f);
	glm::vec3 _offset = glm::vec3(-5.0f, -0.1f, -5.0f);
	
	positions.push_back(glm::vec3(1.0f, 0.0f, 0.0f));
	positions.push_back(glm::vec3(0.0f, 0.0f, 0.0f));
	positions.push_back(glm::vec3(0.0f, 0.0f, 1.0f));
	
	positions.push_back(glm::vec3(1.0f, 0.0f, 0.0f));
	positions.push_back(glm::vec3(0.0f, 0.0f, 1.0f));
	positions.push_back(glm::vec3(1.0f, 0.0f, 1.0f));
	
	normals.push_back(glm::vec3(0.0f, 1.0f, 0.0f));
	normals.push_back(glm::vec3(0.0f, 1.0f, 0.0f));
	normals.push_back(glm::vec3(0.0f, 1.0f, 0.0f));
	
	normals.push_back(glm::vec3(0.0f, 1.0f, 0.0f));
	normals.push_back(glm::vec3(0.0f, 1.0f, 0.0f));
	normals.push_back(glm::vec3(0.0f, 1.0f, 0.0f));
	
	for (unsigned int i = 0; i < positions.size(); ++i) 
	{
		positions[i] *= scale;
		positions[i] += _offset;
	}

	display_color = glm::vec4(0.2f, 0.2f, 0.3f,1.0f);
	Finalize();
}

void ColorObject::GenerateController() 
{
	float len = 0.05f;
	float pointer_width = 0.005f;

	// Changes to pointer length are not always displayed properly for some reason; behaving on previous values
	AddRectangularPrism(glm::vec3(len, len, len*2.5f), glm::vec3(-len/2.0f, -len/2.0f, 0.0f));
	AddRectangularPrism(glm::vec3(pointer_width, pointer_width, -0.25f), glm::vec3(-pointer_width / 2.0f, -pointer_width / 2.0f, 0.0f));
	AddRectangularPrism(glm::vec3(pointer_width / 4.0f, pointer_width / 4.0f, -10.0f), glm::vec3(-pointer_width / 2.0f, -pointer_width / 2.0f, 0.0f));
	
	Finalize();
}

void ColorObject::AddRectangularPrism(glm::vec3 _scale, glm::vec3 _offset) 
{
	// Idea: clear out positions rather than skipping through it?
	int prev_num_positions = positions.size();

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

	for (unsigned int i = prev_num_positions; i < positions.size(); ++i) 
	{
		positions[i].z += 1.0f;
		positions[i] *= _scale;
		positions[i] += _offset;
	}
}

void ColorObject::GenerateXYPrism(float _scaleX, float _scaleY, float _scaleZ, glm::vec2 _padding, glm::vec3 _offset) 
{
	AddRectangularPrism(glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(0.0f, 0.0f, 0.0f));	

	for (unsigned int i = 0; i < positions.size(); ++i) 
	{
		positions[i].x *= _scaleX - _padding.x*2.0f;
		positions[i].y *= _scaleY - _padding.y*2.0f;
		positions[i].z *= _scaleZ;
		positions[i] += glm::vec3(_padding.x, _padding.y, 0.0f);
		positions[i] += _offset;
	}

	Finalize();
}

void ColorObject::GenerateXYPrism(glm::vec3 _scale, glm::vec2 _padding, glm::vec3 _offset) 
{
	AddRectangularPrism(glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(0.0f, 0.0f, 0.0f));

	for (unsigned int i = 0; i < positions.size(); ++i) 
	{
		positions[i].x *= _scale.x - _padding.x*2.0f;
		positions[i].y *= _scale.y - _padding.y*2.0f;
		positions[i].z *= _scale.z;
		positions[i] += glm::vec3(_padding.x, _padding.y, 0.0f);
		positions[i] += _offset;
	}

	Finalize();
}

void ColorObject::GenerateXYPlane(float _scaleX, float _scaleY, float _padding, glm::vec3 _offset) 
{
	GenerateXYPlane(_scaleX,  _scaleY, glm::vec2(_padding,_padding), _offset);
}

void ColorObject::GenerateXZPlane(float _scaleX, float _scaleZ, float _padding, glm::vec3 _offset)
{
	positions.push_back(glm::vec3(_scaleX*0.0f, 0.0f, _scaleZ* 0.0f));
	positions.push_back(glm::vec3(_scaleX*1.0f, 0.0f, _scaleZ* 0.0f));
	positions.push_back(glm::vec3(_scaleX*1.0f, 0.0f, _scaleZ* -1.0f));
	positions.push_back(glm::vec3(_scaleX*0.0f, 0.0f, _scaleZ* 0.0f));
	positions.push_back(glm::vec3(_scaleX*1.0f, 0.0f, _scaleZ* -1.0f));
	positions.push_back(glm::vec3(_scaleX*0.0f, 0.0f, _scaleZ* -1.0f));

	normals.push_back(glm::vec3(0.0f, 1.0f, 0.0f));
	normals.push_back(glm::vec3(0.0f, 1.0f, 0.0f));
	normals.push_back(glm::vec3(0.0f, 1.0f, 0.0f));
	normals.push_back(glm::vec3(0.0f, 1.0f, 0.0f));
	normals.push_back(glm::vec3(0.0f, 1.0f, 0.0f));
	normals.push_back(glm::vec3(0.0f, 1.0f, 0.0f));

	Finalize();
}

void ColorObject::GenerateXYPlane(float _scaleX, float _scaleY, glm::vec2 _padding, glm::vec3 _offset)
{
	positions.push_back(glm::vec3(0.0f, 0.0f, 0.0f));
	positions.push_back(glm::vec3(1.0f, 0.0f, 0.0f));
	positions.push_back(glm::vec3(1.0f, 1.0f, 0.0f));

	positions.push_back(glm::vec3(0.0f, 0.0f, 0.0f));
	positions.push_back(glm::vec3(1.0f, 1.0f, 0.0f));
	positions.push_back(glm::vec3(0.0f, 1.0f, 0.0f));

	normals.push_back(glm::vec3(0.0f, 0.0f, 1.0f));
	normals.push_back(glm::vec3(0.0f, 0.0f, 1.0f));
	normals.push_back(glm::vec3(0.0f, 0.0f, 1.0f));

	normals.push_back(glm::vec3(0.0f, 0.0f, 1.0f));
	normals.push_back(glm::vec3(0.0f, 0.0f, 1.0f));
	normals.push_back(glm::vec3(0.0f, 0.0f, 1.0f));

	for (unsigned int i = 0; i < positions.size(); ++i) 
	{
		positions[i].x *= _scaleX - _padding.x*2.0f;
		positions[i].y *= _scaleY - _padding.y*2.0f;
		positions[i] += glm::vec3(_padding.x, _padding.y, 0.0f);
		positions[i] += _offset;
	}

	Finalize();
}

// variables declared in MarchingCubesVars.cpp 
extern int edgeTable[];
extern int triTable[][16];

glm::vec3 VertexInterp(double isolevel, glm::vec3 p1, glm::vec3 p2, double valp1, double valp2)
{
	glm::vec3 P1toP2 = p2 - p1;
	double isoDiffP1 = (isolevel - valp1) / abs(valp1 - valp2);
	return (float)isoDiffP1 * P1toP2 + p1;
}

void ColorObject::GenerateIsosurfaceFromDicomSet(DicomSet & _dSet, int _isolevel) 
{
	if (_dSet.data.size() < 1) 
	{
		return;
	}

	positions.clear();
	normals.clear();

	int num_gridX = _dSet.data[0].width;
	int num_gridY = _dSet.data[0].height;
	int num_gridXY = _dSet.data[0].width * _dSet.data[0].height;
	int num_gridXYZ = _dSet.data[0].width * _dSet.data[0].height * _dSet.data.size();

	int num_cellsX = _dSet.data[0].width - 1;
	int num_cellsY = _dSet.data[0].height - 1;
	int num_cellsZ = _dSet.data.size();
	int num_cellsXY = (_dSet.data[0].width - 1) * (_dSet.data[0].height - 1);
	int num_cellsXYZ = num_cellsXY *  (_dSet.data.size() - 1);

	for (int i = 0; i < num_cellsXYZ; i+=1) 
	{
		int cell_layer_lower = i / num_cellsXY;
		int cell_layer_upper = cell_layer_lower + 1;
		int cell_x = i % num_cellsX;
		int cell_y = i % num_cellsXY / num_cellsX ;

		int grid_indices[4];
		float grid_values[8];
		grid_indices[0] = cell_y * num_gridX + cell_x;
		grid_indices[1] = grid_indices[0] + 1;
		grid_indices[2] = grid_indices[0] + num_gridX;
		grid_indices[3] = grid_indices[2] + 1;

		grid_values[0] = _dSet.data[cell_layer_lower].isovalues[grid_indices[0]];
		grid_values[1] = _dSet.data[cell_layer_lower].isovalues[grid_indices[1]];
		grid_values[2] = _dSet.data[cell_layer_lower].isovalues[grid_indices[2]];
		grid_values[3] = _dSet.data[cell_layer_lower].isovalues[grid_indices[3]];
		grid_values[4] = _dSet.data[cell_layer_upper].isovalues[grid_indices[0]];
		grid_values[5] = _dSet.data[cell_layer_upper].isovalues[grid_indices[1]];
		grid_values[6] = _dSet.data[cell_layer_upper].isovalues[grid_indices[2]];
		grid_values[7] = _dSet.data[cell_layer_upper].isovalues[grid_indices[3]];
		
		glm::vec3 vertlist[12];
		glm::vec3 cell_points[8];

		cell_points[0] = glm::vec3((float)cell_x / (float)num_gridX, (float)cell_y / (float)num_cellsY, (float)cell_layer_lower / (float)num_cellsZ);
		cell_points[1] = glm::vec3((float)(cell_x+1) / (float)num_gridX, (float)cell_y / (float)num_cellsY, (float)cell_layer_lower / (float)num_cellsZ);
		cell_points[2] = glm::vec3((float)cell_x / (float)num_gridX, (float)(cell_y+1) / (float)num_cellsY, (float)cell_layer_lower / (float)num_cellsZ);
		cell_points[3] = glm::vec3((float)(cell_x + 1) / (float)num_gridX, (float)(cell_y + 1) / (float)num_cellsY, (float)cell_layer_lower / (float)num_cellsZ);
		cell_points[4] = glm::vec3(cell_points[0].x,cell_points[0].y, (float)cell_layer_upper / (float)num_cellsZ);
		cell_points[5] = glm::vec3(cell_points[1].x,cell_points[1].y, (float)cell_layer_upper / (float)num_cellsZ);
		cell_points[6] = glm::vec3(cell_points[2].x,cell_points[2].y, (float)cell_layer_upper / (float)num_cellsZ);
		cell_points[7] = glm::vec3(cell_points[3].x,cell_points[3].y, (float)cell_layer_upper / (float)num_cellsZ);

		int cubeindex = 0;
		if (grid_values[0] < _isolevel) cubeindex |= 1;
		if (grid_values[1] < _isolevel) cubeindex |= 2;
		if (grid_values[2] < _isolevel) cubeindex |= 4;
		if (grid_values[3] < _isolevel) cubeindex |= 8;
		if (grid_values[4] < _isolevel) cubeindex |= 16;
		if (grid_values[5] < _isolevel) cubeindex |= 32;
		if (grid_values[6] < _isolevel) cubeindex |= 64;
		if (grid_values[7] < _isolevel) cubeindex |= 128;


		glm::vec3 normals1[8];
		normals1[0] = glm::vec3(-1.0f, -1.0f, -1.0f);
		normals1[1] = glm::vec3( 1.0f, -1.0f, -1.0f);
		normals1[2] = glm::vec3( 1.0f,  1.0f, -1.0f);
		normals1[3] = glm::vec3(-1.0f,  1.0f, -1.0f);
		normals1[4] = glm::vec3(-1.0f, -1.0f,  1.0f);
		normals1[5] = glm::vec3( 1.0f, -1.0f,  1.0f);
		normals1[6] = glm::vec3( 1.0f,  1.0f,  1.0f);
		normals1[7] = glm::vec3(-1.0f,  1.0f,  1.0f);

		glm::vec3 norm_list[12];

		/* Find the vertices where the surface intersects the cube */
		if (edgeTable[cubeindex] & 1) 
		{
			vertlist[0] = VertexInterp(_isolevel, cell_points[0], cell_points[1], grid_values[0], grid_values[1]);
			norm_list[0] = VertexInterp(_isolevel, normals1[0], normals1[1], grid_values[0], grid_values[1]);
		}
		if (edgeTable[cubeindex] & 2)
		{
			vertlist[1] = VertexInterp(_isolevel, cell_points[1], cell_points[2], grid_values[1], grid_values[2]);
			norm_list[1] = VertexInterp(_isolevel, normals1[1], normals1[2], grid_values[0], grid_values[1]);
		}
		if (edgeTable[cubeindex] & 4)
		{
			vertlist[2] = VertexInterp(_isolevel, cell_points[2], cell_points[3], grid_values[2], grid_values[3]);
			norm_list[2] = VertexInterp(_isolevel, normals1[2], normals1[3], grid_values[0], grid_values[1]);
		}	
		if (edgeTable[cubeindex] & 8)
		{
			vertlist[3] = VertexInterp(_isolevel, cell_points[3], cell_points[0], grid_values[3], grid_values[0]);
			norm_list[3] = VertexInterp(_isolevel, normals1[3], normals1[0], grid_values[0], grid_values[1]);
		}
		if (edgeTable[cubeindex] & 16)
		{
			vertlist[4] = VertexInterp(_isolevel, cell_points[4], cell_points[5], grid_values[4], grid_values[5]);
			norm_list[4] = VertexInterp(_isolevel, normals1[4], normals1[5], grid_values[0], grid_values[1]);
		}
		if (edgeTable[cubeindex] & 32)
		{
			vertlist[5] = VertexInterp(_isolevel, cell_points[5], cell_points[6], grid_values[5], grid_values[6]);
			norm_list[5] = VertexInterp(_isolevel, normals1[5], normals1[6], grid_values[0], grid_values[1]);
		}
		if (edgeTable[cubeindex] & 64)
		{
			vertlist[6] = VertexInterp(_isolevel, cell_points[6], cell_points[7], grid_values[6], grid_values[7]);
			norm_list[6] = VertexInterp(_isolevel, normals1[6], normals1[7], grid_values[0], grid_values[1]);
		}
		if (edgeTable[cubeindex] & 128)
		{
			vertlist[7] = VertexInterp(_isolevel, cell_points[7], cell_points[4], grid_values[7], grid_values[4]);
			norm_list[7] = VertexInterp(_isolevel, normals1[7], normals1[4], grid_values[0], grid_values[1]);
		}
		if (edgeTable[cubeindex] & 256)
		{
			vertlist[8] = VertexInterp(_isolevel, cell_points[0], cell_points[4], grid_values[0], grid_values[4]);
			norm_list[8] = VertexInterp(_isolevel, normals1[0], normals1[4], grid_values[0], grid_values[1]);
		}
		if (edgeTable[cubeindex] & 512)
		{
			vertlist[9] = VertexInterp(_isolevel, cell_points[1], cell_points[5], grid_values[1], grid_values[5]);
			norm_list[9] = VertexInterp(_isolevel, normals1[1], normals1[5], grid_values[0], grid_values[1]);
		}
		if (edgeTable[cubeindex] & 1024)
		{
			vertlist[10] = VertexInterp(_isolevel, cell_points[2], cell_points[6], grid_values[2], grid_values[6]);
			norm_list[10] = VertexInterp(_isolevel, normals1[2], normals1[6], grid_values[0], grid_values[1]);
		}
		if (edgeTable[cubeindex] & 2048)
		{
			vertlist[11] = VertexInterp(_isolevel, cell_points[3], cell_points[7], grid_values[3], grid_values[7]);
			norm_list[11] = VertexInterp(_isolevel, normals1[3], normals1[7], grid_values[0], grid_values[1]);
		}

		for (int j = 0; triTable[cubeindex][j] != -1; j += 3) 
		{
			positions.push_back(_dSet.scale*vertlist[triTable[cubeindex][j]]);
			positions.push_back(_dSet.scale*vertlist[triTable[cubeindex][j + 1]]);
			positions.push_back(_dSet.scale*vertlist[triTable[cubeindex][j + 2]]);

			glm::vec3 p1 = vertlist[triTable[cubeindex][j]];
			glm::vec3 p2 = vertlist[triTable[cubeindex][j + 1]];
			glm::vec3 p3 = vertlist[triTable[cubeindex][j + 2]];
			
			glm::vec3 n = glm::normalize(glm::cross(p2 - p1, p3 - p1));
			
			normals.push_back(n);
			normals.push_back(n);
			normals.push_back(n);
		}
	}

	model_position = glm::vec3(0.0f, 0.0f, 0.0f);
	world_position = glm::vec3(0.0f, 0.0f, 0.0f);

	Finalize();

}

void ColorObject::GenerateSphere(int _res, float _radius, bool _invNormals) {

	positions.clear();
	normals.clear();

	int resolution = _res;
	int num_vertices = resolution * resolution;
	float t_inc1 = 3.145 / (float)(resolution-1);
	float t_inc2 = 6.28 / (float)(resolution);
	glm::vec3* vertices = new glm::vec3[num_vertices];

	for (int i = 0; i < num_vertices; ++i)
	{
		float layer = i / resolution;
		float curr_radius = sin(t_inc1*layer);
		float t = t_inc2*(float)((resolution-1)- (i % resolution));
		vertices[i] = _radius*glm::vec3(curr_radius*cos(t), -1.0f*cos(t_inc1*layer), curr_radius*sin(t));
	}

	int num_patches = resolution * (resolution-1);
	float normal_fac = (_invNormals) ? -1.0f : 1.0f;
	for (int i = 0; i < num_patches; ++i) {

		int x = i % resolution;
		int y = i / resolution;

		if (y != 0) {
			positions.push_back(vertices[i]);
			positions.push_back(vertices[(x + 1) % resolution + y*resolution]);
			positions.push_back(vertices[(x + 1) % resolution + (y + 1)*resolution]);

			normals.push_back(normal_fac*vertices[i]);
			normals.push_back(normal_fac*vertices[(x + 1) % resolution + y*resolution]);
			normals.push_back(normal_fac*vertices[(x + 1) % resolution + (y + 1)*resolution]);
		}

		if (y != resolution - 1) {
			positions.push_back(vertices[i]);
			positions.push_back(vertices[(x + 1) % resolution + (y + 1)*resolution]);
			positions.push_back(vertices[x % resolution + (y + 1)*resolution]);

			normals.push_back(normal_fac*vertices[i]);
			normals.push_back(normal_fac*vertices[(x + 1) % resolution + (y + 1)*resolution]);
			normals.push_back(normal_fac*vertices[x % resolution + (y + 1)*resolution]);
		}

	}

	Finalize();
}