#version 400

layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 texture1;

uniform mat4 translation;
uniform mat4 scale;
uniform mat4 view;
uniform mat4 perspective;
uniform int textureSize;
uniform sampler2D tex;

out vec2 texture_out;
out vec3 normal_out;
out vec3 frag_out;

void main() {
    texture_out = texture1.xy;

    int texWidth = textureSize;
    int texHeight = textureSize;
    int index = gl_InstanceID * 1;
    float x = (index % texWidth) / float(textureSize);
    float y = (index / texHeight) / float(textureSize);

    vec3 pixelValue = texture(tex, vec2(x,y)).xyz;

    mat4 translationNew = translation;
    translationNew[3][0] = pixelValue.x;
    translationNew[3][1] = pixelValue.y;
    translationNew[3][2] = pixelValue.z;

    mat4 faceCamera = view;
    faceCamera[3][0] = 0;
    faceCamera[3][1] = 0;
    faceCamera[3][2] = 0;

    mat4 modelToView = view * translationNew  * inverse(faceCamera) * scale;


    gl_Position = perspective * modelToView * vec4(pos, 1);
}