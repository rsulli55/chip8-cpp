#version 440 core
in vec2 Texcoord;
out vec4 FragColor;
uniform sampler2D texture_sampler;
uniform vec3 box_color = vec3(1.0, 0.0, 0.0);
void main()
{
    float alpha = texture(texture_sampler, Texcoord).r;
    FragColor = vec4(box_color, alpha);
}
