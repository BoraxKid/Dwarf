#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(push_constant) uniform PushConstants
{
	mat4 mvp;
} pushConstants;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inTextureCoord;

layout(location = 0) out vec3 outPosition;
layout(location = 1) out vec3 outNormal;
layout(location = 2) out vec2 outTextureCoord;

out gl_PerVertex
{
	vec4 gl_Position;
};

void main()
{
	outTextureCoord = inTextureCoord;
	outNormal = inNormal;
	outPosition = inPosition;

	gl_Position = pushConstants.mvp * vec4(inPosition, 1.0);
}
