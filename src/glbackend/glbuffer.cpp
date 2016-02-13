#include <cstring>
#include "glbuffer.h"
#include "glselect.h"

using namespace ps::opengl;


GLBuffer::GLBuffer() {
	init();
}


GLBuffer::GLBuffer(GLBufferType type, int step,
  	     	 	 	 	 	   int datatype, U32 szTotal,
  	     	 	 	 	 	   	const void* lpData, GLBufferUsage usage) {
	init();
	this->setup(type, step, datatype, szTotal, lpData, usage);
}

GLBuffer::~GLBuffer() {
	cleanup();
}

void GLBuffer::init() {
	m_isValid  = false;
	m_usage    = GL_DYNAMIC_DRAW;
	m_step 	   = 0;
	m_dataType = GL_FLOAT;
	m_szBuffer = 0;
	m_handle   = INVALID_GLBUFFER;
	m_bufferType = gbtPosition;
}

void GLBuffer::cleanup() {
	if(m_isValid)
		glDeleteBuffers(1, &m_handle);
	m_handle = 0;
	m_isValid = false;
}

bool GLBuffer::setup(GLBufferType type, int step,
  	     	 	 	 	   int datatype, U32 szTotal,
  	     	 	 	 	   const void* lpData, GLBufferUsage usage) {
	m_bufferType = type;
	m_step = step;

	m_dataType = datatype;
	m_szBuffer = szTotal;
	m_usage = usage;

	if (m_bufferType == gbtFaceIndex) {
		glGenBuffers(1, &m_handle);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_handle);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_szBuffer, lpData, m_usage);
	}
	else {
		glGenBuffers(1, &m_handle);
		glBindBuffer(GL_ARRAY_BUFFER, m_handle);
		glBufferData(GL_ARRAY_BUFFER, m_szBuffer, lpData, m_usage);
	}

	m_isValid = true;
	return true;
}

void GLBuffer::resize(U32 szTotal, const void* lpData) {
	if(!m_isValid)
		return;

	m_szBuffer = szTotal;
	glBindBuffer(GL_ARRAY_BUFFER, m_handle);
	glBufferData(GL_ARRAY_BUFFER, m_szBuffer, lpData, m_usage);
}

bool GLBuffer::readBack(U32 szOutBuffer, void* lpOutBuffer) const {
	if(!m_isValid || szOutBuffer < m_szBuffer)
		return false;

	if (m_bufferType == gbtFaceIndex) {
		glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, m_handle);
		void* pData = (void *) glMapBuffer( GL_ELEMENT_ARRAY_BUFFER, GL_READ_ONLY);
		if(pData == NULL)
			return false;

		memcpy(lpOutBuffer, pData, m_szBuffer);
		glUnmapBuffer( GL_ELEMENT_ARRAY_BUFFER);
	}
	else {
		glBindBuffer( GL_ARRAY_BUFFER, m_handle);
		void* pData = (void *) glMapBuffer( GL_ARRAY_BUFFER, GL_READ_ONLY);
		if (pData == NULL)
			return false;

		memcpy(lpOutBuffer, pData, m_szBuffer);
		glUnmapBuffer( GL_ARRAY_BUFFER);
	}
	return true;
}

void GLBuffer::bind() {
	if(!m_isValid)
		return;

	//Bind Buffer
	if(m_bufferType == gbtFaceIndex)
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_handle);
	else
		glBindBuffer(GL_ARRAY_BUFFER, m_handle);

	switch(m_bufferType) {
	case(gbtColor):
		glColorPointer(m_step, m_dataType, 0, 0);
		glEnableClientState(GL_COLOR_ARRAY);
		break;
	case(gbtTexCoord):
		glTexCoordPointer(m_step, m_dataType, 0, 0);
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		break;
	case(gbtNormal):
		glNormalPointer(m_dataType, 0, 0);
		glEnableClientState(GL_NORMAL_ARRAY);
		break;
	case(gbtPosition):
		glVertexPointer(m_step, m_dataType, 0, 0);
		glEnableClientState(GL_VERTEX_ARRAY);
		break;
	case(gbtFaceIndex):
		glEnableClientState(GL_ELEMENT_ARRAY_BUFFER);
		break;
    case(gbtCount):
        break;
	}

}

void GLBuffer::unbind() {
	if(!m_isValid)
		return;

	switch(m_bufferType) {
	case(gbtColor):
		glDisableClientState(GL_COLOR_ARRAY);
		break;
	case(gbtTexCoord):
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
		break;
	case(gbtNormal):
		glDisableClientState(GL_NORMAL_ARRAY);
		break;
	case(gbtPosition):
		glDisableClientState(GL_VERTEX_ARRAY);
		break;
	case(gbtFaceIndex):
		glDisableClientState(GL_ELEMENT_ARRAY_BUFFER);
		break;
    case(gbtCount):
        break;
	}
}

void GLBuffer::drawElements(int faceMode, int ctElements) {
	glDrawElements(faceMode, ctElements, m_dataType, (GLvoid*)0);
}

bool GLBuffer::modify(U32 offset, U32 szTotal, const void* lpData) {
	if(!m_isValid)
		return false;

	if(szTotal != m_szBuffer || lpData == NULL)
		return false;

	//Bind Buffer
	if(m_bufferType == gbtFaceIndex) {
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_handle);
		glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, offset, szTotal, lpData);
	}
	else {
		glBindBuffer(GL_ARRAY_BUFFER, m_handle);
		glBufferSubData(GL_ARRAY_BUFFER, offset, szTotal, lpData);
	}


	return true;
}

