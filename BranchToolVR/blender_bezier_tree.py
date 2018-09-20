'''
Adapted from https://blender.stackexchange.com/questions/6750/poly-bezier-curve-from-a-list-of-coordinates
'''

import bpy
import os
import math
import bmesh
import numpy as np

os.system('cls')

# list of all curves
curves = []
coord0 = []
countPoints = 0
depth = 0

# read data from file
with open('C:/Users/SurfLab/Documents/VR/BranchToolVR/BranchToolVR/curveTree.dat', 'r') as f:
	coords = []
	for line in f:
		if line.startswith('#'):
			depth -= 1
			continue
		if countPoints >= 4:
			curves.append(coords)
			coords = []
			countPoints = 0
		strarr = np.array(line.split())	# split into a string numpy array
		coords.append(strarr.astype(np.float))	# convert string array to float array
		# if depth != 0 and countPoints == 1:
		# 	coord0.append([float(p) for p in line.split()])
		# 	mesh = bpy.data.meshes.new('spring_constraint')
		# 	basic_sphere = bpy.data.objects.new("spring_constraint")
		# 	scn.objects.link(basic_sphere)
		# 	scn.objects.active = basic_sphere
		# 	basic_sphere.select = True
		# 	bpy.data.objects["spring_constraint"].location = (coord0[0], coord0[1], coord0[2])
		# 	bm = bmesh.new()
		# 	bmesh.ops.create_uvsphere(bm, u_segments=12, v_segments=6, diameter=0.04)
		# 	bm.to_mesh(mesh)
		# 	bm.free()
		# 	coord0 = []
		depth += 1
		countPoints += 1
	curves.append(coords)	# append the curve from the last iteration

for index, curve in enumerate(curves):
	# create the Curve Datablock
	curveData = bpy.data.curves.new('trace' + str(index), type='CURVE') #TODO: Curve name should be different for each curve
	curveData.dimensions = '3D'
	u,v,w = curve[0]
	a,b,c = curve[3]
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

	# attach to scene and validate context
	scn = bpy.context.scene
	scn.objects.link(curveObject)
	scn.objects.active = curveObject
	curveObject.select = True
