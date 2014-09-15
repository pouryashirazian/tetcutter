/*
 * CellularMeshTypes.h
 *
 *  Created on: Jul 17, 2014
 *      Author: pourya
 */

#ifndef CELLULARMESHTYPES_H_
#define CELLULARMESHTYPES_H_

#include <algorithm>
#include <functional>
#include "base/Vec.h"

using namespace PS;
using namespace PS::MATH;

#define FACE_SHIFT_A 0
#define FACE_SHIFT_B 21
#define FACE_SHIFT_C 42

#define HEDGE_SHIFT_A 0
#define HEDGE_SHIFT_B 32

#define COUNT_FACE_EDGES 3
#define COUNT_CELL_EDGES 6
#define COUNT_CELL_FACES 4
#define COUNT_CELL_NODES 4

//can keep up to 2097151 nodes
#define FACE_BITMASK 0x001FFFFF
#define HEDGE_BITMASK 0xFFFFFFFF

#define FACEID_HASHSIZE (U64)(1<<(3*FACE_SHIFT_B))
#define FACEID_FROM_IDX(a,b,c) (((U64) ((c) & FACE_BITMASK) << FACE_SHIFT_C) | ((U64) ((b) & FACE_BITMASK) << FACE_SHIFT_B) | ((a) & FACE_BITMASK))

#define HEDGEID_HASHSIZE	(U64)(1<<(HEDGE_SHIFT_B))
#define HEDGEID_FROM_IDX(a,b)	(((U64)((b) & HEDGE_BITMASK) << HEDGE_SHIFT_B) | ((a) & HEDGE_BITMASK))

namespace PS {
namespace MESH {

	//Base link
	class BaseLink {
	public:
		static const U32 INVALID = -1;
		explicit BaseLink(U32 idx) { m_idx = idx;}
		explicit BaseLink(const BaseLink& other) { m_idx = other.m_idx;}


		bool isValid() const { return (m_idx != INVALID);}

		//ops
		BaseLink& operator=(const BaseLink& other) {
			this->m_idx = other.m_idx;
			return (*this);
		}

		BaseLink& operator=(U32 idx) {
			this->m_idx = idx;
			return (*this);
		}

		bool operator<(const BaseLink& other) const { return this->m_idx < other.m_idx; }
		bool operator>(const BaseLink& other) const { return this->m_idx > other.m_idx; }
		bool operator==(const BaseLink& other) const { return this->m_idx == other.m_idx; }
		bool operator<(U32 idx) const { return this->m_idx < idx; }
		bool operator>(U32 idx) const { return this->m_idx > idx; }
		bool operator==(U32 idx) const { return this->m_idx == idx; }

		inline operator U32() const {return m_idx;}

	protected:
		U32 m_idx;
	};

	//links for all entities
	class NodeLink : public BaseLink { NodeLink(U32 idx = INVALID): BaseLink(idx){} };
	class EdgeLink : public BaseLink { EdgeLink(U32 idx = INVALID): BaseLink(idx){} };
	class FaceLink : public BaseLink { FaceLink(U32 idx = INVALID): BaseLink(idx){} };
	class CellLink : public BaseLink { CellLink(U32 idx = INVALID): BaseLink(idx){} };


	// Helper class that is used to decrease all handles
	// exceeding a certain threshold

	/*
	class VHandleCorrection {
	public:
	    VHandleCorrection(VertexHandle _thld) : thld_(_thld) {}
	    void correctValue(VertexHandle& _h) {
	        if(_h > thld_) _h.idx(_h.idx() - 1);
	    }
	private:
	    VertexHandle thld_;
	};

	class HEHandleCorrection {
	public:
	    HEHandleCorrection(HalfEdgeHandle _thld) : thld_(_thld) {}
	    void correctVecValue(std::vector<HalfEdgeHandle>& _vec) {
	        std::for_each(_vec.begin(), _vec.end(), fun::bind(&HEHandleCorrection::correctValue, this, fun::placeholders::_1));
	    }
	    void correctValue(HalfEdgeHandle& _h) {
	        if(_h > thld_) _h.idx(_h.idx() - 2);
	    }
	private:
	    HalfEdgeHandle thld_;
	};

	class HFHandleCorrection {
	public:
	    HFHandleCorrection(HalfFaceHandle _thld) : thld_(_thld) {}
	    void correctVecValue(std::vector<HalfFaceHandle>& _vec) {
	        std::for_each(_vec.begin(), _vec.end(), fun::bind(&HFHandleCorrection::correctValue, this, fun::placeholders::_1));
	    }
	    void correctValue(HalfFaceHandle& _h) {
	        if(_h > thld_) _h.idx(_h.idx() - 2);
	    }
	private:
	    HalfFaceHandle thld_;
	};
	*/

