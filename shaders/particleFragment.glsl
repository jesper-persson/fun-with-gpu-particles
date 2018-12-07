#version 430

layout(location = 0) out vec4 newPositionTexture;
layout(location = 1) out vec4 newVelocityTexture;

// The last component holds time to die in seconds.
uniform sampler2D positionTexture;

// The last component is not used.
uniform sampler2D velocityTexture;

// These two could be replaced by introducing randomness on GPU.
uniform sampler2D initialPositionTexture;
uniform sampler2D initialVelocityTexture; 

uniform sampler3D forceFieldTexture;

uniform float dt; 
uniform sampler2D texDepth;
uniform mat4 toLightSpace;

in vec2 texture_out;


layout (std430, binding=2) buffer collision_data
{
    float numCollisions[];
};


// Map world coordinate of particles to a 3d vector field texel
float fieldLower = -3;
float fieldUpper = 3;
float fieldSize = fieldUpper - fieldLower;
// Each vec4 is a column.
mat4 toForceTextureCoordinates =  (mat4(vec4(1/fieldSize, 0, 0, 0), vec4(0, 1/fieldSize, 0, 0), vec4(0, 0, -1/fieldSize, 0), vec4(-fieldLower/fieldSize, -fieldLower/fieldSize, fieldLower/fieldSize, 0)));
vec4 vectorFieldForce(vec4 position) {
    vec3 vectorFieldTextureCoord = vec3( (toForceTextureCoordinates * vec4(position.xyz, 1)).xyz );
    vec4 force = texture(forceFieldTexture, vectorFieldTextureCoord);
    force.w = 0;
    return force;
}

float rand(vec2 co){
    return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453);
}

void main() {
    

    vec4 velocity = texture(velocityTexture, vec2(texture_out));
    float hasCollided = velocity.w;
    velocity.w = 0;


    vec4 oldPosition = texture(positionTexture, vec2(texture_out));
    float timeLeft = oldPosition.w - dt;

    vec4 newPosition = oldPosition + velocity * dt;
    newPositionTexture = newPosition;

    // vec4 netForce = vectorFieldForce(newPosition);

    vec4 airResistance = vec4(0, 0, 0, 0);
    if (length(velocity) > 0.001) {
        float airResitanceCoef = 0.0f;
        airResistance = normalize(velocity) * -1 * airResitanceCoef * length(velocity) * length(velocity);
    }

    vec4 gravity = vec4(0, -5, 0, 0);

    vec4 netForce = vec4(0, 0, 0, 0);
    newVelocityTexture = velocity + netForce * dt;
    newVelocityTexture.w = hasCollided;

    // Depth texture collision detection and response
    vec4 lightCoord = toLightSpace * vec4(newPosition.xyz, 1);
    vec3 projCoord = lightCoord.xyz / lightCoord.w;
    projCoord = projCoord * 0.5 + 0.5;
    vec4 texDepthV = texture(texDepth, projCoord.xy);
    float bias = 0.001;
    if (projCoord.z + bias > texDepthV.r && hasCollided < 0.9) { // 0.74
        newPosition = oldPosition;
        newPositionTexture = newPosition;

        vec4 hej = vec4(projCoord.xyz, 1);
        hej.z = projCoord.z;
        hej = (hej - 0.5) / 0.5;
        hej = vec4((hej * lightCoord.w).xyz, lightCoord.w);
        vec4 back = inverse(toLightSpace) * hej;

        newPositionTexture = back;
        newPositionTexture.w = newPosition.w;
        newVelocityTexture = velocity;

        float shadow_width = 1024;
        float dh = 1.0/shadow_width;

        float dx = (texture(texDepth, vec2(projCoord.x + dh, projCoord.y)).r - texture(texDepth, vec2(projCoord.x - dh, projCoord.y)).r)/(2*dh);
        float dy = (texture(texDepth, vec2(projCoord.x, projCoord.y + dh)).r - texture(texDepth, vec2(projCoord.x, projCoord.y - dh)).r)/(2*dh);
        float dz = (texture(texDepth, projCoord.xy + vec2(dx, dy) * dh).r - texDepthV.r) / dh;

        // dx, dy, dz vill jag ha i world space. Men ovan blir i light space
        vec4 lightSpaceVelocity = normalize(vec4(dx, dy, dz, 0));
        //lightSpaceVelocity = (lightSpaceVelocity - 0.5) / 0.5;
        //lightSpaceVelocity = vec4((lightSpaceVelocity * lightCoord.w).xyz, lightCoord.w);
        lightSpaceVelocity = inverse(toLightSpace) * lightSpaceVelocity;

        if (length(lightSpaceVelocity) > 0.01) {
            newVelocityTexture.xyz = length(velocity) * normalize(lightSpaceVelocity.xyz); 
        } else {

        }


        newVelocityTexture = vec4(0,0,0,1);


        int mappedX = int(projCoord.x * 1024); 
        int mappedY = int(projCoord.y * 1024); 
        numCollisions[mappedY * 1024 + mappedX] += + 0.5;

        // "Low pass filter"
        numCollisions[mappedY * 1024 + mappedX - 1] += 0.25;
        numCollisions[mappedY * 1024 + mappedX + 1] += 0.25;
        numCollisions[mappedY * 1024 + 1024 + mappedX] += 0.25;
        numCollisions[mappedY * 1024 - 1024 + mappedX] += 0.25;        

        numCollisions[mappedY * 1024 + mappedX - 1 - 1024] += 0.25;
        numCollisions[mappedY * 1024 + mappedX - 1 + 1024] += 0.25;
        numCollisions[mappedY * 1024 + mappedX + 1 - 1024] += 0.25;
        numCollisions[mappedY * 1024 + mappedX + 1 + 1024] += 0.25;
    }

    // Limit max speed
    // if (length(newVelocityTexture) > 2.5) {
    //     newVelocityTexture = normalize(velocity) * 2.5;
    // }

    // Respawn
    newPositionTexture.w = timeLeft;
    if (timeLeft < 0) {
        newPositionTexture = texture(initialPositionTexture, vec2(texture_out));
        newVelocityTexture = texture(initialVelocityTexture, vec2(texture_out));
        newVelocityTexture.x = (rand(vec2(velocity.x, newPosition.z)) - 0.5) * 0.5;
        newVelocityTexture.z = (rand(vec2(velocity.z, newPosition.x)) - 0.5) * 0.5;
        newVelocityTexture.w = 0;
    }
}
