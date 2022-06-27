# ---HOW TO---
# Create a function with the same name as the file you want to generate
# that returns a string with the content you want to generate.
#
# Then call `generate("my_function_name")`.
#
# You can use `all_variable_types()` to get all the variable types we use in CoolLab.
# ------------


def clear_generated_folder():
    import shutil
    import os
    shutil.rmtree(output_folder())
    os.makedirs(output_folder())


def output_folder():
    import os
    from pathlib import Path
    root_folder = Path(__file__).parent
    return os.path.join(root_folder, "src/generated")


def generate(name):
    generate_file(name, eval(f"{name}()"))


def generate_file(name, content):
    import os
    with open(os.path.join(output_folder(), name) + ".inl", 'w') as f:
        f.write(heading(name) + content)


def heading(function_name):
    import os
    return f"""/* -----------------------------------------------------------------------------
 * This file was automatically generated by a Python script.
 * PLEASE DON'T EDIT IT DIRECTLY, your changes would be overwritten the next time the script is run.
 * Instead, go to "{os.path.basename(__file__)}" and edit the "{function_name}" function there.
 * -----------------------------------------------------------------------------
 */
"""


def all_variable_types():
    return [
        "int",
        "float",
        "glm::vec2",
        "glm::vec3",
        "Cool::RgbColor",
        "Cool::Camera",
        "Cool::Angle",
    ]


def all_variable_includes():
    return f"""
#include <Cool/Camera/Camera.h>
#include <Cool/Variables/Variables.h>
"""


def register_set_variable_commands():
    commands = ""
    reversible_commands = ""
    for variable_type in all_variable_types():
        commands += f"LAB_REGISTER_COMMAND(Lab::Command_SetVariable<{variable_type}>)\n"
        reversible_commands += f"LAB_REGISTER_REVERSIBLE_COMMAND(Lab::ReversibleCommand_SetVariable<{variable_type}>)\n"
    return f"""
{commands}
{reversible_commands}
"""


def register_set_variable_metadata_commands():
    commands = "\n"
    for variable_type in all_variable_types():
        commands += f"LAB_REGISTER_COMMAND(Lab::Command_SetVariableMetadata<{variable_type}>)\n"
    return commands


def VariableRegistries():
    return "\n" + "using VariableRegistries = reg::Registries<\n" + ",\n".join(
        map(lambda var_type: f"    Cool::Variable<{var_type}>", all_variable_types())) + "\n>;"


def AnyInput():
    return "\n" + "using AnyInput = std::variant<\n" + ",\n".join(
        map(lambda var_type: f"    Input<{var_type}>", all_variable_types())) + "\n>;"


def AnyInputRef():
    return "\n" + "using AnyInputRef = std::variant<\n" + ",\n".join(
        map(lambda var_type: f"    std::reference_wrapper<Input<{var_type}>>", all_variable_types())) + "\n>;"


def AnyInputRefToConst():
    return "\n" + "using AnyInputRefToConst = std::variant<\n" + ",\n".join(
        map(lambda var_type: f"    std::reference_wrapper<const Input<{var_type}>>", all_variable_types())) + "\n>;"


clear_generated_folder()
generate("register_set_variable_commands")
generate("register_set_variable_metadata_commands")
generate("VariableRegistries")
generate("AnyInput")
generate("AnyInputRef")
generate("AnyInputRefToConst")
generate("all_variable_includes")
