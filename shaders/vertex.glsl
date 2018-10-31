#version 400

in vec3 pos;

uniform mat4 modelToWorld;

void main() {
    gl_Position = modelToWorld * vec4(pos, 1);
}