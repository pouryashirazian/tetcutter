#ifndef IMOUSELISTENER_H_
#define IMOUSELISTENER_H_

namespace ps {

enum MouseButton {mbLeft, mbRight, mbMid};
enum MouseButtonState {mbsUp, mbsDown};
enum MouseWheelDir {mwDown = -1, mwUp = 1};

//Interface for all mouse widgets
class IMouseListener {
public:
    IMouseListener();

	virtual ~IMouseListener() {
	}

	//Mouse Events
    virtual void mousePress(MouseButton button, MouseButtonState state, int x, int y);
    virtual void mouseMove(int x, int y);
    virtual void mouseWheel(MouseWheelDir dir);


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
