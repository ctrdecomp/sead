; DMP_line.gsh

main:
    mov r0.x, r7.wwww
    ifu 23, 3, b15
    mov r0.y, c76.yyyy
    mov r0.y, c76.xxxx
    rcp r15.x, dmp_Line.width.xxxx
    rcp r15.y, dmp_Line.width.yyyy
    cmp eq, eq, c76.xxxx, r0
    mov r0.x, r8.zzzz
    nop
    ifc 31, 149, or x=1 y=1
    mov r7, c76.xxxx
    mov r0.y, gaPosition.zzzz
    cmp lt, lt, c76.xxxx, r0.xyyy
    slti r6.x, r7.wwww, c76.zzzz
    mov r1, gaPosition
    jmpc 179<finish_vertex_processing>, 0, and x=1 y=1
    jmpc 60<calc_directions>, 0, and x=0 y=0
    ifc 53, 7, and x=1 y=0
    add r0, r8.zzzz, -gaPosition.zzzz
    rcp r0, r0.xxxx
    mov r6.x, c76.yyyy
    mul r1.x, r0, r8.zzzz
    mul r1.y, -r0, gaPosition.zzzz
    mul r8, r1.yyyy, r8
    mul r9, r1.yyyy, r9
    mul r10, r1.yyyy, r10
    mul r11, r1.yyyy, r11
    mad r8, r1.xxxx, gaPosition, r8
    mad r9, r1.xxxx, v1, r9
    mad r10, r1.xxxx, v2, r10
    mad r11, r1.xxxx, v3, r11
    mov r8.z, c76.xxxx
    mov r1, gaPosition
    add r0, gaPosition.zzzz, -r8.zzzz
    rcp r0, r0.xxxx
    mul r0.x, r0, gaPosition.zzzz
    mul r0.y, -r0, r8.zzzz
    mul r1, r0.yyyy, gaPosition
    mad r1, r0.xxxx, r8, r1
    mov r1.z, c76.xxxx

calc_directions:
    mul r0.xy, r8, r1.wwww
    mov r3.z, r6.xxxx
    mov r2, c76.xxxx
    mad r0.xy, r1, r8.wwww, -r0
    mov r3.w, c76.xxxx
    mov r2.y, r1.wwww
    max r1.xy, r0, -r0
    mul r1.xy, r1, r15.yxxx
    cmp gt, eq, r1.xxxx, r1.yyyy
    mov r1, c76.xxxx
    ifc 78, 9, x x=1 y=1
    cmp lt, eq, c76.xxxx, r0.xxxx
    mul r2.y, r2.yyyy, r15.yyyy
    mul r1.y, r8.wwww, r15.yyyy
    ifc 76, 1, x x=1 y=1
    mov r7.y, c76.yyyy
    mov r7.y, -c76.yyyy
    max r3.xy, r7, -r7
    mul r2.y, r2.yyyy, r15.xxxx
    cmp lt, eq, c76.xxxx, r0.yyyy
    mul r1.x, -r8.wwww, r15.xxxx
    mov r2.x, -r2.yyyy
    ifc 84, 1, x x=1 y=1
    mov r7.y, c76.zzzz
    mov r7.y, -c76.zzzz
    max r3.xy, r7, -r7
    mov r2.y, c76.xxxx
    cmp eq, gt, r3.xzzz, r3.ywww
    ifc 102, 1, or x=0 y=1
    cmp ge, eq, c76.xxxx, gaPosition.zzzz
    mov output_9, r9
    mov output_9, r10
    mov output_9, r11
    setemit vtx=0 prim=0 winding=0
    add result.position, r8, r1
    emit
    setemit vtx=1 prim=0 winding=0
    emit
    add result.position, r8, -r1
    setemit vtx=1 prim=0 winding=0
    mov r7.z, c76.zzzz
    emit
    cmp ge, eq, c76.xxxx, gaPosition.zzzz
    ifc 109, 15, x x=1 y=1
    cmp lt, lt, c76.xxxx, r7.yyyy
    mov r3, gaPosition
    mov output_9, v1
    mov output_9, v2
    mov output_9, v3
    cmp lt, lt, c76.xxxx, r7.yyyy
    add r1, gaPosition.zzzz, -r8.zzzz
    rcp r1, r1.xxxx
    mul r3.x, r1, gaPosition.zzzz
    mul r3.y, -r1, r8.zzzz
    mul r1, r3.yyyy, gaPosition
    mul r4, r3.yyyy, v1
    mul r6, r3.yyyy, v2
    mad r1, r3.xxxx, r8, r1
    mad output_9, r3.xxxx, r9, r4
    mov r1.z, c76.xxxx
    mul r5, r3.yyyy, v3
    mad output_9, r3.xxxx, r10, r6
    mad output_9, r3.xxxx, r11, r5
    mov r3, r1
    ifc 152, 27, x x=1 y=1
    cmp eq, eq, c76.xyyy, r7.zzzz
    add result.position, r3, r2
    ifc 135, 16, x x=1 y=1
    setemit vtx=0 prim=1 winding=0
    emit
    setemit vtx=1 prim=0 winding=1
    emit
    add result.position, r3, -r2
    setemit vtx=1 prim=1 winding=1
    mov r7.z, c76.zzzz
    ifc 143, 7, y x=1 y=1
    setemit vtx=1 prim=1 winding=0
    emit
    setemit vtx=2 prim=0 winding=1
    emit
    add result.position, r3, -r2
    setemit vtx=2 prim=1 winding=1
    mov r7.z, c76.xxxx
    setemit vtx=2 prim=1 winding=0
    emit
    setemit vtx=0 prim=0 winding=1
    emit
    add result.position, r3, -r2
    setemit vtx=0 prim=1 winding=1
    mov r7.z, c76.yyyy
    nop
    emit
    cmp eq, eq, c76.xyyy, r7.zzzz
    add result.position, r3, r2
    ifc 162, 16, x x=1 y=1
    setemit vtx=0 prim=1 winding=1
    emit
    setemit vtx=1 prim=0 winding=0
    emit
    add result.position, r3, -r2
    setemit vtx=1 prim=1 winding=0
    mov r7.z, c76.zzzz
    ifc 170, 7, y x=1 y=1
    setemit vtx=1 prim=1 winding=1
    emit
    setemit vtx=2 prim=0 winding=0
    emit
    add result.position, r3, -r2
    setemit vtx=2 prim=1 winding=0
    mov r7.z, c76.xxxx
    setemit vtx=2 prim=1 winding=1
    emit
    setemit vtx=0 prim=0 winding=0
    emit
    add result.position, r3, -r2
    setemit vtx=0 prim=1 winding=0
    mov r7.z, c76.yyyy
    nop
    emit

finish_vertex_processing:
    mov r7.x, r7.yyyy
    add r7.w, c76.yyyy, r7.wwww
    mov r8, gaPosition
    mov r9, v1
    mov r10, v2
    mov r11, v3
    end
    nop