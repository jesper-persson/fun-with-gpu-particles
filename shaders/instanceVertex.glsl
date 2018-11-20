#version 400

layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 texture;

uniform mat4 translation;
uniform mat4 scale;
uniform mat4 view;
uniform mat4 perspective;
uniform sampler2D tex;

out vec2 texture_out;

void main() {
    texture_out = texture.xy;

    int texWidth = 100;
    int texHeight = 100;
    int index = gl_InstanceID * 1;
    float x = (index % texWidth) / 100.0f;
    float y = (index / texHeight) / 100.0f;

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