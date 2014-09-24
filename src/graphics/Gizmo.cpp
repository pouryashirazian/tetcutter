/*
 * Gizmo.cpp
 *
 *  Created on: Dec 28, 2013
 *      Author: pourya
 */
#include "Gizmo.h"
#include "selectgl.h"
#include "ShaderManager.h"
#include "base/Logger.h"
#include "SceneGraph.h"

#define DEFAULT_AXIS_LENGTH 2.0f
#define DEFAULT_AXIS_THICKNESS 0.05f
#define GIZMO_SCALE_FACTOR 0.005f
#define GIZMO_ROTATION_FACTOR 10.0f

using namespace PS::GL;

namespace PS {
    namespace SG {

        //The Effect for Gizmos
        class GizmoEffect : public SGEffect {
        public:
            GizmoEffect():m_idColor(-1) {
            }
            
            GizmoEffect(GLShader* s):SGEffect(s) {
                m_idColor = m_lpShader->getUniformLocation("color");
            }
            
            void setColor(const vec4f& color) {
                m_color = color;
                if(m_lpShader->isRunning())
                    m_lpShader->setUniform(m_idColor, m_color);
            }
            
            void bind() {
                SGEffect::bind();
                m_lpShader->setUniform(m_idColor, m_color);
            }
            
        private:
            vec4f m_color;
            int m_idColor;
            
        };

        //////////////////////////////////////////////////////////
    	IGizmoListener::IGizmoListener():IMouseListener(), m_id(-1)  {
    		assert(registerListener());
    	}

    	IGizmoListener::~IGizmoListener() {
    		unregisterListener();
    	}

    	bool IGizmoListener::registerListener() {
    		m_id = TheGizmoManager::Instance().registerClient(this);
    		return (m_id >= 0);
    	}

    	void IGizmoListener::unregisterListener() {
    		TheGizmoManager::Instance().unregisterClient(m_id);
    	}
        //////////////////////////////////////////////////////////
        int GizmoInterface::setAxis(const PS::MATH::Ray &r) {
            //1 = x, 2 = y, 3 = z
        	int res = this->intersect(r);
            if(res > 0)
                this->setAxis((GizmoAxis)(res-1));
            else
            	this->setAxis(axisFree);

            //Set selected
            this->setSelected(res > 0);
            return res;
        }
        
        vec4f GizmoInterface::axisColor(GizmoAxis a) {
            if(a == m_axis)
                return vec4(0.7, 0.7, 0.7, 1.0);
            if(a == axisX)
                return vec4(1,0,0,1);
            else if(a == axisY)
                return vec4(0,1,0,1);
            else if(a == axisZ)
                return vec4(0,0,1,1);
            else
                return vec4(0.7, 0.7, 0.7, 1.0);
        }
      
        ///////////////////////////////////////////////////////////
        //The Translation Gizmo
        GizmoTranslate::GizmoTranslate(): GizmoInterface() {
            this->setName("GizmoTranslate");
            setup();
        }
        
        void GizmoTranslate::setup() {
            
            Geometry x, y, z;
            x.init(3, 4, 2, ftTriangles);
            x.addCircle3D(16, DEFAULT_AXIS_THICKNESS);
            x.extrude(vec3f(DEFAULT_AXIS_LENGTH, 0, 0));
            x.addCone(16, 0.2f, 0.4f, vec3f(DEFAULT_AXIS_LENGTH, 0, 0));
            x.computeNormalsFromFaces();
            y = x;
            quat q1;
            q1.fromAxisAngle(vec3f(0, 0, 1), 90.0f);
            y.transform(mat44f::quatToMatrix(q1));
            
            z = x;
            quat q2;
            q2.fromAxisAngle(vec3f(0, 1, 0), 90.0f);
            z.transform(mat44f::quatToMatrix(q2));
            
            x.addPerVertexColor(vec4f(1, 0, 0, 1), x.countVertices());
            y.addPerVertexColor(vec4f(0, 1, 0, 1), x.countVertices());
            z.addPerVertexColor(vec4f(0, 0, 1, 1), x.countVertices());
            
            
            //Setup
            m_x.setup(x);
            m_y.setup(y);
            m_z.setup(z);
            
            Geometry total = x + y + z;
            GLMeshBuffer::setup(total);
            
            if(TheShaderManager::Instance().has("gizmo")) {
                m_spEffect = SmartPtrSGEffect(new GizmoEffect(TheShaderManager::Instance().get("gizmo")));
            }
            
            this->setAxis(axisX);
        }
        
