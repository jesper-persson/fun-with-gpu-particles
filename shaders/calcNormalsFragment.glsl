#version 400

layout(location = 0) out vec4 normal;

in vec2 texture_out;

uniform sampler2D depthTexture;

int depthSize = 2048;

void main(){

    float size = 0.00055;

    // Calc normal
    float v1 = -texture(depthTexture, texture_out).x;
    float v2 = -texture(depthTexture, texture_out + vec2(1/float(depthSize), 0)).x;
    float v3 = -texture(depthTexture, texture_out + vec2(0, 1/float(depthSize))).x;
    float v4 = -texture(depthTexture, texture_out + vec2(0, -1/float(depthSize))).x;
    float v5 = -texture(depthTexture, texture_out + vec2(-1/float(depthSize), 0)).x;

    float v6 = -texture(depthTexture, texture_out + vec2(1/float(depthSize), 1/float(depthSize))).x;
    float v7 = -texture(depthTexture, texture_out + vec2(-1/float(depthSize), 1/float(depthSize))).x;
    float v8 = -texture(depthTexture, texture_out + vec2(-1/float(depthSize), -1/float(depthSize))).x;
    float v9 = -texture(depthTexture, texture_out + vec2(1/float(depthSize), -1/float(depthSize))).x;

    vec3 one = (vec3(0, v1, 0));
    vec3 two = (vec3(size, v2, 0));
    vec3 three = (vec3(0, v3, -size));
    vec3 four = (vec3(0, v4, size));
    vec3 five = (vec3(-size, v5, 0));

    vec3 six = (vec3(size, v6, -size));
    vec3 seven = (vec3(-size, v7, -size));
    vec3 eight = (vec3(-size, v8, size));
    vec3 nine = (vec3(size, v9, size));

    vec3 tangent1 = normalize(two - one);
    vec3 tangent2 = normalize(three - one);

    vec3 tangent3 = normalize(four - one);
    vec3 tangent4 = normalize(five - one);

    vec3 tangent5 = normalize(six - one);
    vec3 tangent6 = normalize(seven - one);

    vec3 tangent7 = normalize(eight - one);
    vec3 tangent8 = normalize(nine - one);
    
    vec3 normalTemp1 = normalize(cross(tangent1, tangent2));
    vec3 normalTemp2 = normalize(cross(tangent4, tangent3));
    vec3 normalTemp3 = normalize(cross(tangent5, tangent6));
    vec3 normalTemp4 = normalize(cross(tangent7, tangent8));

    vec3 avg = normalize(normalTemp1 + normalTemp2 +normalTemp3+normalTemp4);
   
    normal = vec4(avg.xyz, 1);
}