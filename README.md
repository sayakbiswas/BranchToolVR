# BranchToolVR

A tool designed to map vascular systems present in medical images in virtual reality. The previous non-vr iteration of the tool can be found here: https://github.com/microdr/Branch-Tool

## TODO list:
- [ ] File dialog for DicomSet selection
- [ ] Coarsify the volumetric hex mesh for SOFA
- [ ] Convert the bezier curves in blender python to thick curves
- [ ] Auto detect sensible windowing values
- [ ] Data set testing
- [ ] Zooming/Scaling the voxels
- [x] Generate isosurface of tumor from the point cloud and export - Surface generated using blender python script
- [x] triangulate vein structures - Not needed, blender2sofa takes care of this
- [x] export triangulation (traced veins to blender) - Not needed, export curve control points
- [x] DICOM image in 2D window renders as black when HMD is connected
- [x] Change tracing from Grip buttons to trigger button toggles
- [x] Add spoof input key for the app menu button
- [x] Add spoof input key for the touchpad which initializes a new curve
- [x] Remap the new curve button to the touchpad
- [x] Remap the create sphere action to the trigger
- [ ] Collision between voxels and the ray from controller to generate spheres
	- [x] Instead make the controller pointer shorter and use the tip to generate spheres, no collision detection necessary
- [x] Make the branch point marking spheres smaller
- [x] Export curve control points
- [x] Import script for the traced curves using blender python
- [x] Export volumetric hex mesh for carvable organ

## Libraries used:
* OpenGL
* OpenVR
* GLFW
* GLM
* imebra
* tinyobjloader
