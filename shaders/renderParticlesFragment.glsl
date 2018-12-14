#version 400

uniform sampler2D tex1;


in vec2 texture_out;

in float timeLeft;
out vec4 out_color;

void main() {
    vec4 textureSample = texture(tex1, vec2(texture_out)).xyzw;
    out_color = vec4(textureSample.xyzw);

    out_color.w = out_color.w * min(1, timeLeft);
}