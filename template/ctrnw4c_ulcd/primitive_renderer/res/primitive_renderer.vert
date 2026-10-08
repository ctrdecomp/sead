uniform mat4 wvp;
uniform mat4 user;
uniform vec4 color0;
uniform vec4 color1;
uniform vec4 uv_src;
uniform vec4 uv_size;

attribute vec3 Vertex;
attribute vec2 TexCoord0;
attribute vec4 ColorRate;

void main(void)
{
    gl_Position = vec4(Vertex, 1.0) * user * wvp;
    gl_FrontColor = color0 * (1.0 - ColorRate.r) + color1 * ColorRate.r;
    gl_TexCoord[0] = vec4(TexCoord0 * uv_size.xy + uv_src.xy, 0.0, 1.0);
}