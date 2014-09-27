tetcutter
=========
One of the main objectives of virtual reality based surgical simulation systems is the removal of pathologic tissue. 
Cutting imposes many challenges in the development of a robust, interactive surgery 
simulation, not only because of the nonlinear material behavior exhibited by soft tissue but also due to the
complexity of introducing the cutting-induced discontinuity. In most publications, the progressive surgical cutting is modelled
by the conventional finite element (FE) method, in which the high computational cost and error accumulation due to remeshing constrain 
the computational efficiency and accuracy.

We present a GPU-assisted approach to cutting tetrahedral meshes in real-time. The input to our system is a cut trajectory 
and an edge-based data structure representing the tetrahedral mesh. The user moves the cutting tool and the system records the 
path of the blade endpoints. The first intersection between the recorded trajectory and the model marks the beginning of the 
cutting process. The following steps are performed to complete the cut induced by the scalpel on the mesh:

![ScreenShot](https://raw.githubusercontent.com/GraphicsEmpire/tetcutter/master/data/images/workflow.png)

Cut configurations
=========
Cutting a tetrahedra requires a lookup table to handle different cases. This project also helps to identify those cases.

Examples
=========
![ScreenShot](https://raw.githubusercontent.com/GraphicsEmpire/tetcutter/master/data/images/dumbel04.png)
![ScreenShot](https://raw.githubusercontent.com/GraphicsEmpire/tetcutter/master/data/images/tumor04.png)


