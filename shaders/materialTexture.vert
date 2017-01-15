#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(push_constant) uniform PushConstants
{
    mat4 mvp;
    mat4 transform;
} pushConstants;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inTextureCoord;

layout(binding = 2) uniform Light
{
    vec4 position;
    vec3 color;
} light;

layout(location = 0) out vec3 outPosition;
layout(location = 1) out vec3 outNormal;
layout(location = 2) out vec2 outTextureCoord;
layout(location = 3) out vec4 outLightPos;
layout(location = 4) out vec3 outLightColor;

out gl_PerVertex
{
    vec4 gl_Position;
};

void main()
{
    outTextureCoord = inTextureCoord;
    outNormal = inNormal;
    outPosition = inPosition;

    gl_Position = pushConstants.mvp * pushConstants.transform * vec4(inPosition, 1.0);
    
    vec4 worldPos = pushConstants.transform * vec4(inPosition, 1.0);
    outLightPos = vec4(light.position.xyz - worldPos.xyz, light.position.w);
    outLightColor = light.color;
}
