/* -----------------------------------------------------------------------------
 * This file was automatically generated by a Python script.
 * PLEASE DON'T EDIT IT DIRECTLY, your changes would be overwritten the next time the script is run.
 * Instead, go to "generate_commands.py" and edit the "Command_SetCameraZoom" function there.
 * -----------------------------------------------------------------------------
 */

#include <stringify/stringify.hpp>
#include "CommandCore/CommandExecutionContext_Ref.h"
#include "CommandCore/LAB_REGISTER_COMMAND.h"
#include "CommandCore/LAB_REGISTER_REVERSIBLE_COMMAND.h"
#include "CommandCore/MakeReversibleCommandContext_Ref.h"

namespace Lab {

struct ReversibleCommand_SetCameraZoom;

struct Command_SetCameraZoom {
    float value{};

    void execute(CommandExecutionContext_Ref& ctx) const
    {
        ctx.camera_manager().set_zoom(value, ctx);
    }

    auto to_string() const -> std::string
    {
        return "Set camera zoom to " + Cool::stringify(value);
    }

    auto make_reversible(const MakeReversibleCommandContext_Ref& ctx) const -> ReversibleCommand_SetCameraZoom;
};

struct ReversibleCommand_SetCameraZoom {
    Command_SetCameraZoom forward_command{};
    float    old_value{};

    void execute(CommandExecutionContext_Ref& ctx) const
    {
        forward_command.execute(ctx);
    }

    void revert(CommandExecutionContext_Ref& ctx) const
    {
        ctx.camera_manager().set_zoom(old_value, ctx);
    }

    auto to_string() const -> std::string
    {
        return "Set camera zoom from " + Cool::stringify(old_value) +
               " to " + Cool::stringify(forward_command.value);
    }

    auto merge(const ReversibleCommand_SetCameraZoom& previous) const -> std::optional<ReversibleCommand_SetCameraZoom>
    {
        return ReversibleCommand_SetCameraZoom{
            .forward_command = forward_command,
            .old_value       = previous.old_value,
        };
    };
};

inline auto Command_SetCameraZoom::make_reversible(const MakeReversibleCommandContext_Ref& ctx) const
    -> ReversibleCommand_SetCameraZoom
{
    return ReversibleCommand_SetCameraZoom{
        .forward_command = *this,
        .old_value       = ctx.camera_manager().get_zoom(),
    };
}

} // namespace Lab

namespace cereal {

template<class Archive>
void serialize(Archive& archive, Lab::Command_SetCameraZoom& command)
{
    archive(cereal::make_nvp("camera zoom", command.value));
}

template<class Archive>
void serialize(Archive& archive, Lab::ReversibleCommand_SetCameraZoom& command)
{
    archive(
        cereal::make_nvp("camera zoom", command.forward_command.value),
        cereal::make_nvp("Old camera zoom", command.old_value)
    );
}

} // namespace cereal

LAB_REGISTER_COMMAND(Lab::Command_SetCameraZoom)
LAB_REGISTER_REVERSIBLE_COMMAND(Lab::ReversibleCommand_SetCameraZoom)