        void GizmoTranslate::draw() {
            glClear(GL_DEPTH_BUFFER_BIT);

            m_spTransform->bind();
            GizmoEffect* peff = dynamic_cast<GizmoEffect*>(m_spEffect.get());
            peff->bind();
            
            //X
            peff->setColor(axisColor(axisX));
            m_x.drawNoEffect();
            
            //Y
            peff->setColor(axisColor(axisY));
            m_y.drawNoEffect();
            
            //Z
            peff->setColor(axisColor(axisZ));
            m_z.drawNoEffect();
            
            m_spEffect->unbind();
            m_spTransform->unbind();
        }
        
        
        int GizmoTranslate::intersect(const PS::MATH::Ray &r) {
            if(m_x.intersect(r))
                return 1;
            else if(m_y.intersect(r))
                return 2;
            else if(m_z.intersect(r))
                return 3;
            return -1;
        }
        //////////////////////////////////////////////////////////
        GizmoScale::GizmoScale(): GizmoInterface() {
            this->setName("GizmoScale");
            setup();
        }
        
        void GizmoScale::setup() {
            Geometry x, y, z;
            x.init(3, 4, 2, ftTriangles);
            x.addCircle3D(16, DEFAULT_AXIS_THICKNESS);
            
            vec3f c = vec3f(DEFAULT_AXIS_LENGTH, 0, 0);
            x.extrude(c);
            x.computeNormalsFromFaces();
            x.addCube(c - vec3f(0.15f), c + vec3f(0.15f));
            
            
            y = x;
            quat q1;
            q1.fromAxisAngle(vec3f(0, 0, 1), 90.0f);
            y.transform(mat44f::quatToMatrix(q1));
            
            z = x;
            quat q2;
            q2.fromAxisAngle(vec3f(0, 1, 0), 90.0f);
            z.transform(mat44f::quatToMatrix(q2));
            
            x.addPerVertexColor(vec4f(1, 0, 0, 1), x.countVertices());
            y.addPerVertexColor(vec4f(0, 1, 0, 1), x.countVertices());
            z.addPerVertexColor(vec4f(0, 0, 1, 1), x.countVertices());
            
            
            //Setup
            m_x.setup(x);
            m_y.setup(y);
            m_z.setup(z);
            
            Geometry total = x + y + z;
            GLMeshBuffer::setup(total);
            
            if(TheShaderManager::Instance().has("gizmo")) {
                m_spEffect = SmartPtrSGEffect(new GizmoEffect(TheShaderManager::Instance().get("gizmo")));
            }
            
            this->setAxis(axisX);
        }
        
        void GizmoScale::draw() {
            glClear(GL_DEPTH_BUFFER_BIT);
            m_spTransform->bind();
            GizmoEffect* peff = dynamic_cast<GizmoEffect*>(m_spEffect.get());
            peff->bind();
            
            //X
            peff->setColor(axisColor(axisX));
            m_x.drawNoEffect();
            
            //Y
            peff->setColor(axisColor(axisY));
            m_y.drawNoEffect();
            
            //Z
            peff->setColor(axisColor(axisZ));
            m_z.drawNoEffect();
            
            m_spEffect->unbind();
            m_spTransform->unbind();
        }
        
