tetcutter
=========
Cutting tetrahedral meshes with applications in surgical simulation, visual effects,
mesh manipulation and 3D printing!

License:
=========
The software library is release under the terms of MIT license.

External Dependencies
=========
The source code for the example application uses the following libraries:

1. GLFW: http://www.glfw.org/
2. TBB: https://www.threadingbuildingblocks.org/
3. libpng

How to build?
=========
Just execute build.sh on a unix-based system.

>> ./build.sh

or perform the following in order:

>> mkdir bin && cd bin
>> cmake ..
>> make

Run with the example meshes
==========
Start cutting a voxel grid of 8x8x8

>> ./bin/tetcutter -e cube_8_8_8  

The number of cells in the voxel can be altered arbitrarily: cube_<nx>_<ny>_<nz>:

>> ./bin/tetcutter -e cube_10_4_8  

The same rule holds for our eggshell model: eggshell_<nh>_<nv>
where <nh> and <nv> are the number of segments along the latitude and longtitude of the model, respectively.


>> ./bin/tetcutter -e eggshell_20_16


Input Models
===========
Input models should follow the VEGA file format for volumetric meshes.
More about this format:
http://run.usc.edu/vega/index.html
