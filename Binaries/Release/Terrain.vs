#version 410 core

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

uniform float uMaxTerrainHeight;
uniform sampler2D uHeightMap;

layout(location = 0) in vec2 vPosition;
layout(location = 1) in vec3 vNormal;
layout(location = 2) in vec2 vUv;

out vec2 uv;
out vec3 normal;

void main(void)
{
	normal = vNormal;
	uv = vUv;
	float height = texture(uHeightMap, uv).r * uMaxTerrainHeight;
	vec4 worldPos = modelMatrix * vec4(vPosition.x, height, vPosition.y, 1.0);
	gl_Position = projectionMatrix * viewMatrix * worldPos;
}