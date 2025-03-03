# ---HOW TO---

# ------------

import os
from pathlib import Path
from importlib.machinery import SourceFileLoader
generate_files = SourceFileLoader(
    "generate_files",
    os.path.join(Path(os.path.abspath(__file__)
                      ).parent.parent.parent, "Cool", "src", "Cool", "DebugOptions", "debug_options_generator.py")).load_module()


def all_debug_options():
    from generate_files import DebugOption, Kind
    return [
        DebugOption(
            name_in_code="show_framerate_window",
            name_in_ui="Framerate window",
            window_name="Framerate",
            kind=Kind.WINDOW,
            default_value=True,
        ),
        DebugOption(
            name_in_code="show_imgui_demo_window",
            name_in_ui="ImGui Demo window",
        ),
        DebugOption(
            name_in_code="show_commands_and_registries_debug_windows",
            name_in_ui="Commands and Registries windows",
        ),
        DebugOption(
            name_in_code="log_when_rendering",
            name_in_ui="Log when rendering",
        ),
        DebugOption(
            name_in_code="test_all_variable_widgets__window",
            name_in_ui="Test all Variable Widgets",
            kind=Kind.WINDOW,
        ),
        DebugOption(
            name_in_code="test_is0_nodes",
            name_in_ui="Test is0 Nodes",
            kind=Kind.BUTTON,
        ),
    ]


if __name__ == '__main__':
    from generate_files import generate_debug_options
    generate_debug_options(
        output_folder="generated",
        namespace="Lab",
        cache_file_name="cache/debug-options-lab",
        debug_options=all_debug_options(),
    )
