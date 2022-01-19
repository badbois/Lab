#include "App.h"

App::App(Cool::WindowManager& windows)
    : DefaultApp::DefaultApp{windows, [&](Cool::RenderTarget& render_target, float time) { render(render_target, time); }}
{
    _clock.pause();
#if IS0_TEST_NODES
    for (const auto& node_template : _shader_manager.nodes_template()) {
        _shader_manager.add_node(NodeFactoryU::node_from_template(node_template));
    }
#endif
}

void App::update()
{
    DefaultApp::update();
    if (inputs_are_allowed()) {
        _shader_manager->update();
    }
#if IS0_TEST_NODES
    glfwSetWindowShouldClose(_main_window.glfw(), true);
#endif
}

void App::render(Cool::RenderTarget& render_target, float time)
{
#if defined(COOL_VULKAN)
#elif defined(COOL_OPENGL)
#if IS0_TEST_NODES
    render_target.set_size({1, 1});
#endif
    render_target.render([&]() {
        const auto aspect_ratio = img::SizeU::aspect_ratio(render_target.current_size());
        _camera.apply(aspect_ratio);
        glClearColor(0.f, 0.f, 0.f, 0.f);
        glClear(GL_COLOR_BUFFER_BIT);
        _shader_manager->setup_for_rendering(*_camera, time);
        _shader_manager->render();
    });
#endif
}

void App::imgui_windows()
{
    DefaultApp::imgui_windows();
    if (inputs_are_allowed()) {
        _shader_manager.imgui_windows();
    }
}

void App::imgui_menus()
{
    DefaultApp::imgui_menus();
}

void App::on_keyboard_event(const Cool::KeyboardEvent& event)
{
    DefaultApp::on_keyboard_event(event);
    _shader_manager->on_key_pressed(event);
}

void App::on_mouse_button(const Cool::MouseButtonEvent<Cool::WindowCoordinates>& event)
{
    DefaultApp::on_mouse_button(event);
}

void App::on_mouse_scroll(const Cool::MouseScrollEvent<Cool::WindowCoordinates>& event)
{
    DefaultApp::on_mouse_scroll(event);
}

void App::on_mouse_move(const Cool::MouseMoveEvent<Cool::WindowCoordinates>& event)
{
    DefaultApp::on_mouse_move(event);
}