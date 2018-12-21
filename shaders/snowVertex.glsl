
#version 430

layout (location = 0) in vec4 pos;
layout (location = 1) in vec3 texture_coord;

uniform mat4 modelToWorld;
uniform mat4 projection;
uniform mat4 worldToView;

uniform mat4 toNormalMapSpace;
uniform sampler2D normalMap;

// Shader Storage Buffer Object that is being written to frmo simulateParticlesFragment.glsl
layout (std430, binding=2) buffer collision_data
{
    float numCollisions[];
};

out float snow_accumulated_out;
out vec3 normal_out;
out vec3 frag_out;
out vec2 texture_out;
out vec3 camera_pos_out;

int depthSize = 2048;

void main() {
    texture_out = texture_coord.xy;
    vec4 worldCoord = modelToWorld * vec4(pos.xyz, 1);

    // Find coordinate in normal map space
    vec4 normalMapCoord = toNormalMapSpace * vec4(worldCoord.xyz, 1);
    vec3 projCoord = normalMapCoord.xyz / normalMapCoord.w;
    projCoord = projCoord * 0.5 + 0.5;
    
    // Retrieve normal from normal map.
    normal_out = texture(normalMap, projCoord.xy + vec2(1/float(depthSize), 1/float(depthSize))).xyz;

    // Find how much snow has accumulated on this vertex, from the SSBO
    int mappedX = int(projCoord.x * depthSize); 
    int mappedY = int(projCoord.y * depthSize); 
    float snowAccumulated = float(numCollisions[mappedY * depthSize + mappedX]);

    // The forth component of the position vector indicates if the vertex is an edge to and object.
    // In that case we don't want to displace its vertex.
    bool isEdge = pos.w < 0.5;
    if (!isEdge) {
        worldCoord.y += snowAccumulated / 600.0f; // Displace vertex based on how much snow.
        snow_accumulated_out = snowAccumulated;
    }  else {
        snow_accumulated_out = snowAccumulated;
    }

    frag_out = worldCoord.xyz;
    gl_Position = projection * worldToView * worldCoord;

    // Extract camera position
    mat4 temp = inverse(worldToView);
    camera_pos_out = vec3(temp[3][0], temp[3][1], temp[3][2]);
}

