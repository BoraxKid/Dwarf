#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inTextureCoord;

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

layout(binding = 1) uniform sampler2D textureSampler;

layout(location = 0) out vec4 outColor;

struct lightSource
{
  vec3 position;
  vec3 diffuse;
};

lightSource light0 = lightSource(
    vec3(0.0, 0.0, 10.0),
    vec3(1.0, 1.0, 1.0)
);

void main()
{
	//outColor = texture(textureSampler, inFragTextureCoord) * vec4(inDiffuseReflection, 1.0) * ubo.Kd * ubo.illum;
	vec3 normal = normalize(inNormal);

	vec3 surfaceToLight = light0.position - inPosition;

	float brightness = dot(inNormal, surfaceToLight) / (length(surfaceToLight) * length(inNormal));
	brightness = clamp(brightness, 0, 1);

	vec4 surfaceColor = texture(textureSampler, inTextureCoord);
	outColor = vec4(brightness * light0.diffuse * surfaceColor.rgb, surfaceColor.a);
}
