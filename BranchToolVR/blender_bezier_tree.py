'''
Adapted from https://blender.stackexchange.com/questions/6750/poly-bezier-curve-from-a-list-of-coordinates
'''

import bpy
import os
import math
import bmesh
import numpy as np
import sys

os.system('cls')

# list of all curves
curves = []
countCurve = -1
depth = -1 #changed from 0 b/c we don't want a spring center for any of the first level curves (our traditional roots)
springs = []
blah = 0
currParent = []
tmp = 0
currParent.append(-1)
parentList = []
filename = './curveTree.dat'
pathdir = bpy.context.space_data.text.filepath;

# read data from file
print(bpy.context.space_data.text.filepath)
with open(os.path.join(pathdir, '../BranchToolExport/curveTree.dat'), 'r') as f:
#with open('C:/Users/SurfLab/Documents/VR/BranchToolVR/BranchToolVR/curveTree.dat', 'r') as f:
	coords = []
	for line in f:
		if line.startswith('###'):
			if countCurve != -1:
				curves.append(coords)
			parentList.append(currParent[-1])
			#parent += 1
			coords = []
			blah = 0
			depth += 1

			countCurve += 1
			currParent.append(countCurve)
			continue
		elif line.startswith('//'):
			depth -= 1
			currParent.pop()
			# parent -= 1
			continue
		strarr = np.array(line.split())	# split into a string numpy array
		coords.append(strarr.astype(np.float))	# convert string array to float array
		if blah%4 == 0:
			if depth > 0:
				# mark "p0" point as center of a spring constraint
				springs.append(strarr.astype(np.float))
		blah += 1;
	curves.append(coords)	# append the curve from the last iteration

# format of springs entries: x, y, z, parent curve #, child curve #

for index, curve in enumerate(curves):
	# create the Curve Datablock
	curveData = bpy.data.curves.new('myCurve' + str(index), type='CURVE') #TODO: Curve name should be different for each curve
	curveData.dimensions = '3D'
	u,v,w = curve[0]
	a,b,c = curve[3]
	# use distance formula on the end points and scales by 0.05 to assign curve thickness
	curveData.bevel_depth = math.sqrt(math.pow((u - a), 2) + math.pow((v - b), 2) + math.pow((w - c), 2)) * 0.05
	curveData.fill_mode = 'FULL'
	# map coords to bezier
	polyline = curveData.splines.new('BEZIER')
	polyline.bezier_points.add(1)

	# cubic bezier in blender, P0 and P3 are control points, P1 and P2 are handles
	x,z,y = curve[0]
	polyline.bezier_points[0].handle_left = (x, -1 * y, z) #P0 - TODO: scale in direction
	polyline.bezier_points[0].co = (x, -1 * y, z) #P0
	x,z,y = curve[1]
	polyline.bezier_points[0].handle_right = (x, -1 * y, z) #P1
	x,z,y = curve[2]
	polyline.bezier_points[1].handle_left = (x, -1 * y, z) #P2
	x,z,y = curve[3]
	polyline.bezier_points[1].co = (x, -1 * y, z) #P3
	polyline.bezier_points[1].handle_right = (x, -1 * y, z) #P3 - TODO: scale in direction

	polyline.resolution_u = 20 # Number of straight line segments used for rendering curve in blender

	# create curve object as thick curve
	curveObject = bpy.data.objects.new('myCurve' + str(index), curveData)
	curveObject.template = 'THICKCURVE'
	curveObject.carvable = True

	if parentList[index] != -1:
		curveObject.object1 = 'myCurve' + str(parentList[index])
	# attach to scene and validate context
	scn = bpy.context.scene
	scn.objects.link(curveObject)
	scn.objects.active = curveObject
	curveObject.select = True

################################################################################

os.system('cls')

filepath = os.path.join(pathdir, '../BranchToolExport/hexmesh0.obj')


# import OBJ file
hexmesh = bpy.ops.import_scene.obj(filepath=filepath)
hexmesh_object = bpy.context.selected_objects[0]

# attach to scene and validate context
scn = bpy.context.scene
scn.objects.active = hexmesh_object
hexmesh_object.select = True
#
#
# def recalc_outer_surface(M):
#   print('recalculating outer surface')
#   triFaceSet = set()
#   for t in M.tetrahedra:
#     for l in tet_faces:
#       f = encodeTriFacet(t.vertices[l[0]],t.vertices[l[1]],t.vertices[l[2]])
#       rf = encodeTriFacet(t.vertices[l[0]],t.vertices[l[2]],t.vertices[l[1]])
#       if rf in triFaceSet:
#         triFaceSet.remove(rf)
#       else:
#         triFaceSet.add(f)
#
# # read data from file
# mesh_vertices = []
# with open(filepath, 'r') as f:
# 	hex_vertices = []
# 	count = 0
# 	for line in f: # TODO: Stop iterating after all vertices are read
# 		if line.startswith('v'):
# 			if count != 0 and count % 8 == 0:
# 				mesh_vertices.append(hex_vertices)
# 				hex_vertices = []
# 			strarr = np.array(line.split())
# 			strarr = np.delete(strarr, 0)
# 			hex_vertices.append(strarr.astype(np.float))
# 			count += 1
# 		elif line.startswith('f'):
# 			print('line starts with f')
# 			if(len(hex_vertices) != 0 and count % 8 == 0):
# 				mesh_vertices.append(hex_vertices)
# 			break
#
# #print(mesh_vertices)
# print(len(mesh_vertices))
# print(count)
#
# # create new mesh and add vertices
# hexmesh = bpy.data.meshes.new(name='hexmesh')
# hexmesh.vertices.add(len(mesh_vertices)*8)
# for index, hex_vertices in enumerate(mesh_vertices):
# 	hexahedron = hexmesh.hexahedra.add()
# 	for hex_index, hex_vertex in enumerate(hex_vertices):
# 		x,z,y = hex_vertex
# 		hexmesh.vertices[8 * index + hex_index].co = (x * 1, -1 * y * 1, z * 1) # * 1 is place holder for scaling the cloud if needed
# 		hexahedron.vertices[hex_index] = 8 * index + hex_index # absolute index of the vertex
#
# # finish up mesh and compute outer surface
# hexmesh.update()
# recalc_outer_surface(hexmesh)
# hexmesh.update()
#
# # create blender object for mesh
# hexmesh_object = bpy.data.objects.new(name='hexmesh_obj', object_data=hexmesh)
#
# # attach to scene and validate context
# scn = bpy.context.scene
# scn.objects.link(hexmesh_object)
# scn.objects.active = hexmesh_object
# hexmesh_object.select = True
