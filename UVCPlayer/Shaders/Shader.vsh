attribute vec2 position;

uniform float Yw;

void main()
{
    gl_Position = vec4(position.x, position.y, 0.0, 1.0);
}
