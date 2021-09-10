#include "renderer.h"
#include "imgui.h"

Renderer::Renderer() : shader_{vert_shader_path, frag_shader_path} {
    init_texture();
    init_vbo();
    init_ebo();
    init_vao();
}

Renderer::~Renderer() {
    spdlog::debug("Deleting vao");
    glDeleteVertexArrays(1, &vao_);
    spdlog::debug("Deleting texture");
    glDeleteTextures(1, &texture_);
    spdlog::debug("Deleting vbo");
    glDeleteBuffers(1, &vbo_);
    spdlog::debug("Deleting ebo");
    glDeleteBuffers(1, &ebo_);
}

void Renderer::render(const std::array<bool, Chip8::SCREEN_WIDTH * Chip8::SCREEN_HEIGHT>& screen) {
    update_texture(screen);

    // background color
    glClearColor(bg_color_.r, bg_color_.g, bg_color_.b, 1.0f);
    glClear(ClearBufferMask::GL_COLOR_BUFFER_BIT);

    // bind vao, set box color uniform, and draw
    glBindVertexArray(vao_);
    glUseProgram(shader_.id());
    shader_.set_float3("box_color", box_color_.r, box_color_.g, box_color_.b);
    glDrawElements(GLenum::GL_TRIANGLES, NUM_ELEMENTS, GLenum::GL_UNSIGNED_INT, nullptr);
    // unbind
    glUseProgram(0);
    glBindVertexArray(0);
}

void Renderer::update_texture(const std::array<bool, Chip8::SCREEN_WIDTH * Chip8::SCREEN_HEIGHT>& screen) {
    // calculate luminance from internal screen
    auto to_luminance = [](bool b) { return b ? 255u : 0u; };
    std::ranges::transform(screen, std::begin(luminance_), to_luminance);

    // upload luminance to texture
    glBindTexture(GLenum::GL_TEXTURE_2D, texture_);
    glTexSubImage2D(GLenum::GL_TEXTURE_2D, 0, 0, 0, Chip8::SCREEN_WIDTH, Chip8::SCREEN_HEIGHT,
            GLenum::GL_RED, GLenum::GL_UNSIGNED_BYTE, luminance_.data());
}


void Renderer::init_texture() {
    glGenTextures(1, &texture_);

    // use TEXTURE0
    GLuint active_texture = 0;
    glActiveTexture(GLenum::GL_TEXTURE0);
    // configure texture
    glBindTexture(GLenum::GL_TEXTURE_2D, texture_);
    glTexParameteri(GLenum::GL_TEXTURE_2D, GLenum::GL_TEXTURE_MIN_FILTER, GLenum::GL_NEAREST);
    glTexParameteri(GLenum::GL_TEXTURE_2D, GLenum::GL_TEXTURE_MAG_FILTER, GLenum::GL_NEAREST);
    glTexParameteri(GLenum::GL_TEXTURE_2D, GLenum::GL_TEXTURE_WRAP_S, GLenum::GL_CLAMP_TO_EDGE);
    glTexParameteri(GLenum::GL_TEXTURE_2D, GLenum::GL_TEXTURE_WRAP_T, GLenum::GL_CLAMP_TO_EDGE);

    // create storage for texture
    glTexStorage2D(GLenum::GL_TEXTURE_2D, 1, GLenum::GL_R8, Chip8::SCREEN_WIDTH, Chip8::SCREEN_HEIGHT);
    // link uniform sampler to texture unit
    // TODO: replace constant string "texture_sampler"
    shader_.set_int("texture_sampler", active_texture);

    // unbind texture
    glBindTexture(GLenum::GL_TEXTURE_2D, 0);
}

void Renderer::init_vbo() {
    glGenBuffers(1, &vbo_);
    glBindBuffer(GLenum::GL_ARRAY_BUFFER, vbo_);
    glBufferStorage(GLenum::GL_ARRAY_BUFFER, sizeof(vertices_), vertices_, 
            BufferStorageMask::GL_NONE_BIT);
    glBindBuffer(GLenum::GL_ARRAY_BUFFER, 0);
}

void Renderer::init_ebo() {
    glGenBuffers(1, &ebo_);
    glBindBuffer(GLenum::GL_ELEMENT_ARRAY_BUFFER, ebo_);
    glBufferStorage(GLenum::GL_ELEMENT_ARRAY_BUFFER, sizeof(elements_), elements_, 
            BufferStorageMask::GL_NONE_BIT);
    glBindBuffer(GLenum::GL_ELEMENT_ARRAY_BUFFER, 0);
}

void Renderer::init_vao() {
    glGenVertexArrays(1, &vao_);
    glBindVertexArray(vao_);
    glBindBuffer(GLenum::GL_ARRAY_BUFFER, vbo_);
    glBindBuffer(GLenum::GL_ELEMENT_ARRAY_BUFFER, ebo_);
    // configure vao attributes
    glVertexAttribPointer(POS_ATTR, POS_ATTR_SIZE, GLenum::GL_FLOAT, 
            GL_FALSE, VERTEX_SIZE, reinterpret_cast<void*>(POS_OFFSET));
    glVertexAttribPointer(TEX_ATTR, TEX_ATTR_SIZE, GLenum::GL_FLOAT, 
            GL_FALSE, VERTEX_SIZE, reinterpret_cast<void*>(TEX_OFFSET));
    // enable attributes
    glEnableVertexAttribArray(POS_ATTR);
    glEnableVertexAttribArray(TEX_ATTR);
    // unbind everything
    glBindVertexArray(0);
    glBindBuffer(GLenum::GL_ARRAY_BUFFER, 0);
    glBindBuffer(GLenum::GL_ELEMENT_ARRAY_BUFFER, 0);
}

