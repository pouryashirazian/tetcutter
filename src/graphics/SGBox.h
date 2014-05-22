#ifndef SCENEBOX_H
#define SCENEBOX_H

#include "SGMesh.h"

using namespace PS;
using namespace PS::GL;

namespace PS {
namespace SG {

class SGBox : public SGMesh {
public:
	SGBox();
	SGBox(const vec3f& lo, const vec3f& hi);
	virtual ~SGBox();
	void setup(const vec3f& lo, const vec3f& hi);

	void draw();

	float width() const {return (m_hi.x - m_lo.x);}
    float height() const {return (m_hi.y - m_lo.y);}
    float depth() const {return (m_hi.z - m_lo.z);}
    vec3f lo() const { return m_lo;}
    vec3f hi() const { return m_hi;}
private:
    vec3f m_lo;
    vec3f m_hi;
};

}
}


#endif
