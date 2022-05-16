#pragma once
#include <Cool/File/File.h>
#include <Cool/Gpu/FullscreenPipeline.h>
#include <Cool/Path/Path.h>
#include <range/v3/core.hpp>
#include <range/v3/view/concat.hpp>
#include <range/v3/view/single.hpp>
#include "../Module.h"
#include "Dependencies/InputSlot.h"
#include "Dependencies/Registries.h"

namespace Lab {

class Module_CustomShader : public Module {
public:
    explicit Module_CustomShader(Registries&);

    void render(InputProvider provider) override;
    void imgui_windows(Ui ui) override;

    // auto dependencies()
    // {
    //     return ranges::views::concat(
    //         ranges::single_view(AnyInputSlotRef(_testFloat)),
    //         ranges::single_view(AnyInputSlotRef(_color)),
    //         ranges::single_view(AnyInputSlotRef(_color2)));
    // }

    // auto dependencies() const
    // {
    //     return ranges::views::view(_parameters);
    // }

    auto depends_on(reg::AnyId id) const -> bool override
    {
        return std::ranges::any_of(_parameters, [&id](auto&& dep) {
            return std::visit([&id](auto&& dep) { return dep.id == id; }, dep);
        });
    }

private:
    void compile_shader(std::string_view path);
    void parse_shader_for_params(std::string_view path);

private:
    Cool::FullscreenPipeline  _fullscreen_pipeline{};
    std::vector<AnyInputSlot> _parameters;
    reg::Id<DirtyFlag>        _shader_is_dirty; // Must be before _file because it is used to construct it
    InputSlot_File            _file;

private:
    // Serialization
    // friend class cereal::access;
    // template<class Archive>
    // void save(Archive& archive) const
    // {
    //     archive(
    //         cereal::make_nvp("Shader Path", _file_watcher.path().string()),
    //         cereal::make_nvp("Parameters", _parameters));
    // }
    // template<class Archive>
    // void load(Archive& archive)
    // {
    //     std::string path;
    //     archive(
    //         path,
    //         _parameters);
    //     set_shader_path(path);
    // }
};

} // namespace Lab

CEREAL_REGISTER_TYPE(Lab::Module_CustomShader);