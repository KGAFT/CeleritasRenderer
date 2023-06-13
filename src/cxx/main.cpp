#include "Window/Window.h"
#include "Window/Monitor.h"
#include "Util/ModelLoader.h"
#include <glm/glm.hpp>
#include <array>


int main() {
    Window::initializeContext();
    ModelLoader loader(nullptr);
   
    return 0;
}
