tetcutter
=========
Cutting tetrahedral meshes with applications in surgical simulation, visual effects,
mesh manipulation and 3D printing! Watch this demo on youtube:

https://www.youtube.com/watch?v=r2nqwWJuNPc

Introduction
=========
A tetrahedral element has 4 faces, 6 edges and 4 vertices:

![Alt text](https://raw.githubusercontent.com/GraphicsEmpire/tetcutter/master/data/images/tetconfig3.png)

When cut, each element is decomposed into multiple sub-elements:

![Alt text](https://raw.githubusercontent.com/GraphicsEmpire/tetcutter/master/data/images/case11.png)

The cut results in a reconnected mesh with smaller elements at the cut plane:

![Alt text](https://raw.githubusercontent.com/GraphicsEmpire/tetcutter/master/data/images/tumor04.png)

License:
=========
The software library is released under the terms of MIT license.

External Dependencies
=========
The source code for the example application uses the following libraries:

1. GLFW: http://www.glfw.org/
2. TBB: https://www.threadingbuildingblocks.org/
3. libpng

How to build?
=========
Just execute build.sh on a unix-based system.

```
> ./build.sh
```

or perform the following in order:

```
> mkdir bin && cd bin
> cmake ..
> make
```

Run with the example meshes
==========
Start cutting a voxel grid of 8x8x8

```
> ./bin/tetcutter -e cube_8_8_8  
```

The number of cells in the voxel can be altered arbitrarily: **cube_[nx]_[ny]_[nz]**

```
> ./bin/tetcutter -e cube_10_4_8  
```

The same rule holds for our eggshell model: **eggshell_[nh]_[nv]**
where **nh** and **nv** are the number of segments along the latitude and longtitude of the model, respectively.

```
> ./bin/tetcutter -e eggshell_20_16
```

Other Input Models
===========
Input models should follow the VEGA file format for volumetric meshes.
More about this format:
http://run.usc.edu/vega/index.html
