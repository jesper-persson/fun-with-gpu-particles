
#version 400

layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 texture;

uniform mat4 modelToClip;
out vec2 texture_out;

void main() {
    texture_out = texture.xy;
    gl_Position = modelToClip * vec4(pos, 1);
}