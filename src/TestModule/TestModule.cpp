#include "TestModule.h"
#include <Cool/Log/ToUser.h>

namespace Lab {

TestModule::TestModule(Registries& registries)
    : _colorId{registries.create(glm::vec3{1.f, 0.5f, 0.1f})}
{
}

void TestModule::imgui_windows(Ui ui)
{
    Ui::window({.name = "Test Module"}, [&]() {
        ui.widget("Color", _colorId);
    });
}

void TestModule::render(const Registries& registries)
{
    Cool::Log::ToUser::info("TestModule", "Re-rendering");
    if (_fullscreen_pipeline.shader()) {
        _fullscreen_pipeline.shader()->bind();
        const auto color = registries.get(_colorId);
        if (color) {
            _fullscreen_pipeline.shader()->set_uniform("_color", *color);
        }
        _fullscreen_pipeline.draw();
    }
}

} // namespace Lab
