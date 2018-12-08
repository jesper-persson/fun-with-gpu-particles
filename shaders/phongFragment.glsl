#version 400

uniform sampler2D tex1;
uniform sampler2D depthTexture;
uniform mat4 toLightSpace;

uniform int textureScale;

in vec2 texture_out;
in vec3 normal_out;
in vec3 frag_out;

out vec4 out_color;

void main() {
    // Lighting
    float ambient = 0.5;
    vec3 lightPositionWS = vec3(0, 4, 0); // Must be synced with main.cpp "light"
    vec3 dirToLight = normalize(lightPositionWS - frag_out);
    float intensity = max(dot(dirToLight, normalize(normal_out)), 0.0) + ambient;

    // Shadow
    vec4 lightCoord = toLightSpace * vec4(frag_out.xyz, 1);
    vec3 projCoord = lightCoord.xyz / lightCoord.w;
    projCoord = projCoord * 0.5 + 0.5;
    vec4 texDepthV = texture(depthTexture, projCoord.xy);
    float bias = 0.005;
    if (projCoord.z - bias > texDepthV.r) {
        intensity *= 0.5;
    }   

    vec4 textureSample = texture(tex1, vec2(texture_out * textureScale)).xyzw;
    out_color = vec4(textureSample.xyzw) * intensity;
}