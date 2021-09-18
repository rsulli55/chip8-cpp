
#include "imgui.h"
#include "imgui_impl_opengl3.h"
#include "imgui_impl_sdl.h"

#include "instruction.h"
#include "window.h"

#define IMGUI_IMPL_OPENGL_LOADER_GLBINDING3 1

Window::Window(u8 screen_scale, Renderer& renderer) :
    screen_scale_{screen_scale},
    window_width_{Chip8::SCREEN_WIDTH * screen_scale},
    window_height_{Chip8::SCREEN_HEIGHT * screen_scale},
    renderer_{renderer} {
        u32 ec = init_video();
        if (ec!= 0) {
            spdlog::error("initializing video failed, exiting.");
            std::terminate();
        }
    }

Window::~Window() {
    spdlog::debug("Calling ImGui_ImplOpenGL3_Shutdown");
    ImGui_ImplOpenGL3_Shutdown();
    spdlog::debug("Calling ImGui_ImplSDL2_Shutdown");
    ImGui_ImplSDL2_Shutdown();
    spdlog::debug("Calling ImGui::DestroyContext");
    ImGui::DestroyContext();

    spdlog::debug("Deleting GL Context");
    SDL_GL_DeleteContext(gl_context_);
    spdlog::debug("Deleting SDL Window");
    SDL_DestroyWindow(window_);
    spdlog::debug("Calling SDL Quit");
    SDL_Quit();
}

void Window::render(const std::vector<u16>& past_opcodes, const Chip8& chip8, u32 future_to_show) {
    // start ImGui frame, then draw using OpenGL, then render ImGui frame
    const auto& screen = chip8.screen();
    start_ImGui_frame();
    renderer_.render(screen);
    render_ImGui_frame(past_opcodes, chip8, future_to_show);
    SDL_GL_SwapWindow(window_);
}

void Window::resize() {
    int width = 0;
    int height = 0;
    SDL_GL_GetDrawableSize(window_, &width, &height);
    glViewport(0, 0, width, height);
}


u32 Window::init_video() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        spdlog::error("Could not initialize SDL\nError: %s\n", SDL_GetError());
        return 10;
    }

    // configure OpenGL settings
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK,
                        SDL_GL_CONTEXT_PROFILE_CORE);

    // create window and GL context
    auto window_flags = static_cast<SDL_WindowFlags>(SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
    window_ = SDL_CreateWindow(WINDOW_NAME, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
            window_width_, window_height_, window_flags);
    if (window_ == nullptr) {
        spdlog::error("Window could not be created!\nError: %s\n",
                      SDL_GetError());
        return 20;
    }
    gl_context_ = SDL_GL_CreateContext(window_);
    SDL_GL_MakeCurrent(window_, gl_context_);
    SDL_GL_SetSwapInterval(1); // Enable vsync

    glbinding::initialize([](const char* name) { 
            return (glbinding::ProcAddress)SDL_GL_GetProcAddress(name); 
            });

    // enable debug output
    /* glEnable(GLenum::GL_DEBUG_OUTPUT); */
    /* glDebugMessageCallback(gl_dbg_callback, 0); */

    // set up transparency
    glEnable(GLenum::GL_BLEND);
    glBlendFunc(GLenum::GL_SRC_ALPHA, GLenum::GL_ONE_MINUS_SRC_ALPHA);


    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    // Setup Platform/Renderer backends
    ImGui_ImplSDL2_InitForOpenGL(window_, gl_context_);
    ImGui_ImplOpenGL3_Init(glsl_version);

    return 0;
}

void Window::start_ImGui_frame() {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL2_NewFrame(window_);
    ImGui::NewFrame();
}

void Window::debug_window_widget(const std::vector<u16>& past_opcodes, const Chip8& chip8, u32 future_to_show) {
    
    ImGui::Begin("Debugger");
    registers_box(chip8);
    instructions_box(past_opcodes, chip8, future_to_show);
    ImGui::End();
}

