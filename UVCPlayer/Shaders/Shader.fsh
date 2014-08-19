precision mediump float;

uniform sampler2D Y;
uniform sampler2D Cb;
uniform sampler2D Cr;

varying vec2 texCoordsY;
varying vec2 texCoordsCb;
varying vec2 texCoordsCr;

void main()
{
    float y = texture2D(Y, texCoordsY).a;
    float u = texture2D(Cb, texCoordsCb).a;
    float v = texture2D(Cr, texCoordsCr).a;
    gl_FragColor = vec4(y + 1.403 * v, y - 0.344 * u - 0.714 * v, y + 1.770 * u, 1.0);
}
