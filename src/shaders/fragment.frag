#version 440 core
in vec2 Texcoord;
out vec4 FragColor;
uniform sampler2D texture_sampler;
uniform vec3 box_color;
void main()
{
    float alpha = texture(texture_sampler, Texcoord).r;
    FragColor = vec4(box_color, alpha);
}

