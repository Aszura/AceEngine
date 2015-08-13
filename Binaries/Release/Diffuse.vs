#version 330

layout(std140) uniform StaticBuffer
{
    mat4 projectionMatrix;
    mat4 viewMatrix;
};

layout(std140) uniform DynamicBuffer
{
    mat4 modelMatrix;
    mat4 normalMatrix;
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
out vec3 normal;

void main(void)
{
	gl_Position = projectionMatrix * viewMatrix * modelMatrix * vec4(vPosition, 1.0);
	normal = vNormal;
	uv = vUv;
}