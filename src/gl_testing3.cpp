
#include <SDL_events.h>
#include <glbinding/gl/enum.h>
#include <glbinding/gl/functions.h>
#include <glbinding/glbinding.h>
#include <glbinding/gl/gl.h>
#define IMGUI_IMPL_OPENGL_LOADER_GLBINDING3 1
#include <SDL.h>
#include <SDL_video.h>
#include "spdlog/spdlog.h"

#include "common.h"

// Vertex Shader source code
const char* vertexShaderSource = "#version 440 core\n"
"layout (location = 0) in vec3 aPos;\n"
"layout (location = 1) in vec2 texcoord;\n"
"out vec2 Texcoord;\n"
"void main()\n"
"{\n"
"   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
"   Texcoord = texcoord;\n"
"}\0";
//Fragment Shader source code
const char* fragmentShaderSource = "#version 440 core\n"
"in vec2 Texcoord;\n"
"out vec4 FragColor;\n"
"uniform sampler2D tex;\n"
/* "vec4 rgb_sample(usampler2D sampler, vec2 coordinate)\n" */
/* "{\n" */
/* "       uint texValue = texture(sampler, coordinate).r;\n" */
/* "       return vec4(texValue & 4u, texValue & 2u, texValue & 1u, 1.0);\n" */
/* "}\n" */
"void main()\n"
"{\n"
/* "   FragColor = rgb_sample(tex, Texcoord);\n" */
/* "   FragColor = vec4(texture(tex, Texcoord).rrr, 1.0) * vec4(0.8, 0.2, 0.7, 1.0);\n" */
"   float alpha = texture(tex, Texcoord).r;\n"
/* "   if (alpha < 0.01);\n" */
/* "       discard;\n" */
"   FragColor = vec4(1.0, 0.0, 0.0, alpha);\n"
"}\n\0";


using namespace gl;

void  dbg_callback(GLenum source,
                 GLenum type,
                 GLuint id,
                 GLenum severity,
                 GLsizei length,
                 const GLchar* message,
                 const void* userParam ) {
    spdlog::debug("GL CALLBACK: {}, severity = {:x}, message = {}",
            (type == GL_DEBUG_TYPE_ERROR ? "GL Error" : ""),
            severity, message);
}

