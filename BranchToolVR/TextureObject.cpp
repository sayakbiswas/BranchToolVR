#include "TextureObject.h"

TextureObject::TextureObject()
{
	texture_level = 0;
}

TextureObject::~TextureObject()
{
}

void TextureObject::Finalize() 
{
	SetBoundingSphere();
	is_loaded = true;
	num_vertices = positions.size();
	Load();
}

void TextureObject::SetSelectorScale(std::vector<glm::vec3> _positions) {
	float scale_factor_x = abs(_positions.at(0).x / positions.at(0).x);
	float scale_factor_y = abs(_positions.at(0).y / positions.at(0).y);
	float scale_factor_z = abs(_positions.at(0).z / positions.at(0).z);
	for (int i = 0; i < positions.size(); i++) {
		positions.at(i).x *= scale_factor_x;
		positions.at(i).y *= scale_factor_y;
		positions.at(i).z *= scale_factor_z;
	}
	Finalize();
}

void TextureObject::Load() 
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

	glGenBuffers(1, &uvs_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, uvs_buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2)*uvs.size(), &uvs[0], GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, positions_buffer);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, normals_buffer);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

	glEnableVertexAttribArray(2);
	glBindBuffer(GL_ARRAY_BUFFER, uvs_buffer);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);

	glBindVertexArray(0);
	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);
}

void TextureObject::GenerateText(std::string _text, glm::vec2 _scale, glm::vec2 _padding, glm::vec3 _offset) 
{
	if (is_loaded) 
	{
		positions.clear();
		normals.clear();
		uvs.clear();

		//TODO: delete opengl buffers
	}

	texture_level = 1;
	
	float x_inc = 1.0f / (float)_text.length();

	for (int i = 0; i < _text.length(); ++i) 
	{
		AddLetter(x_inc, 1.0f, glm::vec2(0.0f), glm::vec2(0.0f), glm::vec3(x_inc * (float)i, 0.0f, 0.0f), _text[i]);
	}

	for (unsigned int i = 0; i < positions.size(); ++i) 
	{
		positions[i].x *= _scale.x - _padding.x*2.0f;
		positions[i].y *= _scale.y - _padding.y*2.0f;
		positions[i] += glm::vec3(_padding.x, _padding.y, 0.0f);
		positions[i] += _offset;
	}

	Finalize();
}

void TextureObject::GenerateText(std::string _text, float _scale, glm::vec2 _padding, glm::vec3 _offset) 
{
	if (is_loaded) 
	{
		positions.clear();
		normals.clear();
		uvs.clear();
	}

	texture_level = 1; // enum FONT_TEXTURE in render.h

	float tc_fac1 = 1.0f / 16.0f;
	glm::vec2 uv_padding(tc_fac1 / 5.0f, tc_fac1 / 15.0f);

	_scale -= _scale * uv_padding.x *2.0f;
	float x_inc = _scale / (float)_text.length();

	for (int i = 0; i < _text.length(); ++i) 
	{
		AddLetter(x_inc, x_inc, uv_padding, glm::vec2(0.0f), glm::vec3(x_inc * (float)i, 0.0f, 0.0f) + _offset + glm::vec3(_padding.x,0.0f,0.0f), _text[i]);
	}

	Finalize();
}

void TextureObject::GenerateXYPlane(float _scaleX, float _scaleY, float _padding, glm::vec3 _offset) 
{
	GenerateXYPlane(_scaleX, _scaleY, glm::vec2(_padding, _padding), _offset);
}

void TextureObject::GenerateXYPlane(float _scaleX, float _scaleY, glm::vec2 _padding, glm::vec3 _offset) 
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

	uvs.push_back(glm::vec2(0.0f, 0.0f));
	uvs.push_back(glm::vec2(1.0f, 0.0f));
	uvs.push_back(glm::vec2(1.0f, 1.0f));

	uvs.push_back(glm::vec2(0.0f, 0.0f));
	uvs.push_back(glm::vec2(1.0f, 1.0f));
	uvs.push_back(glm::vec2(0.0f, 1.0f));

	for (unsigned int i = 0; i < positions.size(); ++i)
	{
		positions[i].x *= _scaleX - _padding.x*2.0f;
		positions[i].y *= _scaleY - _padding.y*2.0f;
		positions[i] += glm::vec3(_padding.x, _padding.y, 0.0f);
		positions[i] += _offset;
	}

	Finalize();
}

void TextureObject::AddLetter(float _scaleX, float _scaleY, glm::vec2 _uvPadding, glm::vec2 _padding, glm::vec3 _offset, int _index) 
{
	int num_positions_prev = positions.size();

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

	// 16x16 char font glyph texture
	float uv_per_letter = 1.0f / 16.0f;
	glm::vec2 indices(_index % 16, 15 - (_index / 16));

	uvs.push_back(uv_per_letter*(glm::vec2(0.0f, 0.0f) + indices) + glm::vec2( _uvPadding.x,  _uvPadding.y));
	uvs.push_back(uv_per_letter*(glm::vec2(1.0f, 0.0f) + indices) + glm::vec2(-_uvPadding.x,  _uvPadding.y));
	uvs.push_back(uv_per_letter*(glm::vec2(1.0f, 1.0f) + indices) + glm::vec2(-_uvPadding.x, -_uvPadding.y));
	uvs.push_back(uv_per_letter*(glm::vec2(0.0f, 0.0f) + indices) + glm::vec2( _uvPadding.x,  _uvPadding.y));
	uvs.push_back(uv_per_letter*(glm::vec2(1.0f, 1.0f) + indices) + glm::vec2(-_uvPadding.x, -_uvPadding.y));
	uvs.push_back(uv_per_letter*(glm::vec2(0.0f, 1.0f) + indices) + glm::vec2( _uvPadding.x, -_uvPadding.y));

	for (unsigned int i = num_positions_prev; i < positions.size(); ++i) 
	{
		positions[i].x *= _scaleX - _padding.x*2.0f;
		positions[i].y *= _scaleY - _padding.y*2.0f;
		positions[i] += glm::vec3(_padding.x, _padding.y, 0.0f);
		positions[i] += _offset;
	}
}

