#ifndef IMOUSELISTENER_H_
#define IMOUSELISTENER_H_

namespace ps {

enum MouseButton {mbLeft, mbRight, mbMiddle};
enum MouseButtonState {mbsReleased, mbsPressed};
enum MouseWheelDir {mwDown = -1, mwUp = 1};

//Interface for all mouse widgets
class IMouseListener {
public:
    IMouseListener();

	virtual ~IMouseListener() {
	}

	//Mouse Events
    virtual void mousePress(MouseButton button, MouseButtonState state, int x, int y) = 0;
    virtual void mouseWheel(MouseWheelDir dir) = 0;
    virtual void mouseMove(int x, int y) = 0;


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
