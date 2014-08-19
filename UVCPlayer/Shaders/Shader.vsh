attribute vec2 position;

uniform float Yw;
uniform float Cbw;
uniform float Cbr;

void main()
{
    gl_Position = vec4(position.x * Yw, position.y, 0.0, 1.0);
}
