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

# read data from file
# Filepath stuff needs work
with open(os.path.join(bpy.path.abspath(bpy.data.filepath), 'curveTree.dat'), 'r') as f:
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
