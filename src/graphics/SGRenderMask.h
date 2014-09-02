/*
 * SGRenderMask.h
 *
 *  Created on: Aug 31, 2014
 *      Author: pourya
 */

#ifndef SGRENDERMASK_H_
#define SGRENDERMASK_H_

#include "SGQuad.h"

namespace PS {
    namespace SG {

        class SGRenderMask : public SGQuad {

        public:
        	SGRenderMask();
        	SGRenderMask(GLTexture* lpTex);
            virtual ~SGRenderMask();

            void setup();

            void draw();
        };

    }
}

#endif /* SGRENDERMASK_H_ */
