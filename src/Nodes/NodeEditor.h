#pragma once

#include <imgui-node-editor/imgui_node_editor.h>

namespace ed = ax::NodeEditor;

class NodeEditor {
public:
	NodeEditor();
	~NodeEditor();

	void update();

private:
	ed::EditorContext* _context;
};