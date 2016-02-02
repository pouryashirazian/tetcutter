/*
 * IMouseListener.h
 *
 *  Created on: Mar 16, 2014
 *      Author: pourya
 */

#ifndef IMOUSELISTENER_H_
#define IMOUSELISTENER_H_

namespace PS {

enum MouseButton {mbLeft, mbMiddle, mbRight, mbNone};
enum MouseButtonState {mbsDown, mbsUp};

//Interface for all mouse widgets
class IMouseListener {
public:
    IMouseListener() {
        m_isFocused = false;
    }

    virtual ~IMouseListener() {

    }

    //Mouse Events
    virtual void mousePress(MouseButton button, MouseButtonState state, int x, int y) {}
    virtual void mouseMove(int x, int y) {}
    virtual void mouseWheel(MouseButton button, int dir, int x, int y) {}


    //Focus
    bool focus() const { return m_isFocused;}
    void setFocus(bool focus) {
        m_isFocused = focus;
    }

protected:
    bool m_isFocused;
};

}



#endif /* IMOUSELISTENER_H_ */
