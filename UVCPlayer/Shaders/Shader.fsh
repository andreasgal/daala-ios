precision mediump float;

uniform sampler2D Y;
uniform sampler2D Cb;
uniform sampler2D Cr;

varying vec4 texCoords;

void main()
{
    float y = texture2D(Y, texCoords.xw).a;
    float u = texture2D(Cb, texCoords.yw).a;
    float v = texture2D(Cr, texCoords.zw).a;
    // Now assemble that into a YUV vector, and premultipy the Y.
    vec3 yuv = vec3(y * 1.1643828125, u, v);
    gl_FragColor = vec4(
        yuv.x + 1.59602734375 * yuv.z - 0.87078515625,
        yuv.x - 0.39176171875 * yuv.y - 0.81296875 * yuv.z + 0.52959375,
        yuv.x + 2.017234375   * yuv.y - 1.081390625,
        1.0);
}
