#pragma once

#include <Cool/Uuid/Uuid.h>
#include "Ed.h"
#include "EdSerialization.h"

struct Link {
    ed::LinkId id = static_cast<ed::LinkId>(Cool::Uuid{});
    ed::PinId  from_pin_id;
    ed::PinId  to_pin_id;

private:
    //Serialization
    friend class cereal::access;
    template<class Archive>
    void serialize(Archive& archive)
    {
        archive(cereal::make_nvp("Link id", id),
                cereal::make_nvp("From pin", from_pin_id),
                cereal::make_nvp("To pin", to_pin_id));
    }
};