int TextureObject::Type() 
{
	return AbstractBaseObject::get_type_id<TextureObject>();
}

void TextureObject::SetSelected(bool _selected)
{
	is_selected = _selected;
}

void TextureObject::AddRectangularPrism(glm::vec3 _scale, glm::vec3 _offset) 
{
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

	uvs.push_back(glm::vec2(0.0f, 0.0f));
	uvs.push_back(glm::vec2(1.0f, 0.0f));
	uvs.push_back(glm::vec2(1.0f, 1.0f));

	uvs.push_back(glm::vec2(0.0f, 0.0f));
	uvs.push_back(glm::vec2(1.0f, 1.0f));
	uvs.push_back(glm::vec2(0.0f, 1.0f));

	uvs.push_back(glm::vec2(0.0f, 0.0f));
	uvs.push_back(glm::vec2(1.0f, 0.0f));
	uvs.push_back(glm::vec2(1.0f, 1.0f));

	uvs.push_back(glm::vec2(0.0f, 0.0f));
	uvs.push_back(glm::vec2(1.0f, 1.0f));
	uvs.push_back(glm::vec2(0.0f, 1.0f));

	uvs.push_back(glm::vec2(0.0f, 0.0f));
	uvs.push_back(glm::vec2(1.0f, 0.0f));
	uvs.push_back(glm::vec2(1.0f, 1.0f));

	uvs.push_back(glm::vec2(0.0f, 0.0f));
	uvs.push_back(glm::vec2(1.0f, 1.0f));
	uvs.push_back(glm::vec2(0.0f, 1.0f));

	uvs.push_back(glm::vec2(0.0f, 0.0f));
	uvs.push_back(glm::vec2(1.0f, 0.0f));
	uvs.push_back(glm::vec2(1.0f, 1.0f));

	uvs.push_back(glm::vec2(0.0f, 0.0f));
	uvs.push_back(glm::vec2(1.0f, 1.0f));
	uvs.push_back(glm::vec2(0.0f, 1.0f));

	uvs.push_back(glm::vec2(0.0f, 0.0f));
	uvs.push_back(glm::vec2(1.0f, 0.0f));
	uvs.push_back(glm::vec2(1.0f, 1.0f));

	uvs.push_back(glm::vec2(0.0f, 0.0f));
	uvs.push_back(glm::vec2(1.0f, 1.0f));
	uvs.push_back(glm::vec2(0.0f, 1.0f));

	uvs.push_back(glm::vec2(0.0f, 0.0f));
	uvs.push_back(glm::vec2(1.0f, 0.0f));
	uvs.push_back(glm::vec2(1.0f, 1.0f));

	uvs.push_back(glm::vec2(0.0f, 0.0f));
	uvs.push_back(glm::vec2(1.0f, 1.0f));
	uvs.push_back(glm::vec2(0.0f, 1.0f));

	for (unsigned int i = prev_num_positions; i < positions.size(); ++i) 
	{
		positions[i].z += 1.0f;
		positions[i] *= _scale;
		positions[i] += _offset;
	}
}

void TextureObject::readObjFromFile(std::string _name)
{
	readObjFromFile(_name, 1.0f, glm::vec3(0.0f));
}

void TextureObject::readObjFromFile(std::string _name, float _scale, glm::vec3 _offset)
{
	std::string strFullPath = MiscFunctions::relativeToAbsolutePath(DirectoryInfo::RELATIVE_MODELS_DIR + _name + DirectoryInfo::MODEL_EXT);

	tinyobj::attrib_t attrib;
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;

	std::string err;
	bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &err, strFullPath.c_str());

	// Loop over shapes
	for (size_t s = 0; s < shapes.size(); s++)
	{
		// Loop over faces(polygon)
		size_t index_offset = 0;
		for (size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); f++)
		{
			int fv = shapes[s].mesh.num_face_vertices[f];

			// Loop over vertices in the face.
			for (size_t v = 0; v < fv; v++)
			{
				// access to vertex
				tinyobj::index_t idx = shapes[s].mesh.indices[index_offset + v];
				
				float vx = attrib.vertices[3 * idx.vertex_index + 0];
				float vy = attrib.vertices[3 * idx.vertex_index + 1];
				float vz = attrib.vertices[3 * idx.vertex_index + 2];
				
				float nx = attrib.normals[3 * idx.normal_index + 0];
				float ny = attrib.normals[3 * idx.normal_index + 1];
				float nz = attrib.normals[3 * idx.normal_index + 2];
				
				float tx = 0.0f;
				float ty = 0.0f;

				if (attrib.texcoords.size() > 1) 
				{
					tx = attrib.texcoords[2 * idx.texcoord_index + 0];
					ty = attrib.texcoords[2 * idx.texcoord_index + 1];
				}

				positions.push_back(_scale * glm::vec3(vx, vy, vz) + _offset);
				normals.push_back(glm::vec3(nx, ny, nz));
				uvs.push_back(glm::vec2(tx, ty));
			}

			index_offset += fv;

			// per-face material
			shapes[s].mesh.material_ids[f];
		}
	}

	Finalize();
}

void TextureObject::readObjFromFile(std::string _name, float _scale)
{
	readObjFromFile(_name, _scale, glm::vec3(0.0f));
}
