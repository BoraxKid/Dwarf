#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(push_constant) uniform PushConstants
{
	mat4 mvp;
} pushConstants;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec2 inTextureCoord;

layout(location = 0) out vec3 outFragColor;
layout(location = 1) out vec2 outFragTextureCoord;
out gl_PerVertex
{
    vec4 gl_Position;
};

void main()
{
    gl_Position = pushConstants.mvp * vec4(inPosition, 1.0);
    outFragColor = inColor;
	outFragTextureCoord = inTextureCoord;
}
