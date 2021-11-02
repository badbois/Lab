#pragma once

#include "Node.h"
#include "NodeTemplate.h"

class NodeFactory {
public:
    explicit NodeFactory(std::string_view nodes_folder_path);
    std::optional<Node>              imgui();
    const std::vector<NodeTemplate>& templates() const { return _node_templates; }

private:
    std::vector<NodeTemplate> _node_templates;
};

namespace NodeFactoryU {

Node node_from_template(const NodeTemplate& node_template);

}