        int GizmoScale::intersect(const PS::MATH::Ray &r) {
            if(m_x.intersect(r))
                return 1;
            else if(m_y.intersect(r))
                return 2;
            else if(m_z.intersect(r))
                return 3;
            return -1;
        }

        //////////////////////////////////////////////////////////
        GizmoRotate::GizmoRotate(): GizmoInterface() {
            this->setName("GizmoRotate");
            setup();
        }
        
        void GizmoRotate::setup() {
            Geometry x, y, z;
            x.init(3, 4, 2, ftTriangles);

            Geometry xs, ys, zs;
            xs.init(3, 4, 2, ftLines);

            int sectors = 32;
        	float oneOverSector = 1.0f / static_cast<float>(sectors);
        	float radius = 1.0f;
        	float thickness = 0.2;
        	float halfThickness = 0.5 * thickness;
        	vec3f origin(0,0,0);
        	for(int i=0; i<sectors; i++) {

        		float vcos = cos(i * (TwoPi * oneOverSector));
        		float vsin = sin(i * (TwoPi * oneOverSector));
        		vec3f v1 = origin + vec3f::mul(radius, vec3f(-halfThickness, vcos, vsin));
        		vec3f v2 = origin + vec3f::mul(radius, vec3f(halfThickness, vcos, vsin));

        		//surface
        		x.addVertex(v1);
        		x.addVertex(v2);

        		//segments
        		xs.addVertex(v1);
        		xs.addVertex(v2);

        		if(i > 0) {
        			int base = (i - 1) * 2;
        			x.addTriangle(vec3u32(base + 0, base + 1, base + 2));
        			x.addTriangle(vec3u32(base + 2, base + 1, base + 3));
        		}
        	}

        	//add triangles
        	int lastQuad[4];
        	lastQuad[0] = (sectors - 1) * 2;
        	lastQuad[1] = (sectors - 1) * 2 + 1;
        	lastQuad[2] = 0;
        	lastQuad[3] = 1;
			x.addTriangle(vec3u32(lastQuad[0], lastQuad[1], lastQuad[2]));
			x.addTriangle(vec3u32(lastQuad[2], lastQuad[1], lastQuad[3]));




			//y
            quat quatY;
            quatY.fromAxisAngle(vec3f(0, 0, 1), 90.0f);
			y = x;
            y.transform(mat44f::quatToMatrix(quatY));
            ys = xs;
            ys.transform(mat44f::quatToMatrix(quatY));

            //z
            quat quatZ;
            quatZ.fromAxisAngle(vec3f(0, 1, 0), 90.0f);
            z = x;
            z.transform(mat44f::quatToMatrix(quatZ));
            zs = xs;
            zs.transform(mat44f::quatToMatrix(quatZ));

            
            x.addPerVertexColor(vec4f(1, 0, 0, 0.5), x.countVertices());
            y.addPerVertexColor(vec4f(0, 1, 0, 0.5), y.countVertices());
            z.addPerVertexColor(vec4f(0, 0, 1, 0.5), z.countVertices());

            xs.addPerVertexColor(vec4f(0, 0, 0, 1), xs.countVertices());
            ys.addPerVertexColor(vec4f(0, 0, 0, 1), ys.countVertices());
            zs.addPerVertexColor(vec4f(0, 0, 0, 1), zs.countVertices());
            
            
            //Setup
            m_x.setup(x);
            m_y.setup(y);
            m_z.setup(z);
            m_xs.setup(xs);
            m_ys.setup(ys);
            m_zs.setup(zs);
            
            if(TheShaderManager::Instance().has("gizmo")) {
                m_spEffect = SmartPtrSGEffect(new GizmoEffect(TheShaderManager::Instance().get("gizmo")));
            }
            
            GizmoInterface::setAxis(axisX);
        }
        
