#version 430

layout(location = 0) out vec4 newPositionTexture;
layout(location = 1) out vec4 newVelocityTexture;

// The last component of the pixels holds time to die in seconds.
uniform sampler2D positionTexture;

// The last component of pixels is used to store if a particle has collided or not, to avoid repeating collisions.
uniform sampler2D velocityTexture;

// Used for respawning particles.
uniform sampler2D initialPositionTexture;
uniform sampler2D initialVelocityTexture; 

// Previously used to simulated wind. Not used any longer.
uniform sampler3D forceFieldTexture;

// Used to know how much snow can be accumulated at a certain surface position. (Steep slopes accumulate less snow.)
uniform sampler2D normalTexture;

uniform float dt; 
uniform sampler2D texDepth;
uniform mat4 toDepthTextureSpace;

// Size of depth texture.
int depthSize = 2048;

in vec2 texture_out;

// Shader Storage Buffer Objects to communicate between this shader and snowVertex.glsl
layout (std430, binding=2) buffer collision_data
{
    float numCollisions[];
};

// Map world coordinate of particles to a 3d vector field texel
float fieldLower = -3;
float fieldUpper = 3;
float fieldSize = fieldUpper - fieldLower;
mat4 toForceTextureCoordinates =  (mat4(vec4(1/fieldSize, 0, 0, 0), vec4(0, 1/fieldSize, 0, 0), vec4(0, 0, -1/fieldSize, 0), vec4(-fieldLower/fieldSize, -fieldLower/fieldSize, fieldLower/fieldSize, 0)));

// Finds wind force given a particle position. Note that this was left out in the simulation, due to poor behaviour.
vec4 vectorFieldForce(vec4 position) {
    vec3 vectorFieldTextureCoord = vec3( (toForceTextureCoordinates * vec4(position.xyz, 1)).xyz );
    vec4 force = texture(forceFieldTexture, vectorFieldTextureCoord);
    force.w = 0;
    return force;
}

// Pseudorandomness to avoid particles landing on the exact same locations all the time
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

    // Calculate forces
    vec4 windForce = vectorFieldForce(newPosition) * 0; // Don't include wind force
    vec4 airResistance = vec4(0, 0, 0, 0);
    if (length(velocity) > 0.001) {
        float airResitanceCoef = 0.5f;
        airResistance = normalize(velocity) * -1 * airResitanceCoef * length(velocity) * length(velocity);
    }
    vec4 gravity = vec4(0, -1, 0, 0);
    vec4 netForce = windForce + airResistance + gravity;
    if (hasCollided > 0.1) {
        netForce = vec4(0, 0, 0, 0);
    }

    newVelocityTexture = velocity + netForce * dt;
    newVelocityTexture.w = hasCollided;

    // Depth texture collision detection and response
    vec4 depthTextureCoord = toDepthTextureSpace * vec4(newPosition.xyz, 1);
    vec3 projCoord = depthTextureCoord.xyz / depthTextureCoord.w;
    projCoord = projCoord * 0.5 + 0.5;
    vec4 texDepthV = texture(texDepth, projCoord.xy);
    float bias = 0.001;
    // Collision test
    if (projCoord.z + bias > texDepthV.r && hasCollided < 0.9 && projCoord.z - 0.005 < texDepthV.r ) {
        newPosition = oldPosition;
        newPositionTexture = newPosition;
        newPositionTexture.w = newPosition.w;
        
        // Collision resolution (makes particles follow direction of the surface they collide with).
        // This is not used anylonger as I want particles to stop when they collide.
        // Uses symmetric derivative.
        float dh = 1.0/float(depthSize);
        float dx = (texture(texDepth, vec2(projCoord.x + dh, projCoord.y)).r - texture(texDepth, vec2(projCoord.x - dh, projCoord.y)).r)/(2*dh);
        float dy = (texture(texDepth, vec2(projCoord.x, projCoord.y + dh)).r - texture(texDepth, vec2(projCoord.x, projCoord.y - dh)).r)/(2*dh);
        float dz = (texture(texDepth, projCoord.xy + vec2(dx, dy) * dh).r - texDepthV.r) / dh;
        vec4 textureSpaceVelocity = normalize(vec4(dx, dy, dz, 0));
        textureSpaceVelocity = inverse(toDepthTextureSpace) * textureSpaceVelocity;
        if (length(textureSpaceVelocity) > 0.01) {
            newVelocityTexture.xyz = length(velocity) * normalize(textureSpaceVelocity.xyz); 
        }

        // Set velocity to 0, and the forth component to 1 to indicate that the particle has collided.
        newVelocityTexture = vec4(0,0,0,1);

        // Update SSBO to indicate snow should accumulate where the particle collided.
        vec3 normal = normalize(texture(normalTexture, projCoord.xy).xyz);
        float normalFactor = max(0, dot(-normal, vec3(normalize(velocity.xyz))));
        float collideValue = 1 * normalFactor * normalFactor * normalFactor;
        collideValue = min(1, collideValue);
        collideValue = max(0, collideValue);

        int mappedX = int(projCoord.x * depthSize); 
        int mappedY = int(projCoord.y * depthSize); 

        // Decide which snow buffer index to update. If the current patch has gotten more snow that its neighbours, we
        // add snow to the neightbour with less snow instead. This helps get a smooth looking snow cover.
        float curVal = numCollisions[mappedY * depthSize + mappedX];
        bool increase = true;
        if (curVal > numCollisions[mappedY * depthSize + mappedX - 1]) {
            numCollisions[mappedY * depthSize + mappedX - 1] += collideValue;
            increase = false;
        }
        if (curVal > numCollisions[mappedY * depthSize + mappedX + 1] && increase) {
            numCollisions[mappedY * depthSize + mappedX + 1] += collideValue;
            increase = false;
        }
        if (curVal > numCollisions[mappedY * depthSize + depthSize + mappedX] && increase) {
            numCollisions[mappedY * depthSize + depthSize + mappedX] += collideValue;
            increase = false;
        }
        if (curVal > numCollisions[mappedY * depthSize - depthSize + mappedX] && increase) {
            numCollisions[mappedY * depthSize - depthSize + mappedX] += collideValue;
            increase = false;
        }
        if (increase) {
            numCollisions[mappedY * depthSize + mappedX] += collideValue;
        }

        if (timeLeft > 1) {
            timeLeft = 1;
        }
    }

    // Respawn dead particles. Uses a combination of initial position and velocities texture, but with some randomness added.
    newPositionTexture.w = timeLeft;
    if (timeLeft < 0) {
        newPositionTexture = texture(initialPositionTexture, vec2(texture_out));
        newPositionTexture.x = (rand(vec2(newPosition.x, velocity.z)) - 0.5) * 20;
        newPositionTexture.z = (rand(vec2(velocity.x, newPosition.z)) - 0.5) * 20;

        newVelocityTexture = texture(initialVelocityTexture, vec2(texture_out));
        newVelocityTexture.x = (rand(vec2(velocity.x, newPosition.z)) - 0.5) * 2 * 6;
        newVelocityTexture.z = (rand(vec2(newPosition.x, newPosition.z)) - 0.5) * 2  * 6;
        newVelocityTexture.y = newVelocityTexture.y;
        newVelocityTexture.w = 0;
    }
}