	//correct handles
	class HandleCorrection {
	public:
		HandleCorrection(U32 handle) : m_handle(handle) {}

		void correctVecValue(std::vector<U32>& _vec) {
	        std::for_each(_vec.begin(), _vec.end(), std::bind(&HandleCorrection::correctValue, this, std::placeholders::_1));
	    }

		void correctValue(U32& rhs) {
	        if(rhs > m_handle)
	        	rhs = rhs - 1;
	    }
	private:
	    U32 m_handle;
	};


	//vertices
	class NODE {
	public:
		vec3d pos;
		vec3d restpos;

		NODE& operator = (const NODE& A) {
			pos = A.pos;
			restpos = A.restpos;
			return (*this);
		}
	};

	//Key to access edges in a unique order
	class EdgeKey
	{
	public:
		EdgeKey(): key(0) {}
		explicit EdgeKey(U64 k) { this->key = k; }
		explicit EdgeKey(U32 a, U32 b) {
			setup(a, b);
	    }

		void setup(U32 a, U32 b) {
			if(a > b)
				std::swap(a, b);
	    	key = HEDGEID_FROM_IDX(a, b);
	    }

	    bool operator<(const EdgeKey& k) const { return key < k.key; }

	    bool operator>(const EdgeKey& k) const { return key > k.key; }

	    void operator=(const EdgeKey& other) {
	    	this->key = other.key;
	    }

	    U64 key;
	};


	//edge
	class EDGE {
	public:
		U32 from, to;

		EDGE() { init();}
		EDGE(U32 from_, U32 to_) {
			setup(from_, to_);
		}

		void init() {
			from = to = BaseLink::INVALID;
		}

		void setup(U32 from_, U32 to_) {

			from = from_;
			to = to_;
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
		U32 edges[COUNT_FACE_EDGES];

		FACE() {
			init();
		}

		void init() {
			for(int i=0; i<COUNT_FACE_EDGES; i++)
				edges[i] = BaseLink::INVALID;
		}

		FACE& operator = (const FACE& A) {
			for(int i=0; i<COUNT_FACE_EDGES; i++)
				edges[i] = A.edges[i];
			return (*this);
		}
	};

	//Key to access faces in a unique order
	class FaceKey
	{
	public:
		FaceKey():m_key(0) {}
		explicit FaceKey(U64 k) { this->m_key = k; }
		explicit FaceKey(U32 n[3]) {
			setup(n[0], n[1], n[2]);
		}

	    explicit FaceKey(U32 a, U32 b, U32 c) {
	    	setup(a, b, c);
	    }

	    void setup(U32 a, U32 b, U32 c) {
	    	order_lo2hi(a, b, c);
	    	m_key = FACEID_FROM_IDX(a, b, c);
	    }

	    static void order_lo2hi(U32& a, U32& b, U32& c) {
	    	if(a > b)
	    		swap(a, b);
	    	if(b > c)
	    		swap(b, c);
	    	if(a > b)
	    		swap(a, b);
	    }

	    U64 key() const {return m_key;}

	    bool operator<(const FaceKey& k) const { return m_key < k.m_key; }

	    bool operator>(const FaceKey& k) const { return m_key > k.m_key; }

	    void operator=(const FaceKey& other) {
	    	this->m_key = other.m_key;
	    }

	    bool operator==(const FaceKey& other) {
	    	return (this->m_key == other.m_key);
	    }

	private:
	    U64 m_key;
	};



	//elements
	class CELL {
	public:
		U32 nodes[COUNT_CELL_NODES];
		U32 faces[COUNT_CELL_FACES];
		U32 edges[COUNT_CELL_EDGES];

		CELL() {
			init();
		}

		void init() {
			for(int i=0; i<4; i++) {
				nodes[i] = BaseLink::INVALID;
				faces[i] = BaseLink::INVALID;
			}

			for(int i=0; i<6; i++)
				edges[i] = BaseLink::INVALID;
		}

		CELL& operator = (const CELL& A) {
			for(int i=0; i<4; i++) {
				nodes[i] = A.nodes[i];
				faces[i] = A.faces[i];
			}

			for(int i=0; i<6; i++)
				edges[i] = A.edges[i];

			return (*this);
		}
	};

}
}


#endif /* CELLULARMESHTYPES_H_ */
