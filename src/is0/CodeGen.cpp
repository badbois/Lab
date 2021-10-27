#include "CodeGen.h"
#include <Cool/String/String.h>
#include <numeric>
#include "NodeEditorU.h"

namespace CodeGen {

static constexpr const char* default_sdf = R"(float is0_default_sdf(vec3 pos) {
    return MAX_DIST;
}

)";

static constexpr const char* ray_marcher_begin = R"(#version 430
#define MAX_DIST 1e5
in vec2 vTexCoords; 
uniform float _time; 
#include "Cool/res/shaders/camera.glsl" 
)";

static constexpr const char* ray_marcher_end = R"(

const float u_AbsorptionCoefficient = 0.1f;
const float u_AbsorptionCutoff = 0.01f;
const float u_MarchMultiplier = 0.5f;
const int   u_MaxVolumeMarchSteps = 200;
const int   u_MaxVolumeLightMarchSteps = 100;
const int   u_MaxSdfSphereSteps = 150;
const int   u_MaxOpaqueShadowMarchSteps = 200;

// Adapted from Christopher Wallis' great article https://wallisc.github.io/rendering/2020/05/02/Volumetric-Rendering-Part-2.html
#define PI 3.141592654

#define NUM_LIGHTS 3
#define NUM_LIGHT_COLORS 3

#define INVALID_MATERIAL_ID int(-1)
#define LARGE_NUMBER 1e20
#define EPSILON 0.0001
#define CAST_VOLUME_SHADOW_ON_OPAQUES 1

struct OrbLightDescription
{
    vec3 Position;
    vec3 LightColor;
};

vec3 GetLightColor(int lightIndex)
{
    switch(lightIndex % NUM_LIGHT_COLORS)
    {
        case 0: return vec3(255, 150, 243);
        case 1: return vec3(138, 249, 255);
    }
    return vec3(129, 255, 115);
}

OrbLightDescription GetLight(int lightIndex)
{
    const float lightMultiplier = 17.0f;
    float theta = _time * 0.7 + float(lightIndex) * PI * 2.0 / float(NUM_LIGHT_COLORS);
    float radius = 18.5f;
    
    OrbLightDescription orbLight;
    orbLight.Position = vec3(radius * cos(theta), 6.0 + sin(theta * 2.0) * 2.5, radius * sin(theta));
    orbLight.LightColor =  GetLightColor(lightIndex) * lightMultiplier / 20.;

    return orbLight;
}

float GetLightAttenuation(float distanceToLight)
{
    return 1.0 / pow(distanceToLight, 2.);
}

float IntersectVolumetric(in vec3 rayOrigin, in vec3 rayDirection, float maxT)
{
    // Precision isn't super important, just want a decent starting point before 
    // ray marching with fixed steps
	float precis = 0.5; 
    float t = 0.0f;
    for(int i=0; i<u_MaxSdfSphereSteps; i++ )
    {
	    float result = is0_main_sdf( rayOrigin+rayDirection*t);
        if( result < (precis) || t>maxT ) break;
        t += result;
    }
    return ( t>=maxT ) ? -1.0 : t;
}

vec3 Diffuse(in vec3 normal, in vec3 lightVec, in vec3 diffuse)
{
    float nDotL = dot(normal, lightVec);
    return clamp(nDotL * diffuse, 0.0, 1.0);
}

vec3 GetAmbientLight()
{
	return 10.2 * vec3(0.03, 0.018, 0.018);
}

float GetFogDensity(vec3 position, float sdfDistance)
{
    const float maxSDFMultiplier = 1.0;
    bool insideSDF = sdfDistance < 0.0;
    float sdfMultiplier = insideSDF ? min(abs(sdfDistance), maxSDFMultiplier) : 0.0;

    return sdfMultiplier;
}

float BeerLambert(float absorption, float dist)
{
    return exp(-absorption * dist);
}

