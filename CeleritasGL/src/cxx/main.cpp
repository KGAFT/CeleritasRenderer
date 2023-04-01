#include <iostream>
#include "Window/Window.h"
int main() {
    Window::createWindow(800, 600, "Hello world", nullptr);
    glViewport(0, 0, 800, 600);
    while(!Window::getInstance()->needToClose()){
        glClear(GL_COLOR_BUFFER_BIT);
        glClearColor(0.5f,0.2f,0.5f,1.0f);
        Window::getInstance()->postRenderEvents();
    }
    return 0;
}