        void GizmoRotate::draw() {
            glClear(GL_DEPTH_BUFFER_BIT);
            glDisable(GL_CULL_FACE);

            m_spTransform->bind();
            GizmoEffect* peff = dynamic_cast<GizmoEffect*>(m_spEffect.get());
            peff->bind();
            

            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            glEnable(GL_BLEND);
            //X
            peff->setColor(axisColor(axisX));
            m_x.drawNoEffect();
            m_xs.drawNoEffect();
            
            //Y
            peff->setColor(axisColor(axisY));
            m_y.drawNoEffect();
            m_ys.drawNoEffect();
            
            //Z
            peff->setColor(axisColor(axisZ));
            m_z.drawNoEffect();
            m_zs.drawNoEffect();
            
            m_spEffect->unbind();
            m_spTransform->unbind();

            glDisable(GL_BLEND);
            glEnable(GL_CULL_FACE);
        }
        
        int GizmoRotate::intersect(const PS::MATH::Ray &r) {
            if(m_x.intersect(r))
                return 1;
            else if(m_y.intersect(r))
                return 2;
            else if(m_z.intersect(r))
                return 3;
            return -1;
        }

        int GizmoRotate::setAxis(const PS::MATH::Ray &r) {
            //1 = x, 2 = y, 3 = z
        	int res = this->intersect(r);
            if(res > 0)
                GizmoInterface::setAxis((GizmoAxis)(res-1));

            //Set selected
            this->setSelected(res > 0);
            return res;
        }


        void GizmoRotate::rotate(GizmoAxis axis, float degree) {

        	if(axis == axisX) {
        		m_x.transform()->rotate(vec3f(1, 0, 0), degree);
        		m_xs.transform()->rotate(vec3f(1, 0, 0), degree);
        	}
        	else if(axis == axisY) {
        		m_y.transform()->rotate(vec3f(0, 1, 0), degree);
        		m_ys.transform()->rotate(vec3f(0, 1, 0), degree);
        	}
        	else if(axis == axisZ) {
        		m_z.transform()->rotate(vec3f(0, 0, 1), degree);
        		m_zs.transform()->rotate(vec3f(0, 0, 1), degree);
        	}
        }

        
        //////////////////////////////////////////////////////////
        GizmoAvatar::GizmoAvatar(): GizmoInterface() {
            this->setName("GizmoAvatar");
            setup();
        }
        
        void GizmoAvatar::setup() {
            
        }
        ////////////////////////////////////////////////////////////
        GizmoManager::GizmoManager():SGNode() {
        	m_lpFocusedNode = NULL;
            m_buttonState = ArcBallCamera::bsUp;
            m_pressedPos = vec2i(0, 0);

            //Create All needed Gizmo Widgets
            m_lpGizmoTranslate = new GizmoTranslate();
            m_lpGizmoScale = new GizmoScale();
            m_lpGizmoRotate = new GizmoRotate();
            m_lpGizmoAvatar = new GizmoAvatar();
            
            m_gizmoType = gtTranslate;
            m_gizmoAxis = axisFree;
            m_lpGizmoCurrent = m_lpGizmoTranslate;
            TheSceneGraph::Instance().headers()->addHeaderLine("gizmo", "");

            setFocusedNode(NULL);

            this->resetTransform();
        }
        
        GizmoManager::~GizmoManager() {
            SAFE_DELETE(m_lpGizmoTranslate);
            SAFE_DELETE(m_lpGizmoScale);
            SAFE_DELETE(m_lpGizmoRotate);
            SAFE_DELETE(m_lpGizmoAvatar);
            m_clients.resize(0);
            m_lpFocusedNode = NULL;
        }
        
        void GizmoManager::draw() {
            if(m_lpGizmoCurrent && isVisible()) {
            	glPushMatrix();
            	vec3f t = this->transform()->getTranslate();
            	glTranslatef(t.x, t.y, t.z);

                m_lpGizmoCurrent->draw();

                glPopMatrix();
            }
        }
        
        void GizmoManager::timestep() {

        }

        int GizmoManager::intersect(const Ray& r) {
            if(m_lpGizmoCurrent)
                return m_lpGizmoCurrent->intersect(r);
            else            
                return -1;
        }
        
