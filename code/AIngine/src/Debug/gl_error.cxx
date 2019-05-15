

#include <glad/glad.h>
#include <sstream>

void CheckGLError() {

#ifdef DEBUG_GLFLUSH
    // Forces the command buffer to be flushed
    glFlush();
#endif

    bool errOccurred = false;
    GLenum error;
    std::stringstream ss;
    while ((error = glGetError()) != GL_NO_ERROR) {
        errOccurred = true;

        switch (error) {
            case GL_INVALID_ENUM:
                ss << "GL_INVALID_ENUM" << std::endl;
                break;
            case GL_INVALID_VALUE:
                ss << "GL_INVALID_VALUE" << std::endl;
                break;
            case GL_INVALID_OPERATION:
                ss << "GL_INVALID_OPERATION" << std::endl;
                break;
#ifdef GL_STACK_OVERFLOW
            case GL_STACK_OVERFLOW:
                            ss << "GL_STACK_OVERFLOW" << std::endl;
                            break;
                        case GL_STACK_UNDERFLOW:
                            ss << "GL_STACK_UNDERFLOW" << std::endl;
                            break;
#endif
            case GL_OUT_OF_MEMORY:
                ss << "GL_OUT_OF_MEMORY" << std::endl;
                break;
        }
    }

    if (errOccurred)
        throw std::runtime_error(ss.str());
}
