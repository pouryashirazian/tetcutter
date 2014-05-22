/*
 * SGVoxels.h
 *
 *  Created on: Apr 6, 2014
 *      Author: pourya
 */

#ifndef SGVOXELS_H_
#define SGVOXELS_H_


#include "SGMesh.h"

using namespace PS;
using namespace PS::GL;

namespace PS {
    namespace SG {

        class SGVoxels : public SGMesh {
        public:
        	SGVoxels(const vector<vec3f>& voxels, float side = 1.0f);
            virtual ~SGVoxels();

            void draw();
        };

    }
}




#endif /* SGVOXELS_H_ */
