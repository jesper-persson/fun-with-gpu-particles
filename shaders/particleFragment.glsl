#version 400

layout(location = 0) out vec4 newPositionTexture;
layout(location = 1) out vec4 newVelocityTexture;

// The last component holds time to die in seconds.
uniform sampler2D positionTexture;

// The last component is not used.
uniform sampler2D velocityTexture;

// These two could be replaced by introducing randomness on GPU.
uniform sampler2D initialPositionTexture;
uniform sampler2D initialVelocityTexture; 

uniform float dt; 
uniform sampler2D texDepth;
uniform mat4 toLightSpace;

in vec2 texture_out;

void main() {
    vec4 velocity = texture(velocityTexture, vec2(texture_out));
    velocity.w = 0;

    vec4 oldPosition = texture(positionTexture, vec2(texture_out));
    float timeLeft = oldPosition.w - dt;

    vec4 newPosition = oldPosition + velocity * dt;
    newPositionTexture = newPosition;

    vec4 netForce = vec4(0, -20.0, 0, 0);
    newVelocityTexture = velocity + netForce * dt;


    // Depth texture collision detection and response
    vec4 lightCoord = toLightSpace * vec4(newPosition.xyz, 1);
    vec3 projCoord = lightCoord.xyz / lightCoord.w;
    projCoord = projCoord * 0.5 + 0.5;
    vec4 texDepthV = texture(texDepth, projCoord.xy);
    if (projCoord.z >= texDepthV.r) { // 0.74
        //newPosition = oldPosition;
        //newPositionTexture = newPosition;

        vec4 hej = vec4(projCoord.xyz, 1);
        hej.z = texDepthV.r;
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
    }

    // Limit max speed
    if (length(newVelocityTexture) > 0.5) {
        newVelocityTexture = normalize(velocity) * 0.5;
    }

    // Respawn
    newPositionTexture.w = timeLeft;
    if (timeLeft < 0) {
        newPositionTexture = texture(initialPositionTexture, vec2(texture_out));
        newVelocityTexture = texture(initialVelocityTexture, vec2(texture_out));
    }
}
