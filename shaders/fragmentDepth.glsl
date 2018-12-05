#version 400

layout(location = 0) out float depth;

uniform sampler2D tex;

void main(){
    depth = gl_FragCoord.z;
}