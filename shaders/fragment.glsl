#version 400

uniform sampler2D tex;

in vec2 texture_out;

out vec4 out_color;

void main() {
    vec3 textureSample = texture(tex, vec2(texture_out)).xyz;
    out_color = vec4(textureSample.xyz, 1);
}