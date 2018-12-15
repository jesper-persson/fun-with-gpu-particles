
#version 430

layout (location = 0) in vec4 pos;
layout (location = 1) in vec3 texture1;
layout (location = 2) in vec3 normal;

uniform mat4 modelToWorld; // Is modelToClip though?
uniform mat4 projection;
uniform mat4 worldToView;

uniform mat4 toLightSpace;
uniform sampler2D normalMap;
uniform sampler2D snowOffsetTexture;

layout (std430, binding=2) buffer collision_data
{
    float numCollisions[];
};

out float num_collisions_out;
out float isEdge;

out vec3 normal_out;

out vec3 frag_out;
out vec2 texture_out;

out vec3 camera_pos_out;

int depthSize = 2048;

void main() {
    texture_out = texture1.xy;


    vec4 worldCoord = modelToWorld * vec4(pos.xyz, 1);

    vec4 lightCoord = toLightSpace * vec4(worldCoord.xyz, 1);
    vec3 projCoord = lightCoord.xyz / lightCoord.w;
    projCoord = projCoord * 0.5 + 0.5;
    vec4 offset = texture(snowOffsetTexture, projCoord.xy);
    
    
    mat4 temp = inverse(worldToView);
    camera_pos_out = vec3(temp[3][0], temp[3][1], temp[3][2]);

    normal_out = texture(normalMap, projCoord.xy + vec2(1/float(depthSize), 1/float(depthSize))).xyz;
    // normal_out = normal;
    // normal_out = mat3(transpose(inverse(modelToWorld))) * normal; 

    int mappedX = int(projCoord.x * depthSize); 
    int mappedY = int(projCoord.y * depthSize); 

    float val = float(numCollisions[mappedY * depthSize + mappedX]) / float(1);
    
    if (pos.w >= 0.5) {
        worldCoord.y += val / 400.0f;
        num_collisions_out = val;
    }  else {
        num_collisions_out = val;
    }

    isEdge = pos.w;

    frag_out = worldCoord.xyz;

    gl_Position = projection * worldToView * worldCoord;
}

