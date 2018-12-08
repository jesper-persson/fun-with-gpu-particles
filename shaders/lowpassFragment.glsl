#version 400

layout(location = 0) out vec4 newValue;

uniform sampler2D tex1;

in vec2 texture_out;

out vec4 out_color;

float depthSize = 1024;

void main() {
    float textureSample1 = texture(tex1, vec2(texture_out)).x;

    float textureSample2 = texture(tex1, vec2(texture_out.x + 1/depthSize, texture_out.y)).x;
    float textureSample3 = texture(tex1, vec2(texture_out.x, texture_out.y + 1/depthSize)).x;

    float textureSample4 = texture(tex1, vec2(texture_out.x - 1/depthSize, texture_out.y)).x;
    float textureSample5 = texture(tex1, vec2(texture_out.x, texture_out.y - 1/depthSize)).x;

    float maxH = min(textureSample1, min(textureSample2, min(textureSample3, min(textureSample4, textureSample5))));

    

    vec4 textureSample = vec4(maxH, maxH, maxH, maxH);
    out_color = textureSample;
    newValue = out_color;
}