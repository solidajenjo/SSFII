#version 330 core

out vec4 color;

in vec2 uv0;

uniform sampler2D texture;
uniform float alpha;
uniform vec3 colorInput;

void main()
{
    color = texture2D(texture, uv0) * vec4(colorInput, 1.0f);	
	color.a = color.a * alpha;
}