#ifndef PS_MATRIXSTACK_H
#define PS_MATRIXSTACK_H

#include <vector>

using namespace std;

template <typename T>
class CopyStack{

public:
    CopyStack(){
        T first;
        m_stk.push_back(first);
    }

    virtual ~CopyStack() { m_stk.resize(0);}


    void push()
    {
        T top = m_stk.back();
        m_stk.push_back(top);
    }

    void pop() {m_stk.pop();}

    T top() const { return m_stk.back();}

private:
    std::vector<T> m_stk;
};

#endif // PS_MATRIXSTACK_H
