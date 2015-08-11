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

out vec3 uv;

void main(void)
{
	vec4 position = projectionMatrix * viewMatrix * modelMatrix * vec4(vPosition, 1.0);
	gl_Position = position.xyww;
	uv = vPosition;
}