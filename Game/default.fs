#version 330 core

out vec4 color;

in vec2 uv0;

uniform sampler2D texture;
uniform float alpha;

void main()
{
    color = texture2D(texture, uv0);	
	color.a = color.a * alpha;
}