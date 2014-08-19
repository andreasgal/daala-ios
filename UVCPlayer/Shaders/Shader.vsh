attribute vec2 position;

uniform float Yw;
uniform float Cbw;
uniform float Crw;

varying vec2 texCoordsY;

void main()
{
    gl_Position = vec4(position, 0.0, 1.0);
    vec2 texCoords = (position + vec2(1.0, 1.0)) / vec2(2.0, 2.0);
    texCoordsY = texCoords * vec2(Yw, 1.0);
}
