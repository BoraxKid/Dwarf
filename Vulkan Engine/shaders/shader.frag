#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 inFragColor;
layout(location = 1) in vec2 inFragTextureCoord;

layout(location = 0) out vec4 outColor;

layout(binding = 1) uniform sampler2D textureSampler;

void main()
{
	outColor = texture(textureSampler, inFragTextureCoord);
}
