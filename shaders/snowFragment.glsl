#version 400

uniform sampler2D tex1;
uniform sampler2D normalMap;

in vec2 texture_out;
in float num_collisions_out;
in vec3 frag_out;
in float isEdge;
in vec3 normal_out;
in vec3 camera_pos_out;

out vec4 out_color;

void main() {

    vec3 normal = normalize(normal_out); // texture(normalMap, vec2(texture_out * 4)).xyz;

    float ambient = 0.85; // High ambient because snow reflections easily
    vec3 lightPositionWS = vec3(30, 30, 0); // Must be synced with main.cpp "light"
    vec3 dirToLight = normalize(lightPositionWS - frag_out) * 0.1;
    float intensity = max(dot(dirToLight, normalize(normal)), 0.0) + ambient;

    // Specular
    vec3 reflection = reflect(-dirToLight, normal);
    vec3 toCam = normalize(camera_pos_out - frag_out);
    float cosAngle = max(0.0, dot(toCam, reflection));
    float shininess = 10;
    float specularCoefficient = pow(cosAngle, shininess);

    vec4 textureSample = texture(tex1, vec2(texture_out) * float(1)).xyzw * (intensity + specularCoefficient);
    out_color = vec4(textureSample.xyzw ) ; // * min(num_collisions_out / 10.0f, 1.0);


    if (num_collisions_out > 0) {
        out_color.w = num_collisions_out / (float(1));
    } else {
        out_color.w = 0;
    }

    // out_color.w = 0.5;

    //out_color = vec4(normal, 1);

    // out_color.r = isEdge;
    // out_color.w = 1;

    // if (isEdge < 1 && num_collisions_out > 0) {
    //     out_color.w = 1;
    // }

    // out_color.w = 1;

    // out_color = vec4(normal, 1);
}