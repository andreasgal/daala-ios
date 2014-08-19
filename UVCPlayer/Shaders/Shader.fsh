precision mediump float;

uniform sampler2D Y;
uniform sampler2D Cb;
uniform sampler2D Cr;

varying vec2 texCoordsY;
varying vec2 texCoordsCb;
varying vec2 texCoordsCr;

const mat4 YUV2RGB = mat4(1.1643828125, 0, 1.59602734375, -.87078515625,
                          1.1643828125, -.39176171875, -.81296875, .52959375,
                          1.1643828125, 2.017234375, 0, -1.081390625,
                          0, 0, 0, 1);

void main()
{
    float y = texture2D(Y, texCoordsY).a;
    float u = texture2D(Cb, texCoordsCb).a;
    float v = texture2D(Cr, texCoordsCr).a;
    gl_FragColor = vec4(y, u, v, 1.0) * YUV2RGB;
}
