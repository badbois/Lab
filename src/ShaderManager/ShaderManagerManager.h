#pragma once

#include <is0/Is0.h>
#include "ShaderManager_FromText.h"

class ShaderManagerManager {
public:
    void imgui_windows();

    // clang-format off
    ShaderManager& operator*() { if(_use_nodes) return _is0; else return _from_text; }
    ShaderManager* operator->() { return &operator*(); }
    // clang-format on

    inline void                             add_node(const Node& node) { _is0.add_node(node); }
    inline const std::vector<NodeTemplate>& nodes_templates() { return _is0.nodes_templates(); }

private:
    bool                   _use_nodes = true;
    Is0                    _is0;
    ShaderManager_FromText _from_text;

private:
    // Serialization
    friend class cereal::access;
    template<class Archive>
    void serialize(Archive& archive)
    {
        archive(
            cereal::make_nvp("Is using nodes", _use_nodes),
            cereal::make_nvp("From Nodes", _is0),
            cereal::make_nvp("From Text", _from_text));
    }
};