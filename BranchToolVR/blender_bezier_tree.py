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
countCurve = 0
depth = -1 #changed from 0 b/c we don't want a spring center for any of the first level curves (our traditional roots)
springs = []
center = []

# read data from file
with open('C:/Users/SurfLab/Documents/VR/BranchToolVR/BranchToolVR/curveTree.dat', 'r') as f:
	coords = []
	for line in f:
		if line.startswith('###'):
			if countCurve != 0:
				curves.append(coords)
			coords = []
			countCurve += 1
			continue
		if line.startswith('//'):
			depth -= 1
			continue
		strarr = np.array(line.split())	# split into a string numpy array
		coords.append(strarr.astype(np.float))	# convert string array to float array
		depth += 1
	curves.append(coords)	# append the curve from the last iteration
	if depth > 0:
		center[0] = curves[depth].coords[0] # mark "p0" point as center of a spring constraint
		center[1] = curves[depth] #object1 attachment
		center[2] = curves[depth-1] #object2 attachment
		#unsure if the order of the attachments matters or not
		springs.append(center)


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

	# attach to scene and validate context
	scn = bpy.context.scene
	scn.objects.link(curveObject)
	scn.objects.active = curveObject
	curveObject.select = True

# Branch checking attempts
# for index, curve in enumerate(curves):
# 	scn = bpy.context.scene
# 	x0, z0, y0= curve[0]
# 	x3, z3, y3 = curve[3]
# 	for index2, curve2 in enumerate(curves):
# 		if index != index2:
# 			u0, w0, v0 = curve2[0]
# 			u1, w1, v1 = curve2[1]
# 			u2, w2, v2 = curve2[2]
# 			u3, w3, v3 = curve2[3]
#
# 			# Implicit Bezier Stuff
# 			vals = [0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9, 1.0]
# 			for t in vals:
# 				arg = 1-t
# 				Px = math.pow((arg), 3)*u0 + 3*math.pow((arg), 2)*t*u1 +3*(arg)*math.pow(t, 2)*u2 + math.pow(t, 3)*u3
# 				Py = math.pow((arg), 3)*w0 + 3*math.pow((arg), 2)*t*w1 +3*(arg)*math.pow(t, 2)*w2 + math.pow(t, 3)*w3
# 				Pz = math.pow((arg), 3)*v0 + 3*math.pow((arg), 2)*t*v1 +3*(arg)*math.pow(t, 2)*v2 + math.pow(t, 3)*v3
# 				d0 = math.sqrt(math.pow((x0 - Px), 2) + math.pow((y0 - Py), 2) + math.pow((z0 - Pz), 2))
# 				d3 = math.sqrt(math.pow((x3 - Px), 2) + math.pow((y3 - Py), 2) + math.pow((z3 - Pz), 2))
# 				if d0 < 0.04:
# 					mesh = bpy.data.meshes.new('spring_constraint' + str(index) + str(index2))
# 					basic_sphere = bpy.data.objects.new("spring_constraint" + str(index) + str(index2), mesh)
# 					scn.objects.link(basic_sphere)
# 					scn.objects.active = basic_sphere
# 					basic_sphere.select = True
# 					bpy.data.objects["spring_constraint" + str(index) + str(index2)].location = (x0, -1 * y0, z0)
# 					bm = bmesh.new()
# 					bmesh.ops.create_uvsphere(bm, u_segments=12, v_segments=6, diameter=0.04)
# 					bm.to_mesh(mesh)
# 					bm.free()
# 					break
# 				elif d3 < 0.04:
# 					mesh = bpy.data.meshes.new('spring_constraint' + str(index) + str(index2))
# 					basic_sphere = bpy.data.objects.new("spring_constraint" + str(index) + str(index2), mesh)
# 					scn.objects.link(basic_sphere)
# 					scn.objects.active = basic_sphere
# 					basic_sphere.select = True
# 					bpy.data.objects["spring_constraint" + str(index) + str(index2)].location = (x3, -1 * y3, z3)
# 					bm = bmesh.new()
# 					bmesh.ops.create_uvsphere(bm, u_segments=12, v_segments=6, diameter=0.04)
# 					bm.to_mesh(mesh)
# 					bm.free()
# 					break
			# d03 = math.sqrt(math.pow((x0 - u3), 2) + math.pow((y0 - v3), 2) + math.pow((z0 - w3), 2))
			# d30 = math.sqrt(math.pow((x3 - u0), 2) + math.pow((y3 - v0), 2) + math.pow((z3 - w0), 2))
			# if d03 <= 0.04:
			# 	mesh = bpy.data.meshes.new('spring_constraint' + str(index) + str(index2))
			# 	basic_sphere = bpy.data.objects.new("spring_constraint" + str(index) + str(index2), mesh)
			# 	scn.objects.link(basic_sphere)
			# 	scn.objects.active = basic_sphere
			# 	basic_sphere.select = True
			# 	bpy.data.objects["spring_constraint" + str(index) + str(index2)].location = (x0, -1 * y0, z0)
			# 	bm = bmesh.new()
			# 	bmesh.ops.create_uvsphere(bm, u_segments=12, v_segments=6, diameter=0.04)
			# 	bm.to_mesh(mesh)
			# 	bm.free()
