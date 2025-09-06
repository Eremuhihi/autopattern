$input  v_texcoord0
#include <bgfx_shader.sh>


#if BGFX_SHADER_LANGUAGE_HLSL
    SAMPLER2D(u_tex0, 0);
#else
    SAMPLER2D(u_tex0);
#endif

void main()
{
    vec4 c = texture2D(u_tex0, v_texcoord0);
    gl_FragColor = c;
}

