#pragma once

#include <Cool/Parameter/Parameter.h>
#include <Cool/Parameter/ParameterList.h>

struct RenderEffect {
    std::string         name;
    std::string         extra_code;
    std::string         parameters_declaration;
    std::string         code_template;
    Cool::ParameterList parameters;
    bool                is_active = true;

private:
    friend class cereal::access;
    template<class Archive>
    void serialize(Archive& archive)
    {
        archive(cereal::make_nvp("Effect", name),
                cereal::make_nvp("Parameters", parameters),
                cereal::make_nvp("Enabled", is_active));
    }
};