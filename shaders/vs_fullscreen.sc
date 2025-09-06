$output  v_texcoord0

#include <bgfx_shader.sh>

void main()
{
    // 3頂点のIDは内蔵セマンティクスの gl_VertexID 相当を使う
    // 0:(-1,-3), 1:(3,1), 2:(-1,1)
    int id = int(gl_VertexID);
    vec2 pos = vec2( (id == 1) ?  3.0 : -1.0,
                     (id == 0) ? -3.0 :  1.0 );

    gl_Position = vec4(pos, 0.0, 1.0);
    v_texcoord0 = pos * 0.5 + 0.5; // [-1,1] -> [0,1]
}

