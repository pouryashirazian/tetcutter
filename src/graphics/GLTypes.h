/*
 * GLTypes.h
 *
 *  Created on: Dec 16, 2013
 *      Author: pshiraz
 */

#ifndef GLTYPES_H_
#define GLTYPES_H_

namespace PS {
namespace GL {
/*
#define GL_LINES 0x0001
#define GL_LINE_LOOP 0x0002
#define GL_POINT_BIT 0x00000002
#define GL_CLIENT_VERTEX_ARRAY_BIT 0x00000002
#define GL_LINE_STRIP 0x0003
#define GL_LINE_BIT 0x00000004
#define GL_TRIANGLES 0x0004
#define GL_TRIANGLE_STRIP 0x0005
#define GL_TRIANGLE_FAN 0x0006
#define GL_QUADS 0x0007
#define GL_QUAD_STRIP 0x0008
#define GL_POLYGON_BIT 0x00000008
#define GL_POLYGON 0x0009
*/


#define INVALID_GLBUFFER ~0


//Memory Buffer Type
enum MemoryBufferType {mbtPosition, mbtNormal, mbtColor, mbtTexCoord, mbtFaceIndices, mbtCount};


//Face Types
enum FaceType {ftPoints = 0x0000,
			   ftLines = 0x0001,
			   ftLineLoop = 0x0002,
			   ftLineStrip = 0x0003,
			   ftTriangles = 0x0004,
			   ftTriangleStrip = 0x0005,
			   ftTriangleFan = 0x0006,
			   ftQuads = 0x0007,
			   ftQuadStrip = 0x0008,
			   ftPolygon = 0x0009};

//Shader Effect Type
enum ShaderEffectType {setFixedFunction, setCustom};

}
}

#endif /* GLTYPES_H_ */
