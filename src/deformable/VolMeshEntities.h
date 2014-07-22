/*
 * CellularMeshTypes.h
 *
 *  Created on: Jul 17, 2014
 *      Author: pourya
 */

#ifndef CELLULARMESHTYPES_H_
#define CELLULARMESHTYPES_H_

#include "base/Vec.h"

using namespace PS;
using namespace PS::MATH;

#define FACE_SHIFT_A 0
#define FACE_SHIFT_B 21
#define FACE_SHIFT_C 42

#define HEDGE_SHIFT_A 0
#define HEDGE_SHIFT_B 32
#define DEFAULT_FACE_SIDES 3


//can keep up to 2097151 nodes
#define FACE_BITMASK 0x001FFFFF
#define HEDGE_BITMASK 0xFFFFFFFF

#define FACEID_HASHSIZE (U64)(1<<(3*FACE_SHIFT_B))
#define FACEID_FROM_IDX(a,b,c) (((U64) ((c) & FACE_BITMASK) << FACE_SHIFT_C) | ((U64) ((b) & FACE_BITMASK) << FACE_SHIFT_B) | ((a) & FACE_BITMASK))

#define HEDGEID_HASHSIZE	(U64)(1<<(HEDGE_SHIFT_B))
#define HEDGEID_FROM_IDX(a,b)	(((U64)((b) & HEDGE_BITMASK) << HEDGE_SHIFT_B) | ((a) & HEDGE_BITMASK))

namespace PS {
namespace MESH {

	//BaseHandle
	class BaseHandle {
	public:
		static const U32 INVALID = -1;
		explicit BaseHandle(U32 idx) { m_idx = idx;}
		explicit BaseHandle(const BaseHandle& other) { m_idx = other.m_idx;}


		bool isValid() const { return (m_idx != INVALID);}

		//ops
		BaseHandle& operator=(const BaseHandle& other) {
			this->m_idx = other.m_idx;
			return (*this);
		}

		BaseHandle& operator=(U32 idx) {
			this->m_idx = idx;
			return (*this);
		}

		bool operator<(const BaseHandle& other) const { return this->m_idx < other.m_idx; }
		bool operator>(const BaseHandle& other) const { return this->m_idx > other.m_idx; }
		bool operator==(const BaseHandle& other) const { return this->m_idx == other.m_idx; }
		bool operator<(U32 idx) const { return this->m_idx < idx; }
		bool operator>(U32 idx) const { return this->m_idx > idx; }
		bool operator==(U32 idx) const { return this->m_idx == idx; }

		inline operator U32() const {return m_idx;}

	protected:
		U32 m_idx;
	};

	//handles for all entities
	class NodeHandle : public BaseHandle { NodeHandle(U32 idx = INVALID): BaseHandle(idx){} };
	class HEDGEHandle : public BaseHandle { HEDGEHandle(U32 idx = INVALID): BaseHandle(idx){} };
	class EDGEHandle : public BaseHandle { EDGEHandle(U32 idx = INVALID): BaseHandle(idx){} };
	class FACEHandle : public BaseHandle { FACEHandle(U32 idx = INVALID): BaseHandle(idx){} };
	class HFACEHandle : public BaseHandle { HFACEHandle(U32 idx = INVALID): BaseHandle(idx){} };
	class CELLHandle : public BaseHandle { CELLHandle(U32 idx = INVALID): BaseHandle(idx){} };

	//vertices
	class NODE {
	public:
		vec3d pos;
		vec3d restpos;
		U32 outHE;

		NODE& operator = (const NODE& A) {
			pos = A.pos;
			restpos = A.restpos;
			outHE = A.outHE;
			return (*this);
		}
	};

	//half-edge
	class HEDGE{
	public:
		//Vertex
		U32 from, to;

		//Face
		U32 face;

		//HEDGES
		U32 prev, next, opposite;

		//number of references
		U8 refs;


		HEDGE() { init();}
		HEDGE(U32 from_, U32 to_) {
			init();
			from = from_;
			to = to_;
		};

