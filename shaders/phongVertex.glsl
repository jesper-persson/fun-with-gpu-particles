#version 400

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 texture1;
layout (location = 2) in vec3 normal;

uniform mat4 modelToWorld;
uniform mat4 worldToView;
uniform mat4 projection;

out vec2 texture_out;
out vec3 normal_out;
out vec3 frag_out;

out vec3 camera_pos_out;

void main() {
    texture_out = texture1.xy;
    normal_out = normal;
    frag_out = vec3(modelToWorld * vec4(position, 1.0));
    normal_out = mat3(transpose(inverse(modelToWorld))) * normal; 

    mat4 temp = inverse(worldToView);
    camera_pos_out = vec3(temp[3][0], temp[3][1], temp[3][2]);

    gl_Position = projection * worldToView * modelToWorld * vec4(position, 1);
}