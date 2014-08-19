attribute vec2 position;

uniform float Yw;
uniform float Cbw;
uniform float Crw;

varying vec4 texCoords;

void main()
{
    gl_Position = vec4(position, 0.0, 1.0);
    texCoords = vec4(position.x * Yw, position.x * Cbw, position.x * Crw, position.y);
}
