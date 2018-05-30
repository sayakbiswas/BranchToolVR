# BranchToolVR

A tool designed to map vascular systems present in medical images in virtual reality. The previous non-vr iteration of the tool can be found here: https://github.com/microdr/Branch-Tool

## TODO list:
- [ ] Optimization
- [ ] Control adjustments for improved UX
- [ ] Coarsify the volumetric hex mesh for SOFA
	- Working but somewhat odd behavior, look into per-slider basis and horizontal strip effect
- [ ] Convert the bezier curves in blender python to thick curves
- [x] Slider for adjusting isovalue tolerance
- [x] File dialog for DicomSet selection
- [x] Auto detect sensible windowing values
	- [x] Allow swapping from 2D to 3D interface (no need to determine initial viewport)
- [x] Adjust center/start position for Point Cloud Object
- [x] Zooming/Scaling the voxels
- [x] differentiate isovalue surfaces by color when exporting
- [x] Data set testing
	- [x] Same data set works in different memory locations
	- [x] Additional sample set via MATLAB works
	- [x] Additional sets via Kurenov work
	- Note: only uncompressed DICOM files work (testing with anonymized samples threw odd exceptions)
- [x] Generate isosurface of tumor from the point cloud and export - Surface generated using blender python script
- [x] triangulate vein structures - Not needed, blender2sofa takes care of this
- [x] export triangulation (traced veins to blender) - Not needed, export curve control points
- [x] DICOM image in 2D window renders as black when HMD is connected
- [x] Change tracing from Grip buttons to trigger button toggles
- [x] Add spoof input key for the app menu button
- [x] Add spoof input key for the touchpad which initializes a new curve
- [x] Remap the new curve button to the touchpad
- [x] Remap the create sphere action to the trigger
- [x] Collision between voxels and the ray from controller to generate spheres
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

## Running Solution:
- Download/Clone Repository
- Open "BranchToolVR.sln"
- Go to Project->BranchToolVR Properties->Linker->Input and add $(ProjectDir)Libraries\glew-2.1.0\lib\Release\x64\glew32.lib
- Go to Tools->Options->Debugging->Symbols and check Microsoft Symbol Servers
- Change project type to Release
- If you get errors saying "application could not be loaded", you need to copy BranchToolVR\Libraries\glew-2.1.0\bin\Release\x64\glew32.dll and BranchToolVR\Libraries\openvr-master\bin\win64\openvr_api.dll to BranchToolVR-master\x64\Release
