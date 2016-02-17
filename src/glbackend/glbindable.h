#ifndef GLBINDABLE_H
#define GLBINDABLE_H

namespace ps {
namespace opengl {

class GLBindable
{
public:
    GLBindable();
    virtual ~GLBindable();

    virtual void bind() = 0;

    virtual void unbind() = 0;
};

}
}

#endif // GLBINDABLE_H
