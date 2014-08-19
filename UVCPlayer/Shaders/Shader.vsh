attribute vec2 position;

uniform float Yw;
uniform float Cbw;
uniform float Crw;

varying vec2 texCoordsY;

void main()
{
    gl_Position = vec4(position, 0.0, 1.0);
    texCoordsY = vec2(position.x * Yw, position.y);
}
