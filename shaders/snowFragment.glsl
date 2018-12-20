#version 400

uniform sampler2D snowNoiseDecider;
uniform sampler2D snowNoise1;
uniform sampler2D snowNoise2;
uniform sampler2D snowNoise3;

in vec2 texture_out;
in float snow_accumulated_out;
in vec3 frag_out;
in vec3 normal_out;
in vec3 camera_pos_out;

out vec4 out_color;

int depthSize = 2048;

void main() {
    vec3 normal = normalize(normal_out);

    // Diffuse lighting
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

    vec4 textureSample = texture(snowNoiseDecider, vec2(texture_out) * float(1)).xyzw;
    vec4 textureSample1 = texture(snowNoise1, vec2(texture_out) * float(0.25) * depthSize).xyzw;
    vec4 textureSample2 = texture(snowNoise2, vec2(texture_out) * float(0.25) * depthSize).xyzw;
    vec4 textureSample3 = texture(snowNoise3, vec2(texture_out) * float(0.25) * depthSize).xyzw;

    vec4 noiseSample;
    int decider = int( mod( int(textureSample.r * 100), 3) );
    if (decider == 0) {
        noiseSample = textureSample1;
    } else if (decider == 1) {
        noiseSample = textureSample2;
    } else if (decider == 2) {
        noiseSample = textureSample3;
    }  

    // Use noise texture, and interpolation of how much snow has fallen on
    // this fragment, to decide if the fragment should be drawn or not.
    if (noiseSample.r >= snow_accumulated_out) {
        discard;
    }

    // A sligh bluish color for the snow.
    out_color = vec4(0.95,0.95,1,1) * (intensity + specularCoefficient);

    // Fade edge of noise to make the snow smooth.
    float startFade = max(snow_accumulated_out - 0.3, 0);
    float endFade = snow_accumulated_out;
    float alpha = (endFade - noiseSample.r) / (endFade - startFade);
    out_color.a = mix(0, 1, alpha);
}