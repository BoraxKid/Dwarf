#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(push_constant) uniform PushConstants
{
	mat4 mvp;
} pushConstants;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inTextureCoord;

layout(location = 0) out vec3 outFragColor;
layout(location = 1) out vec2 outFragTextureCoord;
layout(location = 2) out vec3 outDiffuseReflection;

out gl_PerVertex
{
	vec4 gl_Position;
};

struct lightSource
{
  vec4 position;
  vec4 diffuse;
};

lightSource light0 = lightSource(
    vec4(0.0, 0.0, 0.0, 0.0),
    vec4(1.0, 1.0, 1.0, 1.0)
);


void main()
{
	gl_Position = pushConstants.mvp * vec4(inPosition, 1.0);
	vec3 normalDirection = normalize(inNormal);
	vec3 lightDirection = normalize(vec3(light0.position));
	outDiffuseReflection = vec3(light0.diffuse) * max(0.0, dot(normalDirection, lightDirection));
	outFragColor = inNormal;
	outFragTextureCoord = inTextureCoord;
}
