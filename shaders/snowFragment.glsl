#version 400

uniform sampler2D tex1;
uniform sampler2D normalMap;

in vec2 texture_out;
in float num_collisions_out;
in vec3 frag_out;
in float isEdge;

out vec4 out_color;

void main() {

    vec3 normal = texture(normalMap, vec2(texture_out * 4)).xyz;

    float ambient = 0;
    vec3 lightPositionWS = vec3(40, 40, 40); // Must be synced with main.cpp "light"
    vec3 dirToLight = normalize(lightPositionWS - frag_out);
    float intensity = max(dot(dirToLight, normalize(normal)), 0.0) + ambient;

    vec3 textureSample = texture(tex1, vec2(texture_out)).xyz * intensity;
    out_color = vec4(textureSample.xyz, 1)  ; // * min(num_collisions_out / 10.0f, 1.0);


    if (num_collisions_out > 0) {
        out_color.w = num_collisions_out / 1.0f;
    } else {
        out_color.w = 0;
    }

    // out_color.r = isEdge;
    // out_color.w = 1;

    // if (isEdge < 1 && num_collisions_out > 0) {
    //     out_color.w = 1;
    // }

    // out_color.w = 1;

    // out_color = vec4(normal, 1);
}