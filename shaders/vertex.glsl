
#version 400

layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 texture;

uniform mat4 modelToWorld; // Is modelToClip though?

out vec2 texture_out;

void main() {
    texture_out = texture.xy;
    gl_Position = modelToWorld * vec4(pos, 1);
}