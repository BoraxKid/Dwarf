#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 inFragColor;
layout(location = 1) in vec2 inFragTextureCoord;

layout (set = 0, binding = 0) uniform UBO 
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

layout(binding = 1) uniform sampler2D textureSampler;

layout(location = 0) out vec4 outColor;

void main()
{
	outColor = vec4(ubo.Ks.rgb, ubo.d);
}
