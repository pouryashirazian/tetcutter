#ifndef GLTEXTURE_H
#define GLTEXTURE_H

#include <string>
#include <unordered_map>

#include "base/vec.h"
#include "base/str.h"
#include "base/color.h"
#include "glbindable.h"
#include "glshader.h"


using namespace std;
using namespace ps;
using namespace ps::base;

namespace ps {
    namespace opengl {

        class GLFont {
        public:
            GLFont();
            GLFont(const string& strFontPath);
            virtual ~GLFont();

            /// Holds all state information relevant to a character as loaded using FreeType
            struct Character {
                U32 TextureID;   // ID handle of the glyph texture
                U32 Advance;    // Horizontal offset to advance to next glyph
                vec2i Size;    // Size of glyph
                vec2i Bearing;  // Offset from baseline to left/top of glyph
            };

            bool setup(const string& font_name);
            void render_text(GLShader& shader, const string& text, float x, float y, float scale, const Color& color);

        protected:
            std::unordered_map<char, Character> m_alphabet;
            U32 m_vao;
            U32 m_vbo;
        };

    }
}


#endif