		void init() {
			from = to = face =  BaseHandle::INVALID;
			prev = next = opposite = BaseHandle::INVALID;
			refs = 0;
		}

		HEDGE& operator = (const HEDGE& A) {

			from = A.from;
			to = A.to;
			face = A.face;
			prev = A.prev;
			next = A.next;
			opposite = A.opposite;
			refs = A.refs;

			return(*this);
		};
	};

	//Key to access half-edges in a unique order
	class HEdgeKey
	{
	public:
		HEdgeKey(U64 k) { this->key = k; }
		HEdgeKey(U32 a, U32 b) {
	    	key = HEDGEID_FROM_IDX(a, b);
	    }

	    bool operator<(const HEdgeKey& k) const { return key < k.key; }

	    bool operator>(const HEdgeKey& k) const { return key > k.key; }

	    void operator=(const HEdgeKey& other) {
	    	this->key = other.key;
	    }

	    U64 key;
	};


	//edge
	class EDGE {
	public:
		U32 from, to;

		EDGE() { init();}
		EDGE(const HEDGE& e1, const HEDGE& e2) {
			setup(e1, e2);
		}

		void init() {
			from = to = BaseHandle::INVALID;
		}

		void setup(const HEDGE& e1, const HEDGE& e2) {
			assert(e1.from == e2.to && e1.to == e2.from);
			from = e1.from;
			to = e1.to;
		}

		EDGE& operator =(const EDGE& A) {
			from = A.from;
			to = A.to;
			return (*this);
		}
	};

	//face
	class FACE {
	public:
		U32 halfedge[DEFAULT_FACE_SIDES];
		U8 refs;
		bool removed;

		FACE() {
			init();
		}

		void init() {
			for(int i=0; i<DEFAULT_FACE_SIDES; i++)
				halfedge[i] = BaseHandle::INVALID;
			refs = 0;
			removed = false;
		}

		FACE& operator = (const FACE& A) {
			for(int i=0; i<DEFAULT_FACE_SIDES; i++)
				halfedge[i] = A.halfedge[i];
			refs = A.refs;
			removed = A.removed;
			return (*this);
		}
	};

	//Key to access faces in a unique order
	class FaceKey
	{
	public:
		FaceKey(U64 k) { this->key = k; }

		FaceKey(U32 n[3]) {
	    	order_lo2hi(n[0], n[1], n[2]);
	    	key = FACEID_FROM_IDX(n[0], n[1], n[2]);
		}

	    FaceKey(U32 a, U32 b, U32 c) {
	    	order_lo2hi(a, b, c);
	    	key = FACEID_FROM_IDX(a, b, c);
	    }

	    static void order_lo2hi(U32& a, U32& b, U32& c) {
	    	if(a > b)
	    		swap(a, b);
	    	if(b > c)
	    		swap(b, c);
	    	if(a > b)
	    		swap(a, b);
	    }

	    bool operator<(const FaceKey& k) const { return key < k.key; }

	    bool operator>(const FaceKey& k) const { return key > k.key; }

	    void operator=(const FaceKey& other) {
	    	this->key = other.key;
	    }


	    U64 key;
	};


	//elements
	class CELL {
	public:
		U32 faces[4];
		U32 nodes[4];
		U32 halfedge[6];
		bool posDet;
		bool removed;

		CELL() {
			init();
		}

		void init() {
			for(int i=0; i<4; i++) {
				faces[i] = BaseHandle::INVALID;
				nodes[i] = BaseHandle::INVALID;
			}
			for(int i=0; i<6; i++)
				halfedge[i] = BaseHandle::INVALID;
			posDet = false;
			removed = false;
		}

		CELL& operator = (const CELL& A) {
			for(int i=0; i<4; i++) {
				faces[i] = A.faces[i];
				nodes[i] = A.nodes[i];
			}
			for(int i=0; i<6; i++)
				halfedge[i] = A.halfedge[i];
			posDet = A.posDet;
			removed = A.removed;
			return (*this);
		}
	};

}
}


#endif /* CELLULARMESHTYPES_H_ */
