$input  v_texcoord0
#include <bgfx_shader.sh>

SAMPLER2D(u_tex0);

void main()
{
    vec4 c = texture2D(u_tex0, v_texcoord0);
    gl_FragColor = c;
}

