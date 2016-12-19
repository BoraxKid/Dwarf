#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 inFragColor;
layout(location = 1) in vec2 inFragTextureCoord;
layout(location = 2) in vec3 inDiffuseReflection;

layout (binding = 0) uniform UBO 
{
	vec4 Ka; // ambient
	vec4 Kd; // diffuse
	vec4 Ks; // specular
	vec4 Tf; // transmittance
	vec4 Ke; // emission
	float Ns; // shininess
	float Ni; // ior
	float d; // dissolve
	int illum; // illum
} ubo;

layout(location = 0) out vec4 outColor;

void main()
{
	if (ubo.d < 1.0)
		discard;
	outColor = vec4(inDiffuseReflection, 1.0) * ubo.Kd * ubo.illum;
}
