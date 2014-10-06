/*
 * SGBulletSoftMesh.cpp
 *
 *  Created on: Sep 7, 2014
 *      Author: pourya
 */

#include "SGBulletSoftMesh.h"

using namespace PS::SG;

SGBulletSoftMesh::SGBulletSoftMesh() {
	init();
}

SGBulletSoftMesh::SGBulletSoftMesh(btSoftBody* aSoftBody) {
	init();
	setup(aSoftBody);
}

SGBulletSoftMesh::~SGBulletSoftMesh() {
	cleanup();
}


void SGBulletSoftMesh::setup(const btSoftBody* pBody, float mass) {
	m_lpSoftBody = const_cast<btSoftBody*>(pBody);
}

void SGBulletSoftMesh::init() {
	m_lpSoftBody = NULL;
}

void SGBulletSoftMesh::cleanup() {

}

void SGBulletSoftMesh::draw() {
	SGMesh::draw();
}

void SGBulletSoftMesh::timestep() {
	//sync
}





