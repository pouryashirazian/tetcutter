/*
 * Color.h
 *
 *  Created on: Sep 23, 2014
 *      Author: pourya
 */

#ifndef COLOR_H_
#define COLOR_H_

#include "Vec.h"

namespace PS{
namespace MATH{


class Color {
public:
	Color() {
		m_color = vec4f(0,0,0,1);
	}

	explicit Color(U8 r, U8 g, U8 b, U8 a = 255) {
		fromRGBA(r, g, b, a);
	}

	explicit Color(const vec4u8& color) {
		fromRGBA(color);
	}

	explicit Color(const vec4f& color) {
		m_color = color;
	}

	Color(const Color& other);

	void fromRGBA(const vec4u8& color) {
		fromRGBA(color.x, color.y, color.z, color.w);
	}

	void fromRGBA(U8 r, U8 g, U8 b, U8 a = 255) {
		static const float fconv = 1.0f / 255.0f;
		m_color = vec4f((float)r, (float)g, (float)b, (float)a) * fconv;
	}

	void fromRGBA(const vec4f& color) { m_color = color;}

	vec4f toVec4f() const { return m_color;}

	vec4u8 toVec4u8() const {
		vec4u8 res((U8)(m_color.x * 255.0f), (U8)(m_color.y * 255.0f), (U8)(m_color.z * 255.0f), (U8)(m_color.w * 255.0f));
		return res;
	}

	Color& operator=(const Color& rhs) {
		m_color = rhs.m_color;
		return *this;
	}

	//sample colors
	static Color red() { return Color(255, 0, 0);}
	static Color green() { return Color(0, 255, 0);}
	static Color blue() { return Color(0, 0, 255);}
	static Color grey() { return Color(128, 128, 128);}
	static Color black() { return Color(0, 0, 0);}
	static Color white() { return Color(255, 0, 0);}
	static Color skin() { return Color(239, 208, 207);}
private:
	vec4f m_color;

};

}
}



#endif /* COLOR_H_ */
