/*
 * AvatarRing.cpp
 *
 *  Created on: Sep 21, 2014
 *      Author: pourya
 */

#include <deformable/AvatarRing.h>
#include "base/Logger.h"
#include "graphics/selectgl.h"

using namespace PS;

namespace PS {
namespace MESH {

   //Effect
   class TexturedEffect : public SGEffect {
   public:
       TexturedEffect(GLShader* s):SGEffect(s) {
           m_idSampler = s->getUniformLocation("sampler2d");
       }

       void bind() {
           SGEffect::bind();
           m_lpShader->setUniform1i(m_idSampler, 0);

       }
   private:
       int m_idSampler;
   };


AvatarRing::AvatarRing(GLTexture* aTex):IAvatar(), m_isSweptQuadValid(false) {
	// TODO Auto-generated constructor stub
	this->init();
	m_lpTex = aTex;
}

AvatarRing::~AvatarRing() {
	// TODO Auto-generated destructor stub
	clearCutContext();
}

void AvatarRing::init() {
	m_lpTex = NULL;
	m_isSweptQuadValid = false;

	double radius = 0.3;
	int sectors = 8;
	vec3d origin = vec3d(0.0, 0.0, 0.0);

	m_vSegmentsRef.reserve(sectors);

	vector<float> vertices;
	vertices.reserve(sectors * 3);

	double oneOverSector = 1.0 / static_cast<double>(sectors);
	for(int i=0; i<sectors+1; i++) {
		double angle = double(i) * TwoPi * oneOverSector;
		vec3d v = origin + vec3d::mul(radius, vec3d(cos(angle), 0.0, sin(angle)));
		m_vSegmentsRef.push_back(v);

		vertices.push_back(v.x);
		vertices.push_back(v.y);
		vertices.push_back(v.z);
	}

	//Set the size of cur segments and quads
	m_vSegmentsCur.resize(m_vSegmentsRef.size());
	m_vSweptQuads.resize(m_vSegmentsRef.size() * 2);


	//Outline
	Geometry g;
	g.init(3, 4, 2, ftTriangles);
	g.addCylinder(radius, 2.0, sectors, false, false);
	g.addPerVertexColor(vec4f(1.0, 0.0, 0.0, 1.0));
	m_outline.setup(g);
	m_outline.setWireFrameMode(true);
	//m_outline.setupPerVertexColor(vec4f(0, 0, 0, 1));

	//Geometry
	SGMesh::setup(g);



	resetTransform();
    if(TheShaderManager::Instance().has("textured")) {
        m_spEffect = SmartPtrSGEffect(new TexturedEffect(TheShaderManager::Instance().get("textured")));
    }
}

void AvatarRing::draw() {

	glDisable(GL_CULL_FACE);

	glEnable(GL_LIGHTING);
	if(m_lpTex)
		m_lpTex->bind();

	SGMesh::draw();

	if(m_lpTex)
		m_lpTex->unbind();
	glDisable(GL_LIGHTING);

    //WireFrame
	/*
	m_spTransform->bind();
	glPushAttrib(GL_ALL_ATTRIB_BITS);
		glColor3f(0,0,0);
    	m_outline.drawNoEffect();
    glPopAttrib();
    m_spTransform->unbind();
    */

    glEnable(GL_CULL_FACE);


    //draw markers
    glDisable(GL_LIGHTING);
	glPushAttrib(GL_ALL_ATTRIB_BITS);
		glLineWidth(1.0f);
		glColor3d(1.0, 0.0, 0.0);
		glBegin(GL_LINE_STRIP);
		for (U32 i = 0; i < m_vCuttingPath.size(); i+=8) {
			glVertex3dv(m_vCuttingPath[i].cptr());
		}
		glEnd();

		glColor3d(0.0, 0.0, 0.0);
		glBegin(GL_LINE_STRIP);
		for (U32 i = 0; i < m_vSegmentsCur.size(); i++)
			glVertex3dv(m_vSegmentsCur[i].cptr());
		glEnd();
	glPopAttrib();

	//Draw Swept Quad
	if (m_isSweptQuadValid) {
		glPushAttrib(GL_ALL_ATTRIB_BITS);
		glDisable(GL_CULL_FACE);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE);

		glColor4d(1.0, 0.0, 1.0, 0.5);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glBegin(GL_QUAD_STRIP);
		for (U32 i = 0; i < m_vSweptQuads.size(); i++)
			glVertex3dv(m_vSweptQuads[i].cptr());
		glEnd();

		glDisable(GL_BLEND);
		glEnable(GL_CULL_FACE);
		glPopAttrib();
	}

	glEnable(GL_LIGHTING);
}

//From Gizmo Manager
void AvatarRing::onTranslate(const vec3f& delta, const vec3f& pos) {
	if(m_lpTissue == NULL || !m_isToolActive)
		return;

	//Box test
	m_aabbCurrent = this->aabb();
	m_aabbCurrent.transform(m_spTransform->forward());

	//1.If boxes donot intersect and sweptquad is invalid then return
	if (!m_lpTissue->aabb().intersect(m_aabbCurrent) || isGripActive()) {

		if(m_isSweptQuadValid) {
			//call the cut method if the tool has passed through the tissue
			int res = m_lpTissue->cut(m_vSegmentsCur, m_vSweptQuads, true);
			LogInfoArg1("Tissue cut. res = %d", res);
			if((res > 0) && (m_fOnCutFinished != NULL))
				m_fOnCutFinished();

			clearCutContext();
			updateVolMeshInfoHeader();
		}

		return;
	}

	//edges
	//Set the size of cur segments and quads
	m_vSegmentsCur.resize(m_vSegmentsRef.size());
	for(U32 i=0; i < m_vSegmentsRef.size(); i++) {
		vec3d vd = m_vSegmentsRef[i];
		vec3f vf = vec3f(vd.x, vd.y, vd.z);
		vf = m_spTransform->forward().map(vf);
		m_vSegmentsCur[i] = vec3d(vf.x, vf.y, vf.z);
	}


	//Swept quad: starts when blade crosses the tissue first and ends where the blade leaves the body
	m_isSweptQuadValid = false;
	if(m_vCuttingPath.size() == 0) {
		//write even
		for(U32 i = 0; i < m_vSegmentsCur.size(); i++) {
			m_vSweptQuads[i * 2] = m_vSegmentsCur[i];
		}
	}
	else {
		//write odds
		for(U32 i = 0; i < m_vSegmentsCur.size(); i++) {
			m_vSweptQuads[i * 2 + 1] = m_vSegmentsCur[i];
		}
		m_isSweptQuadValid = true;
	}


	//Insert new scalpal position into buffer
	m_vCuttingPath.push_back(m_vSegmentsCur.back());

	//delete last if overflow buffer
	if (m_vCuttingPath.size() > MAX_SCALPEL_TRAJECTORY_NODES)
		m_vCuttingPath.erase(m_vCuttingPath.begin());
}

void AvatarRing::clearCutContext() {
	if(m_lpTissue)
		m_lpTissue->clearCutContext();
	m_vCuttingPath.clear();
	m_isSweptQuadValid = false;
	m_applyGripper = false;
}


} /* namespace MESH */
} /* namespace PS */
