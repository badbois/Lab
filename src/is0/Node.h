#pragma once

#include <Cool/Parameter/ParameterList.h>
#include <Cool/Uuid/Uuid.h>
#include "Pin.h"

struct Node {
    Cool::Uuid            uuid{};
    std::string           node_template_name;
    Cool::ParameterList   parameter_list{};
    std::vector<PinSdfIn> input_pins;
    PinSdfOut             output_pin;

    ed::NodeId id() const { return static_cast<ed::NodeId>(uuid); }

private:
    //Serialization
    friend class cereal::access;
    template<class Archive>
    void serialize(Archive& archive)
    {
        archive(cereal::make_nvp("Uuid", uuid),
                cereal::make_nvp("Node Template", node_template_name),
                cereal::make_nvp("Parameters", parameter_list),
                cereal::make_nvp("Input pins", input_pins),
                cereal::make_nvp("Output pin", output_pin));
    }
};