int main() {
    spdlog::set_level(spdlog::level::debug);

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        spdlog::error("Could not initialize SDL\nError: %s\n", SDL_GetError());
        return 10;
    }

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK,
                        SDL_GL_CONTEXT_PROFILE_CORE);

    SDL_Window *window = SDL_CreateWindow(
        "OpenGL window", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 600,
        400, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
    SDL_GLContext gl_context = SDL_GL_CreateContext(window);
    SDL_GL_MakeCurrent(window, gl_context);
    SDL_GL_SetSwapInterval(1);

    glbinding::initialize([](const char *name) {
        return (glbinding::ProcAddress)SDL_GL_GetProcAddress(name);
    });

    glEnable(GLenum::GL_DEBUG_OUTPUT);
    glDebugMessageCallback(dbg_callback, 0);

    // set up transparency
    glEnable(GLenum::GL_BLEND);
    glBlendFunc(GLenum::GL_SRC_ALPHA, GLenum::GL_ONE_MINUS_SRC_ALPHA);

    glViewport(0, 0, 600, 400);

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
    glBindFragDataLocation(shader_program, 0, "outColor");
    // link program
    glLinkProgram(shader_program);
    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);
    glUseProgram(shader_program);

    GLfloat vertices[] =
	{ 
                // screen coordinates   // texture coordinates
		-1.0f, -1.0f, 0.0f,     0.0f, 0.0f,           // Lower left corner
		1.0f, -1.0f, 0.0f,      1.0f, 0.0f,           // Lower right corner
		1.0f, 1.0f, 0.0f,       1.0f, 1.0f,           // Upper right
		-1.0f, 1.0f, 0.0f,      0.0f, 1.0f            // Upper left
	};

    // make texture
    GLuint texture;
    glGenTextures(1, &texture);

    /* std::array<GLubyte, 24> screen_data {  */
    /*     10u, 255u, 90u, 255u,  200u, 20u, 20u, 255u,  20u, 20u, 0u, 255u, */
    /*     20u, 20u, 20u, 255u,     20u, 200u, 20u, 255u,  200u, 200u, 200u, 255u */
    /* }; */

    /* std::array<GLubyte, 18> screen_data {  */
    /*     10u, 255u, 90u,     200u, 20u, 20u,     20u, 20u, 0u, */
    /*     20u, 20u, 20u,      20u, 200u, 20u,     200u, 200u, 200u */
    /* }; */

    std::array<GLubyte, 6> screen_data { 
        10u,    200u,   20u, 
        20u,    20u,    200u
    };

    // configure texture
    glBindTexture(GLenum::GL_TEXTURE_2D, texture);
    glTexParameteri(GLenum::GL_TEXTURE_2D, GLenum::GL_TEXTURE_MIN_FILTER, GLenum::GL_NEAREST);
    glTexParameteri(GLenum::GL_TEXTURE_2D, GLenum::GL_TEXTURE_MAG_FILTER, GLenum::GL_NEAREST);
    glTexParameteri(GLenum::GL_TEXTURE_2D, GLenum::GL_TEXTURE_WRAP_S, GLenum::GL_CLAMP_TO_EDGE);
    glTexParameteri(GLenum::GL_TEXTURE_2D, GLenum::GL_TEXTURE_WRAP_T, GLenum::GL_CLAMP_TO_EDGE);
    // glGenerateMipmap(GLenum::GL_TEXTURE_2D);
    glTexStorage2D(GLenum::GL_TEXTURE_2D, 1, GLenum::GL_R8, 3, 2);
    glTexSubImage2D(GLenum::GL_TEXTURE_2D, 0, 0, 0, 3, 2,
            GLenum::GL_RED, GLenum::GL_UNSIGNED_BYTE, screen_data.data());
    // uniform sampler for shader
    glUniform1i(glGetUniformLocation(shader_program, "tex"), 0);



    // create references for vertex array object and vertex buffer object
    GLuint vao, vbo;
    // generate them with 1 object each
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);

    // bind vertex array object
    glBindVertexArray(vao);

    // bind vbo 
    glBindBuffer(GLenum::GL_ARRAY_BUFFER, vbo);
    // give vertices data to buffer
    glBufferData(GLenum::GL_ARRAY_BUFFER, sizeof(vertices), vertices, GLenum::GL_STATIC_DRAW);

    // add elements to draw over vertices
    GLuint elements[] = {
        0, 1, 2, 2, 3, 0
    };
    GLuint ebo;
    glGenBuffers(1, &ebo);
    glBindBuffer(GLenum::GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GLenum::GL_ELEMENT_ARRAY_BUFFER, sizeof(elements), elements, GLenum::GL_STATIC_DRAW);

    // configure vertex attribute so gl know how to read vbo
    glVertexAttribPointer(0, 3, GLenum::GL_FLOAT, GL_FALSE, 5*sizeof(float), 0);
    // texture attribute
    glVertexAttribPointer(1, 2, GLenum::GL_FLOAT, GL_FALSE, 5*sizeof(float), (void*)(3*sizeof(float)));
    // enable position attribute
    glEnableVertexAttribArray(0);
    // enable texture attribute
    glEnableVertexAttribArray(1);

    // rebind vao and vbo  so that we dont modify it incorrectly
    glBindBuffer(GLenum::GL_ARRAY_BUFFER, 0);

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
        // bind texture
        /* glBindTexture(GLenum::GL_TEXTURE_2D, texture); */
        // bind vertex_array
        /* glBindVertexArray(vao); */
        /* // draw trianges using GL_TRIANGLES primitive */
        /* // use glDrawElements instead to make a rectangle */
        /* // glDrawArrays(GLenum::GL_TRIANGLE_STRIP, 0, 4); */
        /* glDrawElements(GLenum::GL_TRIANGLES, 6, GLenum::GL_UNSIGNED_INT, nullptr); */
        // swap buffers
        SDL_GL_SwapWindow(window);
    }

    // free objects
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
    glDeleteBuffers(1, &ebo);
    glDeleteProgram(shader_program);
    glDeleteTextures(1, &texture);

    SDL_GL_DeleteContext(gl_context);
    SDL_DestroyWindow(window);
    SDL_Quit();
}


        



