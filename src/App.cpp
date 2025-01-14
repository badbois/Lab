#include "App.h"
#include <Cool/DebugOptions/TestMessageConsole.h>
#include <Cool/DebugOptions/TestPresets.h>
#include <Cool/DebugOptions/TestVariables.h>
#include <Cool/Input/Input.h>
#include <Cool/Log/ToUser.h>
#include <Cool/Parameter/ParametersHistory.h>
#include <Cool/Path/Path.h>
#include <Cool/Time/ClockU.h>
#include <cmd/imgui.hpp>
#include <serv/serv.hpp>
#include <stringify/stringify.hpp>
#include "CommandCore/command_to_string.h"
#include "Debug/DebugOptions.h"
#include "Menus/menu_info.h"
#include "Module_CustomShader/Module_CustomShader.h"
#include "Module_is0/Module_is0.h"
#include "UI/imgui_show.h"

namespace Lab {

App::App(Cool::WindowManager& windows)
    : _camera_manager{_variable_registries.of<Cool::Variable<Cool::Camera>>().create({})}
    , _main_window{windows.main_window()}
    , _is0_view{_views.make_view("View | is0")}
    , _custom_shader_view{_views.make_view("View | Custom Shader")}
    , _is0_module{std::make_unique<Module_is0>(dirty_flag_factory(), input_factory())}
    , _custom_shader_module{std::make_unique<Module_CustomShader>(dirty_flag_factory(), input_factory())}
    , _texture{Cool::Path::root() + "/res/image resources/ETOILE5.png"}
{
    _camera_manager.hook_events(_is0_view.view.mouse_events(), _variable_registries, command_executor());
    _camera_manager.hook_events(_custom_shader_view.view.mouse_events(), _variable_registries, command_executor());
    // serv::init([](std::string_view request) {
    //     Cool::Log::Debug::info("Scripting", "{}", request);
    // });
    _clock.pause();
}

App::~App()
{
    // serv::shut_down();
}

void App::compile_all_is0_nodes()
{
    for (const auto& node_template : _is0_module->nodes_templates())
    {
        _is0_module->remove_all_nodes();
        Cool::Log::Debug::info("Test is0 Nodes", node_template.name);
        _is0_module->add_node(NodeFactoryU::node_from_template(node_template));
        _is0_module->recompile(update_context(), true);
    }
    _is0_module->remove_all_nodes();
}

void App::update()
{
    if (!_exporter.is_exporting())
    {
        _clock.update();
        for (auto& view : _views)
        {
            view.update_size(_preview_constraint);
        }
        polaroid().render(_clock.time());
    }
    else
    {
        _exporter.update(polaroid());
    }

    if (_last_time != _clock.time())
    {
        _last_time = _clock.time();

        set_dirty_flag()(_is0_module->dirty_flag());
        set_dirty_flag()(_custom_shader_module->dirty_flag());
    }
    if (_is0_view.render_target.needs_resizing())
    {
        set_dirty_flag()(_is0_module->dirty_flag());
    }
    if (_custom_shader_view.render_target.needs_resizing())
    {
        set_dirty_flag()(_custom_shader_module->dirty_flag());
    }

    if (inputs_are_allowed())
    {
        _is0_module->update(update_context());
        _custom_shader_module->update(update_context());
        check_inputs();
    }

#if DEBUG
    if (DebugOptions::test_is0_nodes())
    {
        compile_all_is0_nodes();
    }
#endif
}

auto App::all_inputs() -> Cool::AllInputRefsToConst
{
    auto vec  = _custom_shader_module->all_inputs();
    auto vec2 = _is0_module->all_inputs();
    for (const auto& x : vec2)
    {
        vec.push_back(x);
    }
    return vec;
}

Cool::Polaroid App::polaroid()
{
    return {
        .render_target = _custom_shader_view.render_target,
        .render_fn     = [this](Cool::RenderTarget& render_target, float time) {
            render(render_target, time);
        }};
}

auto App::inputs_are_allowed() const -> bool
{
    return !_exporter.is_exporting();
}

auto App::wants_to_show_menu_bar() const -> bool
{
    return !_exporter.is_exporting();
}

static void imgui_window_console()
{
    Cool::Log::ToUser::console().imgui_window();
#if DEBUG
    Cool::Log::Debug::console().imgui_window();
#endif
}

static void imgui_window_exporter(Cool::Exporter& exporter, Cool::Polaroid polaroid, float time)
{
    exporter.imgui_windows(polaroid, time);
}

void App::render_one_module(Module& some_module, Cool::RenderTarget& render_target, float time)
{
#if DEBUG
    if (DebugOptions::log_when_rendering())
    {
        Cool::Log::Debug::info(some_module.name() + " Rendering", "Rendered");
    }
#endif
    render_target.render([&]() {
        glClearColor(0.f, 0.f, 0.f, 0.f);
        glClear(GL_COLOR_BUFFER_BIT);
        const auto aspect_ratio = img::SizeU::aspect_ratio(render_target.desired_size());
        some_module.do_rendering(
            {
                input_provider(aspect_ratio, time),
                input_factory(),
                input_destructor(),
                is_dirty__functor(),
                set_clean__functor(),
            },
            update_context()
        );
    });
}

void App::render_is0(Cool::RenderTarget& render_target, float time, img::Size size)
{
    render_target.set_size(size);
    render_one_module(*_is0_module, render_target, time);
}

void App::render_custom_shader(Cool::RenderTarget& render_target, float time)
{
    _custom_shader_module->set_image_in_shader("_image", 0, _is0_view.render_target.get().texture_id());
    _custom_shader_module->set_image_in_shader("_texture", 1, _texture.ID());
    render_one_module(*_custom_shader_module, render_target, time);
}

void App::render(Cool::RenderTarget& render_target, float time)
{
    render_is0(_is0_view.render_target, time, render_target.desired_size());
    render_custom_shader(render_target, time);
}

void App::imgui_commands_and_registries_debug_windows()
{
    const auto the_ui = ui();
    the_ui.window({.name = "Registry of vec3"}, [&]() {
        imgui_show(_variable_registries.of<Cool::Variable<glm::vec3>>());
    });
    the_ui.window({.name = "Registry of float"}, [&]() {
        imgui_show(_variable_registries.of<Cool::Variable<float>>());
    });
    the_ui.window({.name = "Registry of int"}, [&]() {
        imgui_show(_variable_registries.of<Cool::Variable<int>>());
    });
    the_ui.window({.name = "Registry of Camera"}, [&]() {
        imgui_show(_variable_registries.of<Cool::Variable<Cool::Camera>>());
    });
    the_ui.window({.name = "Registry of DirtyFlag"}, [&]() {
        imgui_show(_dirty_registry);
    });
    the_ui.window({.name = "History"}, [&]() {
        _history.imgui_show([](const ReversibleCommand& command) {
            return command_to_string(command);
        });
    });
    the_ui.window({.name = "Command Logger"}, [&]() {
        _command_logger.imgui_show();
    });
}

void App::imgui_windows()
{
    _is0_view.imgui_window();
    _custom_shader_view.imgui_window();

    imgui_window_exporter(_exporter, polaroid(), _clock.time());

    imgui_window_console();

    if (inputs_are_allowed())
    {
        const auto the_ui = ui();
        _is0_module->imgui_windows(the_ui);
        _custom_shader_module->imgui_windows(the_ui);
        // Time
        ImGui::Begin("Time");
        Cool::ClockU::imgui_timeline(_clock);
        ImGui::End();
        // Camera
        ImGui::Begin("Camera");
        _camera_manager.imgui(_variable_registries, command_executor());
        ImGui::End();
#if DEBUG
        DebugOptions::show_framerate_window([&] {
            ImGui::Text("%.1f FPS", ImGui::GetIO().Framerate);
            _main_window.imgui_cap_framerate();
        });
        if (DebugOptions::show_imgui_demo_window())                         // Show the big demo window (Most of the sample code is
        {                                                                   // in ImGui::ShowDemoWindow()! You can browse its code
            ImGui::ShowDemoWindow(&DebugOptions::show_imgui_demo_window()); // to learn more about Dear ImGui!).
        }
        if (DebugOptions::show_commands_and_registries_debug_windows())
        {
            imgui_commands_and_registries_debug_windows();
        }
        DebugOptions::test_all_variable_widgets__window(&Cool::test_variables);

        Cool::DebugOptions::test_message_console__window([]() {
            static auto test_message_console = Cool::TestMessageConsole{};
            test_message_console.imgui(
                Cool::Log::ToUser::console()
            );
        });

        Cool::DebugOptions::test_presets__window([]() {
            static auto test_presets = TestPresets{};
            test_presets.imgui();
        });
#endif // DEBUG
    }
}

void App::menu_preview()
{
    if (ImGui::BeginMenu("Preview"))
    {
        if (_preview_constraint.imgui())
        {
            // render_impl(_view.render_target, *_current_module, _clock.time());
        }
        ImGui::EndMenu();
    }
}

void App::menu_windows()
{
    if (ImGui::BeginMenu("Windows"))
    {
        for (auto& view : _views)
        {
            view.view.imgui_open_close_checkbox();
        }
        ImGui::EndMenu();
    }
}

void App::menu_export()
{
    if (ImGui::BeginMenu("Export"))
    {
        _exporter.imgui_menu_items();
        ImGui::EndMenu();
    }
}

void App::menu_settings()
{
    if (ImGui::BeginMenu("Settings"))
    {
        _history.imgui_max_size();
        ImGui::Separator();
        ImGui::Separator();
        ImGui::Separator();
        _history.imgui_max_saved_size();
        ImGui::Separator();
        ImGui::Separator();
        ImGui::Separator();
        _theme_manager.imgui();
        ImGui::EndMenu();
    }
}

void App::menu_debug()
{
#if DEBUG
    static bool was_closed_last_frame{true}; // HACK(JF) I guess a `static` here is okay because no one is gonna want two distinct instances of the same debug menu O:) A better solution would be to make a small Menu class that would remember if it was open last frame or not.
    if (ImGui::BeginMenu("Debug"))
    {
        DebugOptionsManager::imgui_ui_for_all_options(was_closed_last_frame);
        was_closed_last_frame = false;
        ImGui::EndMenu();
    }
    else
    {
        was_closed_last_frame = true;
    }
#endif
}

void App::imgui_menus()
{
    menu_preview();
    menu_windows();
    menu_export();
    menu_settings();
    menu_info();
    menu_debug();
}

void App::check_inputs()
{
    if (!ImGui::GetIO().WantTextInput)
    {
        check_inputs__history();
        check_inputs__export_windows();
    }
}

void App::check_inputs__history()
{
    auto        exec = reversible_command_executor_without_history();
    const auto& io   = ImGui::GetIO();

    // Undo
    if (io.KeyCtrl && ImGui::IsKeyPressed(ImGuiKey_Z))
    {
        _history.move_backward(exec);
        Cool::ParametersHistory::get().move_backward();
        // TODO(JF) Remove this hack.
        // Force recompilation of all shaders when a variable changes to allow gradient variables to update (bc they need to generate shader code)
        {
            auto&            registry = _dirty_registry;
            std::unique_lock lock{registry.mutex()};
            for (auto& [_, is_dirty] : registry)
            {
                is_dirty.is_dirty = true;
            }
        }
    }

    // Redo
    if ((io.KeyCtrl && ImGui::IsKeyPressed(ImGuiKey_Y)) ||
        (io.KeyCtrl && io.KeyShift && ImGui::IsKeyPressed(ImGuiKey_Z)))
    {
        _history.move_forward(exec);
        Cool::ParametersHistory::get().move_forward();
        // TODO(JF) Remove this hack.
        // Force recompilation of all shaders when a variable changes to allow gradient variables to update (bc they need to generate shader code)
        {
            auto&            registry = _dirty_registry;
            std::unique_lock lock{registry.mutex()};
            for (auto& [_, is_dirty] : registry)
            {
                is_dirty.is_dirty = true;
            }
        }
    }
}

void App::check_inputs__export_windows()
{
    const auto& io = ImGui::GetIO();

    if (ImGui::IsKeyReleased(ImGuiKey_S) && io.KeyCtrl)
    {
        _exporter.image_export_window().open();
    }
    if (ImGui::IsKeyReleased(ImGuiKey_E) && io.KeyCtrl)
    {
        _exporter.video_export_window().open();
    }
}

void App::on_mouse_button(const Cool::MouseButtonEvent<Cool::WindowCoordinates>& event)
{
    for (auto& view : _views)
    {
        view.view.dispatch_mouse_button_event(view_event(event, view));
    }
}

void App::on_mouse_scroll(const Cool::MouseScrollEvent<Cool::WindowCoordinates>& event)
{
    for (auto& view : _views)
    {
        view.view.dispatch_mouse_scroll_event(view_event(event, view));
    }
}

void App::on_mouse_move(const Cool::MouseMoveEvent<Cool::WindowCoordinates>& event)
{
    for (auto& view : _views)
    {
        view.view.dispatch_mouse_move_event(view_event(event, view));
    }
}

} // namespace Lab