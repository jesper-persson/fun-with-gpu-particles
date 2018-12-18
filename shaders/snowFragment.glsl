#version 400

uniform sampler2D tex1;
uniform sampler2D tex2;
uniform sampler2D tex3;
uniform sampler2D tex4;
uniform sampler2D normalMap;

in vec2 texture_out;
in float num_collisions_out;
in vec3 frag_out;
in float isEdge;
in vec3 normal_out;
in vec3 camera_pos_out;

out vec4 out_color;

int depthSize = 2048;

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

    vec4 textureSample = texture(tex1, vec2(texture_out) * float(1)).xyzw;
    out_color = vec4(textureSample.xyzw );

    vec4 textureSample1 = texture(tex1, vec2(texture_out) * float(1)).xyzw;
    vec4 textureSample2 = texture(tex2, vec2(texture_out) * float(0.25) * depthSize).xyzw;
    vec4 textureSample3 = texture(tex3, vec2(texture_out) * float(0.25) * depthSize).xyzw;
    vec4 textureSample4 = texture(tex4, vec2(texture_out) * float(0.25) * depthSize).xyzw;

    // if (num_collisions_out > 0) {
    //     out_color.w = num_collisions_out / (float(1));
    // } else {
    //     out_color.w = 0;
    // }

    int decider = int( mod( int(textureSample1.r * 100), 3) );
    if (decider == 0) {
        textureSample = textureSample2;
    } else if (decider == 1) {
        textureSample = textureSample3;
    } else if (decider == 2) {
        textureSample = textureSample4;
    }  

    float startFade = max(num_collisions_out - 0.3, 0);
    float endFade = num_collisions_out;
    if (textureSample.r >= num_collisions_out) {
        discard;
    }

    out_color = vec4(0.95,0.95,1,1) * (intensity + specularCoefficient);
    // out_color.a = 1;

    float alpha = (endFade - textureSample.r) / (endFade - startFade);
    out_color.a = mix(0, 1, alpha);
}