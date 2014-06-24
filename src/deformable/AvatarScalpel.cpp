/*
 * AvatarScalpel.cpp
 *
 *  Created on: Apr 6, 2014
 *      Author: pourya
 */

#include "AvatarScalpel.h"
#include "base/Logger.h"
#include "graphics/selectgl.h"

using namespace PS;

AvatarScalpel::AvatarScalpel(CuttableMesh* tissue):SGMesh(), IGizmoListener() {
	setName("scalpel");
	m_lpTissue = tissue;
	m_isSweptQuadValid = false;
	m_isToolActive = false;

	m_edgeref0 = vec3f(-2.0, 0, 0);
	m_edgeref1 = vec3f(2.0, 0, 0);
	vec3f lo = vec3f(m_edgeref0.x, 0.0f, -0.001f);
	vec3f hi = vec3f(m_edgeref1.x, 1.0f, 0.001);

	//Geometry
	Geometry g;
	g.init(3, 4, 2, ftTriangles);
	g.addCube(lo, hi);
	g.addPerVertexColor(vec4f(0, 1, 0, 1), g.countVertices());
	SGMesh::setup(g);

	//Outline
	Geometry gWireframe;
	gWireframe.init(3, 4, 2, ftQuads);
	gWireframe.addCube(lo, hi);
	m_outline.setup(gWireframe);
	m_outline.setWireFrameMode(true);


	resetTransform();
	if(TheShaderManager::Instance().has("phong")) {
        m_spEffect = SmartPtrSGEffect(new SGEffect(TheShaderManager::Instance().get("phong")));
    }

	//Add a header
	TheSceneGraph::Instance().headers()->addHeaderLine("scalpel", "scalpel");
}

AvatarScalpel::~AvatarScalpel() {
	SGMesh::cleanup();
}

void AvatarScalpel::draw() {


	//SGMesh::draw();

    //WireFrame
    m_spTransform->bind();
    glPushAttrib(GL_ALL_ATTRIB_BITS);
    glDisable(GL_LIGHTING);
    glColor3f(0,0.5f,1.0f);
    	m_outline.draw();
    glEnable(GL_LIGHTING);
    glPopAttrib();

    m_spTransform->unbind();

    //Draw cut paths
    //if(m_isToolActive)
    {
    	glDisable(GL_LIGHTING);
		glPushAttrib(GL_ALL_ATTRIB_BITS);
			glLineWidth(1.0f);
			glColor3d(0.0, 1.0, 0.0);
			glBegin(GL_LINES);
			for(U32 i=0; i< m_vCuttingPathEdge0.size(); i+=8) {
				glVertex3dv(m_vCuttingPathEdge0[i].cptr());
				glVertex3dv(m_vCuttingPathEdge1[i].cptr());
			}
			glEnd();

			glColor3d(0.0, 0.5, 1.0);
			glBegin(GL_LINES);
			for(U32 i=0; i< m_vCuttingPathEdge0.size(); i++)
				glVertex3dv(m_vCuttingPathEdge0[i].cptr());
			glEnd();
		glPopAttrib();

		//Draw Swept Quad
		if(m_isSweptQuadValid)
		{
			glPushAttrib(GL_ALL_ATTRIB_BITS);
				glDisable(GL_CULL_FACE);
				glEnable(GL_BLEND);
				glBlendFunc(GL_SRC_ALPHA,GL_ONE);

				glColor4d(1.0, 0.0, 1.0, 0.5);
				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
				glBegin(GL_QUADS);
					glVertex3dv(m_sweptQuad[0].cptr());
					glVertex3dv(m_sweptQuad[1].cptr());
					glVertex3dv(m_sweptQuad[2].cptr());
					glVertex3dv(m_sweptQuad[3].cptr());
				glEnd();
				//glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

				glDisable(GL_BLEND);
				glEnable(GL_CULL_FACE);
			glPopAttrib();
		}

		glEnable(GL_LIGHTING);
    }
}

void AvatarScalpel::clearCutContext() {
	m_vCuttingPathEdge0.resize(0);
	m_vCuttingPathEdge1.resize(0);
	m_isSweptQuadValid = false;

	if(m_lpTissue)
		m_lpTissue->clearCutContext();
}

