#version 330

layout(std140) uniform StaticBuffer
{
    mat4 projectionMatrix;
    mat4 viewMatrix;
};

layout(std140) uniform DynamicBuffer
{
    mat4 modelMatrix;
	vec4 color;
	vec4 specular;
	vec4 ambient;
	float shininess;
};

layout(location = 0) in vec3 vPosition;
layout(location = 1) in vec3 vNormal;
layout(location = 2) in vec3 vTangent;
layout(location = 3) in vec3 vBitangent;
layout(location = 4) in vec2 vUv;

out vec2 uv;
out mat3 tbn;

void main(void)
{
	gl_Position = projectionMatrix * viewMatrix * modelMatrix * vec4(vPosition, 1.0);
	uv = vUv;

	vec3 n = normalize((modelMatrix * vec4(vNormal, 0.0)).xyz);
	vec3 t = normalize((modelMatrix * vec4(vTangent, 0.0)).xyz);
	vec3 b = normalize((modelMatrix * vec4(vBitangent, 0.0)).xyz);
	tbn = mat3(t, b, n);
}