        void GizmoManager::setAxis(const Ray& r) {
            if(m_lpGizmoCurrent) {
                if(m_lpGizmoCurrent->setAxis(r)) {
                	m_gizmoAxis = m_lpGizmoCurrent->axis();
                	LogInfoArg1("Set selected axis to: %d", m_gizmoAxis);
                }

                //Selection
                this->setSelected(m_lpGizmoCurrent->isSelected());
            }
        }
        
        void GizmoManager::setAxis(GizmoAxis axis) {
        	m_gizmoAxis = axis;
            if(m_lpGizmoCurrent) {
            	m_lpGizmoCurrent->setAxis(axis);
            }
        }
        
        void GizmoManager::setType(GizmoType gtype) {
            m_gizmoType = gtype;
            switch (gtype) {
                case gtTranslate:
                    m_lpGizmoCurrent = m_lpGizmoTranslate;
                    break;
                case gtScale:
                    m_lpGizmoCurrent = m_lpGizmoScale;
                    break;
                case gtRotate:
                    m_lpGizmoCurrent = m_lpGizmoRotate;
                    break;
                case gtAvatar:
                    m_lpGizmoCurrent = m_lpGizmoAvatar;
                    break;
                    
                default:
                    break;
            }
            
            //Apply the selected axis to this Gizmo
            m_lpGizmoCurrent->setAxis(m_gizmoAxis);
            glutPostRedisplay();
        }
        
        void GizmoManager::mousePress(int button, int state, int x, int y) {
            m_button = (ArcBallCamera::MouseButton)button;
            m_buttonState = (ArcBallCamera::ButtonState)state;
            if(m_button == ArcBallCamera::mbLeft && m_buttonState == ArcBallCamera::bsDown && isVisible()) {
            	LogInfoArg2("Select gizmo axis using mouse coords: [%d, %d]", x, y);
                Ray r = TheSceneGraph::Instance().screenToWorldRay(x, y);
                r.setStart(r.getStart() - this->transform()->getTranslate());

                //set axis using ray shooting
                setAxis(r);
            }

            m_pressedPos = vec2i(x, y);

        	//Post Messages
        	for(U32 i=0; i<m_clients.size(); i++)
        		m_clients[i]->mousePress(button, state, x, y);
        }

        void GizmoManager::mouseMove(int x, int y) {

        	if(m_button != ArcBallCamera::mbLeft)
        		return;
        	if(m_buttonState != ArcBallCamera::bsDown)
        		return;

        	//handle the case where the gizmo is not visible and we try to move
        	if(!isVisible() && m_gizmoType != gtTranslate) {
        		LogInfo("Reset the gizmo to translate mode since it became invisible");
        		setType(gtTranslate);
        	}

        	vec3f gizmoAxis[4] = {vec3f(1,0,0), vec3f(0,1,0), vec3f(0,0,1), vec3f(1,1,1)};
        	Ray r1 = TheSceneGraph::Instance().screenToWorldRay(m_pressedPos.x, m_pressedPos.y);
        	Ray r2 = TheSceneGraph::Instance().screenToWorldRay(x, y);
        	vec3f delta = vec3f::mul((r2.start - r1.start), gizmoAxis[m_gizmoAxis]) * 1000;
        	m_pressedPos = vec2i(x, y);

        	//apply to gizmo type
            switch (m_gizmoType) {
                case gtTranslate: {
                	cmdTranslate(delta);
                }
                break;
                case gtScale: {
                	cmdScale(delta);
                }
                break;
                case gtRotate: {
        			vec3f axis = gizmoAxis[m_gizmoAxis];
        			float degree = delta[m_gizmoAxis] * GIZMO_ROTATION_FACTOR;
        			cmdRotate(axis, degree);
                }
                break;

                case gtAvatar: {

                }
                break;

                case gtCount: {

                }
                break;
            };

        	for(U32 i=0; i<m_clients.size(); i++)
        		m_clients[i]->mouseMove(x, y);
        }