float GetLightVisiblity(in vec3 rayOrigin, in vec3 rayDirection, in float maxT, in int maxSteps, in float marchSize)
{
    float t = 0.0f;
    float lightVisibility = 1.0f;
    float signedDistance = 0.0;
    for(int i = 0; i < maxSteps; i++)
    {                       
        t += max(marchSize, signedDistance);
        if(t > maxT || lightVisibility < u_AbsorptionCutoff) break;

        vec3 position = rayOrigin + t*rayDirection;

        signedDistance = is0_main_sdf(position);
        if(signedDistance < 0.0)
        {
            lightVisibility *= BeerLambert(u_AbsorptionCoefficient * GetFogDensity(position, signedDistance), marchSize);
        }
    }
    return lightVisibility;
}


float Luminance(vec3 color)
{
    return (color.r * 0.3) + (color.g * 0.59) + (color.b * 0.11);
}

bool IsColorInsignificant(vec3 color)
{
    const float minValue = 0.009;
    return Luminance(color) < minValue;
}

vec3 Render(vec3 rayOrigin, vec3 rayDirection)
{
    float depth = LARGE_NUMBER;
    vec3 opaqueColor = vec3(0.);
    
    float volumeDepth = IntersectVolumetric(rayOrigin, rayDirection, depth);
    float opaqueVisiblity = 1.0f;
    vec3 volumetricColor = vec3(0.0f);
    if(volumeDepth > 0.)
    {
        const vec3 volumeAlbedo = vec3(0.8);
        float marchSize = 0.6f * u_MarchMultiplier;
        float distanceInVolume = 0.0f;
        float signedDistance = 0.0;
        for(int i = 0; i < u_MaxVolumeMarchSteps; i++)
        {
            volumeDepth += max(marchSize, signedDistance);
            if(volumeDepth > depth || opaqueVisiblity < u_AbsorptionCutoff) break;
            
            vec3 position = rayOrigin + volumeDepth*rayDirection;

            signedDistance = is0_main_sdf(position);
			if(signedDistance < 0.0f)
            {
                distanceInVolume += marchSize;
                float previousOpaqueVisiblity = opaqueVisiblity;
                opaqueVisiblity *= BeerLambert(u_AbsorptionCoefficient * GetFogDensity(position, signedDistance), marchSize);
                float absorptionFromMarch = previousOpaqueVisiblity - opaqueVisiblity;
                
                for(int lightIndex = 0; lightIndex < NUM_LIGHTS; lightIndex++)
    			{
                    float lightVolumeDepth = 0.0f;
                    vec3 lightDirection = (GetLight(lightIndex).Position - position);
                    float lightDistance = length(lightDirection);
                    lightDirection /= lightDistance;
                    
                    vec3 lightColor = GetLight(lightIndex).LightColor * GetLightAttenuation(lightDistance); 
                    if(IsColorInsignificant(lightColor)) continue;
                    
                    float lightMarchSize = 0.65f * u_MarchMultiplier;
                    float lightVisiblity = GetLightVisiblity(position, lightDirection, lightDistance, u_MaxVolumeLightMarchSteps, lightMarchSize); 
                    volumetricColor += absorptionFromMarch * lightVisiblity * volumeAlbedo * lightColor;
                }
                volumetricColor += absorptionFromMarch * volumeAlbedo * GetAmbientLight();
            }
        }
    }
    
    return volumetricColor + opaqueVisiblity * opaqueColor;
}

vec3 GammaCorrect(vec3 color) 
{
    return pow(color, vec3(2.));
}

void main()
{
    vec3 ro = cool_ray_origin(); 
    vec3 rd = cool_ray_direction(); 
    gl_FragColor = vec4(GammaCorrect(Render(ro, rd)), 1.);
}
)";

