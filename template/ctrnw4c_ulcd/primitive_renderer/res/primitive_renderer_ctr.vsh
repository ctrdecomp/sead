; primitive_renderer_ctr.vsh

main:
    mov r14, c83.xxxz
    mov r14.xyz, Vertex.xyzz
    dp4 r15.x, user[0], r14
    dp4 r15.y, user[1], r14
    dp4 r15.z, user[2], r14
    dp4 r15.w, user[3], r14
    dp4 r14.x, wvp[0], r15
    dp4 r14.y, wvp[1], r15
    dp4 r14.z, wvp[2], r15
    dp4 r14.w, wvp[3], r15
    mov result.position, r14
    mul r13, color1, ColorRate.xxxx
    add r12, c83.zzzz, -ColorRate.xxxx
    mad result.color, r12.xxxx, color0, r13
    mul r11.xy, uv_size.xyyy, TexCoord0.xyyy
    add r11.xy, uv_src.xyyy, r11.xyyy
    mov result.texcoord0w.w, c83.zzzz
    mov result.texcoord0w, r11.xyyy
    end
    nop