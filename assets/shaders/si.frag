#version 330 core
in vec2 fraguv;
uniform sampler2D texture_map;
out vec4 color;
void main()
{
    color = texture2D(texture_map, fraguv);
}
