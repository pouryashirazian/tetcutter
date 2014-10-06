/*
 * GLMemBuffer.h
 *
 *  Created on: Oct 5, 2014
 *      Author: pourya
 */

#ifndef GLMEMBUFFER_H_
#define GLMEMBUFFER_H_

#include "GLTypes.h"
#include "base/Vec.h"

namespace PS {
namespace GL {



/*!
 * OpenGL Memory Buffer
 */
class GLMemoryBuffer {
public:
	GLMemoryBuffer();
	GLMemoryBuffer(GLBufferType type, int step,
			   	     int datatype, U32 szTotal,
			   	     const void* lpData, GLBufferUsage usage = gbuStaticDraw);

	virtual ~GLMemoryBuffer();

	bool setup(GLBufferType type, int step,
	   	     int datatype, U32 szTotal,
	   	     const void* lpData, GLBufferUsage usage = gbuStaticDraw);
	void cleanup();

	//Resize buffer
	void resize(U32 szTotal, const void* lpData);

	void attach();
	void detach();
	void drawElements(int faceMode, int ctElements);

	/*!
	 * Modifies the buffer for changes
	 */
	bool modify(U32 offset, U32 szTotal, const void* lpData);
	bool readBack(U32 szOutBuffer, void* lpOutBuffer) const;

	//Read Attribs
	bool isValid() const { return m_isValid;}
	int step() const {return m_step;}
	U32 size() const {return m_szBuffer;}
	U32 handle() const {return m_handle;}
	GLBufferType type() const {return m_bufferType;}

protected:
	void init();

private:
	bool m_isValid;
	int m_usage;
	int m_step;
	int m_dataType;
	U32 m_szBuffer;
	U32 m_handle;
	GLBufferType m_bufferType;
};



} /* namespace MESH */
} /* namespace PS */

#endif /* GLMEMBUFFER_H_ */