void Window::registers_box(const Chip8& chip8) {
    const auto text_base_height = ImGui::GetTextLineHeightWithSpacing();
    const auto text_base_width = ImGui::CalcTextSize("A").x;
    const static ImGuiTableFlags flags = ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders |
        ImGuiTableFlags_NoHostExtendX | ImGuiTableFlags_SizingFixedSame;
    const auto num_rows = 4;
    const auto num_cols = 8;
    const auto special_reg_bg_color = ImGui::GetColorU32(ImVec4(53/255.0f, 76/255.0f, 140/255.0f, 1.0f));

    if (ImGui::BeginTable("table_registers", num_cols, flags)) {
        // always show the header row
        ImGui::TableSetupScrollFreeze(0, 1);
        for (int col = 0; col < num_cols / 2; ++col) {
            ImGui::TableSetupColumn("Reg", ImGuiTableColumnFlags_None);
            ImGui::TableSetupColumn("Value", ImGuiTableColumnFlags_None);
        }
        ImGui::TableHeadersRow();

        // special registers row
        ImGui::TableNextRow();
        ImGui::TableSetBgColor(ImGuiTableBgTarget_RowBg0, special_reg_bg_color);
        ImGui::TableNextColumn();
        ImGui::Text("PC");
        ImGui::TableNextColumn();
        ImGui::Text("0x%x", chip8.pc());
        ImGui::TableNextColumn();
        ImGui::Text("I");
        ImGui::TableNextColumn();
        ImGui::Text("0x%x", chip8.I());
        ImGui::TableNextColumn();
        ImGui::Text("SND");
        ImGui::TableNextColumn();
        ImGui::Text("%d", chip8.sound());
        ImGui::TableNextColumn();
        ImGui::Text("DLY");
        ImGui::TableNextColumn();
        ImGui::Text("%d", chip8.delay());

        ImGuiListClipper clipper;
        clipper.Begin(num_rows);
        while (clipper.Step()) {
            for (int row = clipper.DisplayStart; row < clipper.DisplayEnd; ++row) {
                ImGui::TableNextRow();
                for (int col = 0; col < num_cols / 2; ++col) {
                    const auto reg = row + col * num_rows;
                    ImGui::TableNextColumn();
                    ImGui::Text("V%d", reg);
                    ImGui::TableNextColumn();
                    ImGui::Text("0x%x", chip8.V(reg));
                }
            }
        }
        ImGui::EndTable();
    }
}
                


void Window::instructions_box(const std::vector<u16>& past_opcodes, const Chip8& chip8, u32 future_to_show) {
    const auto text_base_height = ImGui::GetTextLineHeightWithSpacing();
    const auto text_base_width = ImGui::CalcTextSize("A").x;
    const auto instructions_to_show = past_opcodes.size() + future_to_show + 1;
    const auto rows_to_show = 12;
    const auto past_ops_bg_color = ImGui::GetColorU32(ImVec4(140/255.0f, 53/255.0f, 59/255.0f, 1.0f));
    const auto current_op_bg_color = ImGui::GetColorU32(ImVec4(53/255.0f, 140/255.0f, 79/255.0f, 1.0f));
    const auto future_ops_bg_color = ImGui::GetColorU32(ImVec4(53/255.0f, 76/255.0f, 140/255.0f, 1.0f));
    
    const static ImGuiTableFlags flags = ImGuiTableFlags_ScrollY | ImGuiTableFlags_ScrollX |
        ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders | ImGuiTableFlags_Resizable | 
        ImGuiTableFlags_NoHostExtendX | ImGuiTableFlags_SizingFixedSame;
    
    ImVec2 outer_size = ImVec2(0.0f, rows_to_show * text_base_height);
    if (ImGui::BeginTable("table_instructions", 3, flags, outer_size)) {
        // always show the header row
        ImGui::TableSetupScrollFreeze(0, 1);
        ImGui::TableSetupColumn("Opcode", ImGuiTableColumnFlags_None);
        ImGui::TableSetupColumn("Type", ImGuiTableColumnFlags_None);
        ImGui::TableSetupColumn("Description", ImGuiTableColumnFlags_None);
        ImGui::TableHeadersRow();

        ImGuiListClipper clipper;
        clipper.Begin(instructions_to_show);
        while (clipper.Step()) {
            for (int row = clipper.DisplayStart; row < clipper.DisplayEnd; ++row) {

                ImGui::TableNextRow();

                // calculate opcode and set row color
                u16 opcode = 0;
                if (row < past_opcodes.size()) {
                    opcode = past_opcodes[row];
                    ImGui::TableSetBgColor(ImGuiTableBgTarget_RowBg0, past_ops_bg_color);
                }
                else if (row == past_opcodes.size()) {
                    opcode = chip8.fetch_with_pc(chip8.pc());
                    ImGui::TableSetBgColor(ImGuiTableBgTarget_RowBg0, current_op_bg_color);
                }
                else {
                    auto offset = row - past_opcodes.size();
                    opcode = chip8.fetch_with_pc(chip8.pc() + 2*offset);
                    ImGui::TableSetBgColor(ImGuiTableBgTarget_RowBg0, future_ops_bg_color);
                }

                auto type = opcode_to_instruction_type(opcode);
                ImGui::TableNextColumn();
                ImGui::Text("0x%x", opcode);
                ImGui::TableNextColumn();
                ImGui::Text("%s", instruction_type_to_string(type));
                ImGui::TableNextColumn();
                ImGui::Text("%s", chip8.instruction_table(type));
            }
        }

        ImGui::EndTable();
        }
}

void Window::render_ImGui_frame(const std::vector<u16>& past_opcodes, const Chip8& chip8, u32 future_to_show) {
    debug_window_widget(past_opcodes, chip8, future_to_show);
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}


