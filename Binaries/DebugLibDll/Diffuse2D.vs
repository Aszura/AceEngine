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
};

layout(location = 0) in vec2 vPosition;
layout(location = 1) in vec2 vSize;

out vec2 uv;

void main(void)
{
	gl_Position = projectionMatrix *  vec4(vPosition * vSize, 0.0, 1.0);
	uv = vPosition;
}