#version 400

layout(location = 0) out vec4 ocolor;
layout(location = 1) out vec4 ovelocity;

uniform sampler2D tex;
uniform sampler2D texVelocity;
uniform sampler2D initVelocity;

in vec2 texture_out;

void main() {
    // vec4 velocity = vec4(-0.001, 0, 0, 0);

    // if (texture_out.y > 0.5) {
    //     velocity.x = 0.001;
    // }

    // vec4 velocity = vec4(texture_out.xy, 0, 0) * 0.01;
    vec4 velocity = texture(texVelocity, vec2(texture_out));
    
    vec4 velocitySmall = velocity * 0.0005;
    vec4 oldPosition = texture(tex, vec2(texture_out));
    vec4 newPosition = oldPosition + velocitySmall;

    float dt = 0.001f;
    newPosition.w = newPosition.w - dt;

    ocolor = newPosition;
    ovelocity = velocity  + vec4(0, -0.003, 0, 0); 

    float timeLeft = oldPosition.w;
    if (timeLeft < 0) {
        ocolor = vec4(0, 0, 0, 1.5);
        ovelocity = texture(initVelocity, vec2(texture_out));
    }
    //ocolor = vec4(texture_out.xy, 0, 1);
}