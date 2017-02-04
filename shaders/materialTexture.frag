#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inTextureCoord;
layout(location = 3) in vec4 inLightPos;
layout(location = 4) in vec3 inLightColor;

layout(binding = 0) uniform UBO 
{
    vec3 Ka; // ambient
    vec3 Kd; // diffuse
    vec3 Ks; // specular
    vec3 Tf; // transmittance
    vec3 Ke; // emission
    float Ns; // shininess
    float Ni; // ior
    float d; // dissolve
    int illum; // illum
} ubo;

layout(binding = 1) uniform sampler2D textureSampler;

layout(location = 0) out vec4 outColor;

#define MAX_LIGHT_DIST 9.0 * 9.0

void main()
{
    vec4 surfaceColor = texture(textureSampler, inTextureCoord);
    float lRadius =  MAX_LIGHT_DIST * inLightPos.w;
    float dist = min(dot(inLightPos, inLightPos), lRadius) / lRadius;
    float distFactor = 1.0 - dist;
    vec3 diffuse = inLightColor * distFactor;
    if (ubo.d < 1.0)
        discard;
    outColor = surfaceColor * vec4(diffuse, 1.0) * vec4(ubo.Kd, 1.0) * ubo.illum;
}
