
#version 430

layout (location = 0) in vec4 pos;
layout (location = 1) in vec3 texture;

uniform mat4 modelToWorld; // Is modelToClip though?
uniform mat4 projection;
uniform mat4 worldToView;

uniform mat4 toLightSpace;

uniform sampler2D snowOffsetTexture;

layout (std430, binding=2) buffer collision_data
{
    float numCollisions[];
};

out float num_collisions_out;

out vec2 texture_out;

void main() {
    texture_out = texture.xy;


    vec4 worldCoord = modelToWorld * vec4(pos.xyz, 1);

    vec4 lightCoord = toLightSpace * vec4(worldCoord.xyz, 1);
    vec3 projCoord = lightCoord.xyz / lightCoord.w;
    projCoord = projCoord * 0.5 + 0.5;
    vec4 offset = texture(snowOffsetTexture, projCoord.xy);


    int mappedX = int(projCoord.x * 1024); 
    int mappedY = int(projCoord.y * 1024); 
    
    if (pos.w >= 0.5) {
        worldCoord.y += numCollisions[mappedY * 1024 + mappedX] / 1000.0f;
        num_collisions_out = float(numCollisions[mappedY * 1024 + mappedX]);
    } 

    gl_Position = projection * worldToView * worldCoord;
}

