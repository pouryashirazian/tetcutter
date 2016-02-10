#include "imouselistener.h"
#include "base.h"

using namespace ps;

IMouseListener::IMouseListener() {
    m_isFocused = false;
}


void IMouseListener::mousePress(MouseButton button, MouseButtonState state, int x, int y) {
    PS_UNUSED(button);
    PS_UNUSED(state);
    PS_UNUSED(x);
    PS_UNUSED(y);
}

void IMouseListener::mouseMove(int x, int y) {
    PS_UNUSED(x);
    PS_UNUSED(y);
}

void IMouseListener::mouseWheel(MouseWheelDir dir) {
    PS_UNUSED(dir);
}