//From Gizmo Manager
void AvatarScalpel::mousePress(int button, int state, int x, int y) {
	if(button == ArcBallCamera::mbRight) {
		LogInfo("Right clicked cleared cut context!");
		clearCutContext();
		return;
	}

	if(button != ArcBallCamera::mbLeft)
		return;

	//Down = Start
	if(state == 0) {
		if(m_lpTissue) {
			m_isToolActive = true;
			TheSceneGraph::Instance().headers()->updateHeaderLine("scalpel", "scalpel: start cutting");
		}
	}
	else {
		//Up = Stop
		if (m_lpTissue) {
			m_isToolActive = false;
			TheSceneGraph::Instance().headers()->updateHeaderLine("scalpel", "scalpel: end cutting");
		}
	}
}

void AvatarScalpel::onTranslate(const vec3f& delta, const vec3f& pos) {
	if(m_lpTissue == NULL || !m_isToolActive)
		return;

	//Box test
	m_aabbCurrent = this->aabb();
	m_aabbCurrent.transform(m_spTransform->forward());

	//1.If boxes donot intersect and sweptquad is invalid then return
	if (!m_lpTissue->aabb().intersect(m_aabbCurrent)) {

		if(m_isSweptQuadValid) {
			//call the cut method if the tool has passed through the tissue
			int res = m_lpTissue->cut(m_vCuttingPathEdge0, m_vCuttingPathEdge1, m_sweptQuad, true);
			clearCutContext();
			LogInfoArg1("Tissue cut. res = %d", res);
		}

		return;
	}

	//consts
	const double minSweptLength = 0.4;
	const U32 maxNodes = 1024;

	//edges
	vec3f e0 = m_spTransform->forward().map(m_edgeref0);
	vec3f e1 = m_spTransform->forward().map(m_edgeref1);
	vec3d edge0 = vec3d(e0.x, e0.y, e0.z);
	vec3d edge1 = vec3d(e1.x, e1.y, e1.z);

	//delete path if direction changed
	if(m_vCuttingPathEdge0.size() > 8) {

		//U32 len = m_vCuttingPathEdge0.size();
		vec3d o = m_vCuttingPathEdge0[0];
		vec3d prevDir = (m_vCuttingPathEdge0[4] - o).normalized();
		vec3d dir;
		double angle = 0;
		double maxAngle = 0;

		for(U32 i=4; i < m_vCuttingPathEdge0.size(); i+=4) {
			dir = (m_vCuttingPathEdge0[i] - o).normalized();
			angle = RADTODEG(abs(acos(vec3d::dot(dir, prevDir))));
			maxAngle = MATHMAX(maxAngle, angle);

			o = m_vCuttingPathEdge0[i];
			prevDir = dir;
		}

		if(maxAngle > 60) {
			m_vCuttingPathEdge0.resize(0);
			m_vCuttingPathEdge1.resize(1);
			m_isSweptQuadValid = false;
			LogInfoArg1("Cutting trajectory changed %.2f degrees. Resetting path.", maxAngle);
		}
	}


	//Swept quad: starts when blade crosses the tissue first and ends where the blade leaves the body
	m_isSweptQuadValid = false;
	if(m_vCuttingPathEdge0.size() == 0) {
		m_sweptQuad[0] = edge0;
		m_sweptQuad[1] = edge1;
	}
	else {
		m_sweptQuad[2] = edge1;
		m_sweptQuad[3] = edge0;
		m_isSweptQuadValid = true;
	}


	/*
	if (m_vCuttingPathEdge0.size() > 1) {
		//Loop over the path from the recently added to the first one
		for (int i = (int) m_vCuttingPathEdge0.size() - 1; i >= 0; i--) {
			double d = vec3d::distance(m_vCuttingPathEdge0[i], edge0);
			if (d >= minSweptLength) {
				m_sweptQuad[2] = m_vCuttingPathEdge1[i];
				m_sweptQuad[3] = m_vCuttingPathEdge0[i];
				m_isSweptQuadValid = true;


				//printf("swept quad is valid\n");
				break;
			}
		}
	}
	*/

	//Insert new scalpal position into buffer
	m_vCuttingPathEdge0.push_back(edge0);
	m_vCuttingPathEdge1.push_back(edge1);


	//delete last if overflow buffer
	if (m_vCuttingPathEdge0.size() > maxNodes)
		m_vCuttingPathEdge0.erase(m_vCuttingPathEdge0.begin());
	if (m_vCuttingPathEdge1.size() > maxNodes)
		m_vCuttingPathEdge1.erase(m_vCuttingPathEdge1.begin());

	int res = m_lpTissue->cut(m_vCuttingPathEdge0, m_vCuttingPathEdge1, m_sweptQuad, false);
	LogInfoArg1("Progressive cutting not implemented. res = %d", res);
}



