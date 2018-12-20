#version 400

uniform sampler2D tex1;

in vec2 texture_out;

out vec4 out_color;

float depthSize = 2048;

void main() {
    float kernel[9] = float[] (0.0625, 0.125, 0.0625, 0.125, 0.25, 0.125, 0.0625, 0.125, 0.0625);
    vec3 pixels[9];

    float step =  1 / depthSize;

    pixels[0] = texture(tex1, vec2(texture_out.x - step, texture_out.y + step)).xyz;
    pixels[1] = texture(tex1, vec2(texture_out.x, texture_out.y + step)).xyz;
    pixels[2] = texture(tex1, vec2(texture_out.x + step, texture_out.y + step)).xyz;
    
    pixels[3] = texture(tex1, vec2(texture_out.x - step, texture_out.y)).xyz;
    pixels[4] = texture(tex1, vec2(texture_out)).xyz;
    pixels[5] = texture(tex1, vec2(texture_out.x + step, texture_out.y)).xyz;

    pixels[6] = texture(tex1, vec2(texture_out.x - step, texture_out.y - step)).xyz;
    pixels[7] = texture(tex1, vec2(texture_out.x, texture_out.y - step)).xyz;
    pixels[8] = texture(tex1, vec2(texture_out.x + step, texture_out.y - step)).xyz;

    int kernelSize = 9;
    vec3 sum = vec3(0, 0, 0);
    for (int i = 0; i < kernelSize; i++) {
        sum += kernel[i] * pixels[i];
    }

    sum = normalize(sum);
    out_color = vec4(sum.xyz, 1);
}