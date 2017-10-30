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

for curve in curves:
	# create the Curve Datablock
	curveData = bpy.data.curves.new('myCurve', type='CURVE')
	curveData.dimensions = '3D'
	curveData.resolution_u = 2

	# map coords to bezier
	polyline = curveData.splines.new('BEZIER')
	polyline.bezier_points.add(len(curve))
	for i, coord in enumerate(curve):
		print(coord)
		print()
		x,y,z = coord
		polyline.bezier_points[i].co = (x, y, z)

	# create curve object
	curveObject = bpy.data.objects.new('myCurve', curveData)

	# attach to scene and validate context
	scn = bpy.context.scene
	scn.objects.link(curveObject)
	scn.objects.active = curveObject
	curveObject.select = True