#include <SDL_events.h>
#include <glbinding/gl/enum.h>
#include <glbinding/gl/functions.h>
#include <glbinding/glbinding.h>
#include <glbinding/gl/gl.h>
#define IMGUI_IMPL_OPENGL_LOADER_GLBINDING3
#include <SDL.h>
#include <SDL2/SDL_video.h>

#include "spdlog/spdlog.h"
// Vertex Shader source code
const char* vertexShaderSource = "#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"void main()\n"
"{\n"
"   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
"}\0";
//Fragment Shader source code
const char* fragmentShaderSource = "#version 330 core\n"
"out vec4 FragColor;\n"
"void main()\n"
"{\n"
"   FragColor = vec4(0.8f, 0.3f, 0.02f, 1.0f);\n"
"}\n\0";


using namespace gl;

int main() {
    spdlog::set_level(spdlog::level::debug);

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        spdlog::error("Could not initialize SDL\nError: %s\n", SDL_GetError());
        return 10;
    }

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK,
                        SDL_GL_CONTEXT_PROFILE_CORE);

    SDL_Window *window = SDL_CreateWindow(
        "OpenGL window", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 800,
        800, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
    SDL_GLContext gl_context = SDL_GL_CreateContext(window);
    SDL_GL_MakeCurrent(window, gl_context);
    SDL_GL_SetSwapInterval(1);

    glbinding::initialize([](const char *name) {
        return (glbinding::ProcAddress)SDL_GL_GetProcAddress(name);
    });

    glViewport(0, 0, 800, 800);

    // create vertex shader object
    GLuint vertex_shader = glCreateShader(GLenum::GL_VERTEX_SHADER);
    // attache shader source to object
    glShaderSource(vertex_shader, 1, &vertexShaderSource, nullptr);
    glCompileShader(vertex_shader);
    GLint status;
    glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &status);
    spdlog::debug("vertex_shader status: {}", status);
    char buffer[512];
    glGetShaderInfoLog(vertex_shader, 512, nullptr, buffer);
    spdlog::debug("vertex_shader log\n{}", buffer);
    

    // do the same for fragment shader
    GLuint fragment_shader = glCreateShader(GLenum::GL_FRAGMENT_SHADER);
    glShaderSource(fragment_shader, 1, &fragmentShaderSource, nullptr);
    glCompileShader(fragment_shader);
    glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &status);
    spdlog::debug("fragment_shader status: {}", status);
    glGetShaderInfoLog(fragment_shader, 512, nullptr, buffer);
    spdlog::debug("fragment_shader log\n{}", buffer);

    // create shader programs
    GLuint shader_program = glCreateProgram();
    // attach shaders
    glAttachShader(shader_program, vertex_shader);
    glAttachShader(shader_program, fragment_shader);
    // link program
    glLinkProgram(shader_program);

    GLfloat vertices[] =
	{
		-0.5f, -0.5f, 0.0f, // Lower left corner
		0.5f, -0.5f, 0.0f, // Lower right corner
		0.5f, 0.5f, 0.0f, // Upper right
		-0.5f, 0.5f, 0.0f // Upper left
	};


    // create references for vertex array object and vertex buffer object
    GLuint vertex_array_object, vertex_buffer_object;
    // generate them with 1 object each
    glGenVertexArrays(1, &vertex_array_object);
    glGenBuffers(1, &vertex_buffer_object);

    // bind vertex array object
    glBindVertexArray(vertex_array_object);

    // bind vertex_buffer_object 
    glBindBuffer(GLenum::GL_ARRAY_BUFFER, vertex_buffer_object);
    // give vertices data to buffer
    glBufferData(GLenum::GL_ARRAY_BUFFER, sizeof(vertices), vertices, GLenum::GL_STATIC_DRAW);

    // add elements to draw over vertices
    GLuint elements[] = {
        0, 1, 2, 2, 3, 0
    };
    GLuint elements_buffer_object;
    glGenBuffers(1, &elements_buffer_object);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elements_buffer_object);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(elements), elements, GL_STATIC_DRAW);

    // configure vertex attribute so gl know how to read vertex_buffer_object
    glVertexAttribPointer(0, 3, GLenum::GL_FLOAT, GL_FALSE, 3*sizeof(float), nullptr);
    // enable vertex attribute
    glEnableVertexAttribArray(0);

    // rebind vertex_array_object and vertex_buffer_object  so that we dont modify it incorrectly
    glBindBuffer(GLenum::GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    bool running = true;
    auto steps = std::array<int, 5> {1, 2, 3, 4, 5};
    auto counter = 0;
    auto step = 0;
    float color = 0.07f;

    while (running) {
        counter++;
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_QUIT:
                    running = false;
            }
        }

        if (counter >= 60) {
            color = 0.07f * steps[step++];
            counter = 0;
        } 

        // color background
        glClearColor(color, 0.13f, 0.17f, 1.0f);
        // clean back buffer and assign color to it
        glClear(GL_COLOR_BUFFER_BIT);
        // tell gl to use shader program
        glUseProgram(shader_program);
        // bind vertex_array
        glBindVertexArray(vertex_array_object);
        // draw trianges using GL_TRIANGLES primitive
        // use glDrawElements instead to make a rectangle
        // glDrawArrays(GLenum::GL_TRIANGLE_STRIP, 0, 4);
        glDrawElements(GLenum::GL_TRIANGLES, 6, GLenum::GL_UNSIGNED_INT, nullptr);
        // swap buffers
        SDL_GL_SwapWindow(window);
    }

    // free objects
    glDeleteVertexArrays(1, &vertex_array_object);
    glDeleteBuffers(1, &vertex_buffer_object);
    glDeleteProgram(shader_program);

    SDL_GL_DeleteContext(gl_context);
    SDL_DestroyWindow(window);
    SDL_Quit();
}


        