static std::vector<std::pair<std::string, std::string>> compute_sdf_identifiers(const Node& node, const NodeTemplate& node_template, const std::vector<Node>& nodes, const std::vector<Link>& links)
{
    using namespace std::string_literals;
    std::vector<std::pair<std::string, std::string>> sdf_identifiers;
    sdf_identifiers.reserve(node.input_pins.size());
    for (size_t i = 0; i < node.input_pins.size(); ++i) {
        const Node* input_node = NodeEditorU::find_input_node(node.input_pins[i], nodes, links);
        sdf_identifiers.push_back(std::make_pair(
            node_template.sdf_identifiers[i],
            input_node ? function_name({input_node->node_template_name, input_node->uuid}) : "is0_default_sdf"s));
    }
    return sdf_identifiers;
}

static const NodeTemplate& find_node_template(const Node& node, const std::vector<NodeTemplate>& node_templates)
{
    return *std::ranges::find_if(node_templates, [&](const NodeTemplate& node_template) {
        return node_template.name == node.node_template_name;
    });
}

std::string full_shader_code(const std::vector<Node>& nodes, const std::vector<Link>& links, const std::vector<NodeTemplate>& node_templates)
{
    return ray_marcher_begin + std::string{default_sdf} + main_sdf(nodes, links, node_templates) + ray_marcher_end;
}

std::string main_sdf(const std::vector<Node>& nodes, const std::vector<Link>& links, const std::vector<NodeTemplate>& node_templates)
{
    std::stringstream declarations;
    std::stringstream definitions;
    std::stringstream main_sdf_definition;
    main_sdf_definition << R"(float is0_main_sdf(vec3 pos) {
    float d = MAX_DIST;)";

    for (const auto& node : nodes) {
        const auto& node_template       = find_node_template(node, node_templates);
        const auto  fn_signature_params = FnSignatureParams{.fn_name_params = FnNameParams{
                                                               .node_template_name = node.node_template_name,
                                                               .node_uuid          = node.uuid},
                                                           .sdf_param_declaration = node_template.vec3_input_declaration};
        declarations << function_declaration(fn_signature_params) << '\n';
        definitions << function_definition(FnDefinitionParams{
            .fn_signature_params = fn_signature_params,
            .body                = function_body(node.parameter_list,
                                  node_template.code_template,
                                  compute_sdf_identifiers(node, node_template, nodes, links))});
        definitions << "\n\n";
        if (NodeEditorU::has_no_successor(node, links)) {
            main_sdf_definition << "\n    d = min(d, " << function_name({node.node_template_name, node.uuid}) << "(pos));";
        }
    }
    main_sdf_definition << R"(
    return d;
})";
    return declarations.str() + '\n' + definitions.str() + main_sdf_definition.str();
}

std::string function_name(const FnNameParams& p)
{
    return std::string{p.node_template_name} + "_" + std::to_string(p.node_uuid);
}

std::string function_signature(const FnSignatureParams& p)
{
    return "float " + function_name(p.fn_name_params) + std::string{p.sdf_param_declaration};
}

std::string function_declaration(const FnSignatureParams& p)
{
    return function_signature(p) + ";";
}

std::string function_definition(const FnDefinitionParams& p)
{
    return function_signature(p.fn_signature_params) + std::string{p.body};
}

std::string function_body(const Cool::ParameterList& list, const std::string& code_template, const std::vector<std::pair<std::string, std::string>>& sdf_identifiers)
{
    return "{\n" +
           parameters_definitions(list) +
           Cool::String::replace({code_template, sdf_identifiers, "${", "}"}) +
           "\n}";
}

std::string parameters_definitions(const Cool::ParameterList& list)
{
    return std::accumulate(list->begin(), list->end(), std::string{""}, [](const std::string& acc, const auto& param) {
        return acc + "    " + parameter_definition_any(param) + '\n';
    });
}

std::string parameter_definition_any(const Cool::Parameter::Any& param)
{
    return std::visit([](auto&& param) { return parameter_definition(param); }, param);
}

} // namespace CodeGen