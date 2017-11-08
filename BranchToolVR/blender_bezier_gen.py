'''
Adapted from https://blender.stackexchange.com/questions/6750/poly-bezier-curve-from-a-list-of-coordinates
'''

import bpy
import os
import numpy as np

os.system('cls')

# list of all curves
curves = []
countCurve = 0

# read data from file
with open('D:\Sayak\BranchToolVR\BranchToolVR\curves.dat', 'r') as f:
	coords = []
	for line in f:
		if line.startswith('###'):
			if countCurve != 0:
				curves.append(coords)
			coords = []
			countCurve += 1
			continue
		strarr = np.array(line.split())	# split into a string numpy array
		coords.append(strarr.astype(np.float))	# convert string array to float array
	curves.append(coords)	# append the curve from the last iteration

for index, curve in enumerate(curves):
	# create the Curve Datablock
	curveData = bpy.data.curves.new('myCurve' + str(index), type='CURVE') #TODO: Curve name should be different for each curve
	curveData.dimensions = '3D'

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
	
	# create curve object
	curveObject = bpy.data.objects.new('myCurve' + str(index), curveData)

	# attach to scene and validate context
	scn = bpy.context.scene
	scn.objects.link(curveObject)
	scn.objects.active = curveObject
	curveObject.select = True

def hello():
	print('hello')