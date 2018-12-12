#version 400

layout(location = 0) out vec4 newValue;

uniform sampler2D tex1;

in vec2 texture_out;

out vec4 out_color;

float depthSize = 2048;

void main() {
    vec3 t1 = texture(tex1, vec2(texture_out)).xyz;

    vec3 t2 = texture(tex1, vec2(texture_out.x + 1/depthSize, texture_out.y)).xyz;
    vec3 t3 = texture(tex1, vec2(texture_out.x, texture_out.y + 1/depthSize)).xyz;

    vec3 t4 = texture(tex1, vec2(texture_out.x - 1/depthSize, texture_out.y)).xyz;
    vec3 t5 = texture(tex1, vec2(texture_out.x, texture_out.y - 1/depthSize)).xyz;

    vec3 t6 = texture(tex1, vec2(texture_out.x + 2/depthSize, texture_out.y)).xyz;
    vec3 t7 = texture(tex1, vec2(texture_out.x, texture_out.y + 2/depthSize)).xyz;

    vec3 t8 = texture(tex1, vec2(texture_out.x - 2/depthSize, texture_out.y)).xyz;
    vec3 t9 = texture(tex1, vec2(texture_out.x, texture_out.y - 2/depthSize)).xyz;

    
    
//    vec3 sum = (0.250 * t1 + 0.09375 * t2 + 0.09375 * t3 + 0.09375 * t4 + 0.09375 * t5 + 0.09375 * t6 + 0.09375 * t6 + 0.09375 * t6 + 0.09375 * t9);// / (float(5));
    vec3 sum = (0.5 * t1 + 0.125 * t2 + 0.125 * t3 + 0.125 * t4 + 0.125 * t5);// / (float(5));

    sum = normalize(sum);
    

    out_color = vec4(sum.xyz, 1);
    newValue = out_color;
}