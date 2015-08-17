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

layout(std140) uniform LightBuffer
{
	vec4 lightColor;
	vec4 lightAmbient;
    vec3 lightPosition;
	float lightIntensity;
};

layout(location = 0) in vec3 vPosition;
layout(location = 1) in vec3 vNormal;
layout(location = 2) in vec3 vTangent;
layout(location = 3) in vec3 vBitangent;
layout(location = 4) in vec2 vUv;

out vec2 uv;
out vec3 lightDirection;

void main(void)
{
	vec4 viewPosition = viewMatrix * modelMatrix * vec4(vPosition, 1.0);

	vec3 n = normalize((normalMatrix * vec4(vNormal, 0.0)).xyz);
	vec3 t = normalize((normalMatrix * vec4(vTangent, 0.0)).xyz);
	vec3 b = normalize((normalMatrix * vec4(vBitangent, 0.0)).xyz);
	
	vec3 lightDir = (viewMatrix * vec4(lightPosition, 0.0)).xyz;
	lightDirection.x = dot(lightDir, t);
	lightDirection.y = dot(lightDir, b);
	lightDirection.z = dot(lightDir, n);

	gl_Position = projectionMatrix * viewPosition;
	uv = vUv;
}