#version 330 core

out vec4 color;

in vec2 uv0;

uniform sampler2D texture;

void main()
{
    color = texture2D(texture, uv0);
	if (color.a < 0.9)
		discard;
}