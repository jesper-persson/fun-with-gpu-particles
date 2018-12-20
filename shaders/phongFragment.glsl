#version 400

uniform sampler2D diffuseTexture;
uniform sampler2D depthTexture;
uniform mat4 toLightSpace;

uniform int textureScale;

in vec2 texture_out;
in vec3 normal_out;
in vec3 frag_out;

in vec3 camera_pos_out;

out vec4 out_color;

void main() {
    // Lighting
    float ambient = 0.7;
    vec3 lightPositionWS = vec3(0, 4, 0); // Must be synced with main.cpp "light"
    vec3 dirToLight = normalize(lightPositionWS - frag_out);
    float intensity = max(dot(dirToLight, normalize(normal_out)), 0.0) + ambient;

    // Shadow mapping
    vec4 lightCoord = toLightSpace * vec4(frag_out.xyz, 1);
    vec3 projCoord = lightCoord.xyz / lightCoord.w;
    projCoord = projCoord * 0.5 + 0.5;
    vec4 texDepthV = texture(depthTexture, projCoord.xy);
    float bias = 0.005; // Fixes shadow acne
    if (projCoord.z - bias > texDepthV.r) {
        intensity *= 0.7;
    }   

    vec4 textureSample = texture(diffuseTexture, vec2(texture_out * textureScale)).xyzw;
    out_color = vec4(textureSample.xyzw) * intensity;

    // Mix distant objects with sky color to give impression that they fade away.
    float beginFade = 10.0;
    float endFade = 25.0;
    vec4 endColor = vec4(164/255.0, 184/255.0, 196/255.0, 0);
    float distance = length(camera_pos_out - frag_out);
    if (distance > beginFade && true) {
        float a = clamp( (distance-beginFade)/(endFade-beginFade), 0,  1);
        out_color = mix(out_color, endColor, a);
    }
}