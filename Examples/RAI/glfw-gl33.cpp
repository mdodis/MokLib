#include "Platform/RAI.h"
#include "Platform/RAIOpenGL33.h"
#include <GLFW/glfw3.h>
#include "FileSystem/Extras.h"

int main(int argc, char *argv[]) {
    if (glfwInit() != GLFW_TRUE) {
        return false;
    }

    glfwWindowHint(GLFW_RED_BITS, 8);
    glfwWindowHint(GLFW_BLUE_BITS, 8);
    glfwWindowHint(GLFW_GREEN_BITS, 8);
    glfwWindowHint(GLFW_ALPHA_BITS, 8);
    glfwWindowHint(GLFW_ALPHA_BITS, 8);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

}

#include "Platform/RAIOpenGL33.cpp"