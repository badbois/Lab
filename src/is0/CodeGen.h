#pragma once

#include <Cool/Uuid/Uuid.h>

namespace CodeGen {

struct FnNameParams {
    std::string_view node_template_name;
    Cool::Uuid       node_uuid;
};

struct FnSignatureParams {
    const FnNameParams& fn_name_params;
    std::string_view    sdf_param_declaration;
};

struct FnDefinitionParams {
    const FnSignatureParams& fn_signature_params;
    std::string_view         body;
};

std::string function_name(const FnNameParams& p);
std::string function_signature(const FnSignatureParams& p);
std::string function_declaration(const FnSignatureParams& p);
std::string function_definition(const FnDefinitionParams& p);

} // namespace CodeGen

TEST_CASE("[is0::CodeGen] Function generation")
{
    // Given
    const auto name              = std::string{"my_sdf"};
    const auto name_params       = CodeGen::FnNameParams{name,
                                                   Cool::Uuid{42}};
    const auto signature_params  = CodeGen::FnSignatureParams{name_params,
                                                             "(vec3 pos)"};
    const auto definition_params = CodeGen::FnDefinitionParams{signature_params,
                                                               "{ return 1.; }"};
    // Then
    CHECK(CodeGen::function_name(name_params) == "my_sdf_42");
    CHECK(CodeGen::function_signature(signature_params) == "float my_sdf_42(vec3 pos)");
    CHECK(CodeGen::function_declaration(signature_params) == "float my_sdf_42(vec3 pos);");
    CHECK(CodeGen::function_definition(definition_params) == "float my_sdf_42(vec3 pos){ return 1.; }");
}