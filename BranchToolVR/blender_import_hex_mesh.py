import bpy
import numpy as np
import os
from io_msh import recalc_outer_surface
from blender_bezier_gen import hello

os.system('cls')

filepath = "D:\Sayak\BranchToolVR\BranchToolVR\hexmesh.obj"

'''
# import OBJ file
hexmesh = bpy.ops.import_scene.obj(filepath=filepath)
hexmesh_object = bpy.context.selected_objects[0]

# attach to scene and validate context
scn = bpy.context.scene
scn.objects.active = hexmesh_object
hexmesh_object.select = True
'''

# read data from file
mesh_vertices = []
with open(filepath, 'r') as f:
	hex_vertices = []
	count = 0
	for line in f: # TODO: Stop iterating after all vertices are read
		if line.startswith('v'):
			if count != 0 and count % 8 == 0:
				mesh_vertices.append(hex_vertices)
				hex_vertices = []
			strarr = np.array(line.split())
			strarr = np.delete(strarr, 0)
			hex_vertices.append(strarr.astype(np.float))
			count += 1
		elif line.startswith('f'):
			break

#print(mesh_vertices)
print(len(mesh_vertices))

# create new mesh and add vertices
hexmesh = bpy.data.meshes.new(name='hexmesh')
hexmesh.vertices.add(len(mesh_vertices)*8)
for index, hex_vertices in enumerate(mesh_vertices):
	hexahedron = hexmesh.hexahedra.add()
	for hex_index, hex_vertex in enumerate(hex_vertices):
		x,z,y = hex_vertex
		hexmesh.vertices[8 * index + hex_index].co = (x, -1 * y, z)
		hexahedron.vertices[hex_index] = 8 # TODO: Don't know yet!!

# finish up mesh and compute outer surface
hello()
hexmesh.update()
recalc_outer_surface(hexmesh)
hexmesh.update()

# create blender object for mesh
hexmesh_object = bpy.data.objects.new(name='hexmesh_obj', object_data=hexmesh)

# attach to scene and validate context
scn = bpy.context.scene
scn.objects.link(hexmesh_object)
scn.objects.active = hexmesh_object
hexmesh_object.select = True