        string GizmoManager::AxisToStr(GizmoAxis axis) {
        	string strAxis;
        	switch (axis) {
			case axisX:
				strAxis = "X";
				break;
			case axisY:
				strAxis = "Y";
				break;
			case axisZ:
				strAxis = "Z";
				break;
			case axisFree:
				strAxis = "FREE";
				break;

			case axisCount: {

			}
				break;
			}

        	return strAxis;
        }

    	void GizmoManager::cmdTranslate(const vec3f& increment) {
        	transform()->translate(increment);
        	vec3f final = transform()->getTranslate();

            if(m_lpFocusedNode)
            	m_lpFocusedNode->transform()->translate(increment);

        	//Post Messages
        	for(U32 i=0; i<m_clients.size(); i++)
        		m_clients[i]->onTranslate(increment, final);

        	//push to header
        	char buffer[512];
        	sprintf(buffer,
        			"translate increment = (%.3f, %.3f, %.3f), final = (%.3f, %0.3f, %.3f)",
        			increment.x, increment.y, increment.z, final.x, final.y, final.z);
        	TheSceneGraph::Instance().headers()->updateHeaderLine("gizmo", buffer);
    	}

		void GizmoManager::cmdScale(const vec3f& increment) {
			this->transform()->scale(increment);
			vec3f final = transform()->getScale();

			if (m_lpFocusedNode)
				m_lpFocusedNode->transform()->scale(increment);

			//Post Messages
			for (U32 i = 0; i < m_clients.size(); i++)
				m_clients[i]->onScale(increment, final);

        	//push to header
        	char buffer[512];
			sprintf(buffer,
					"scale increment = (%.3f, %.3f, %.3f), final = (%.3f, %0.3f, %.3f)",
					increment.x, increment.y, increment.z, final.x, final.y, final.z);
			TheSceneGraph::Instance().headers()->updateHeaderLine("gizmo", buffer);
		}

		void GizmoManager::cmdRotate(const vec3f& axis, float degreeIncrement) {
			quatf quatIncrement;
			quatIncrement.fromAxisAngle(axis, degreeIncrement);

			if (m_lpFocusedNode)
				m_lpFocusedNode->transform()->rotate(quatIncrement);

			//rotate gizmo
			if (m_lpGizmoRotate)
				m_lpGizmoRotate->rotate(m_gizmoAxis, degreeIncrement);

			//Post Messages
			for (U32 i = 0; i < m_clients.size(); i++)
				m_clients[i]->onRotate(quatIncrement);

        	//push to header
        	char buffer[512];
			sprintf(buffer, "rotate axis=(%.3f, %.3f, %.3f), angle=(%.3f)",
					axis.x, axis.y, axis.z, degreeIncrement);
			TheSceneGraph::Instance().headers()->updateHeaderLine("gizmo", buffer);
		}


        //MouseWheel
        void GizmoManager::mouseWheel(int button, int dir, int x, int y) {
        	for(U32 i=0; i<m_clients.size(); i++)
        		m_clients[i]->mouseWheel(button, dir, x, y);
        }

        void GizmoManager::setFocusedNode(SGNode* node) {
        	if(node == NULL) {
        		this->transform()->reset();
        		return;
        	}

        	m_lpFocusedNode = node;
        	this->transform()->copyFrom(*m_lpFocusedNode->transform().get());

        	glutPostRedisplay();
        }

    	int GizmoManager::registerClient(IGizmoListener* client) {
    		for(U32 i=0; i<m_clients.size(); i++)
    			if(m_clients[i] == client) {
    				LogError("Client already registered!");
    				return -1;
    			}

    		m_clients.push_back(client);
    		return (m_clients.size() - 1);
    	}

    	void GizmoManager::unregisterClient(int id) {
    		if(id < (int)m_clients.size())
    			m_clients.erase(m_clients.begin() + id);
    	}


    }
}


