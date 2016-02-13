#ifndef GLMEMBUFFER_H_
#define GLMEMBUFFER_H_

#include "gltypes.h"
#include "glbindable.h"
#include "base/vec.h"

using namespace ps;

namespace ps {
namespace opengl {


/*!
 * OpenGL Memory Buffer
 */
class GLBuffer : public GLBindable {
public:
    GLBuffer();
    GLBuffer(GLBufferType type, int step,
			   	     int datatype, U32 szTotal,
			   	     const void* lpData, GLBufferUsage usage = gbuStaticDraw);

    virtual ~GLBuffer();

	bool setup(GLBufferType type, int step,
	   	     int datatype, U32 szTotal,
	   	     const void* lpData, GLBufferUsage usage = gbuStaticDraw);
	void cleanup();

	//Resize buffer
	void resize(U32 szTotal, const void* lpData);

    void bind();
    void unbind();
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



}
}

#endif /* GLMEMBUFFER_H_ */
