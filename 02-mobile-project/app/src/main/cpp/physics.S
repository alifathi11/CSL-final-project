.global update_positions_asm
.type update_positions_asm, %function

update_positions_asm:
    // x0 = posX
    // x1 = posY
    // x2 = velX
    // x3 = velY
    // w4 = count
    // s0 = dt

    uxtw x4, w4          
    mov x9, #0           

    dup v0.4s, v0.s[0]

update_positions_simd_loop:
    add x10, x9, #4
    cmp x10, x4
    bgt update_positions_scalar_tail

    lsl x11, x9, #2

    // Compute addresses
    add x12, x0, x11
    add x13, x1, x11
    add x14, x2, x11
    add x15, x3, x11

    // Load
    ld1 {v1.4s}, [x12]
    ld1 {v2.4s}, [x13]
    ld1 {v3.4s}, [x14]
    ld1 {v4.4s}, [x15]

    // pos += vel * dt
    fmul v3.4s, v3.4s, v0.4s
    fmul v4.4s, v4.4s, v0.4s
    fadd v1.4s, v1.4s, v3.4s
    fadd v2.4s, v2.4s, v4.4s

    // Store
    st1 {v1.4s}, [x12]
    st1 {v2.4s}, [x13]

    add x9, x9, #4
    b update_positions_simd_loop

update_positions_scalar_tail:
    cmp x9, x4
    bge update_positions_done

update_positions_scalar_loop:
    ldr s1, [x0, x9, LSL #2]
    ldr s2, [x1, x9, LSL #2]
    ldr s3, [x2, x9, LSL #2]
    ldr s4, [x3, x9, LSL #2]

    fmul s3, s3, s0
    fmul s4, s4, s0
    fadd s1, s1, s3
    fadd s2, s2, s4

    str s1, [x0, x9, LSL #2]
    str s2, [x1, x9, LSL #2]

    add x9, x9, #1
    cmp x9, x4
    blt update_positions_scalar_loop

update_positions_done:
    ret


// TODO: use Vector Registers
.global handle_wall_collisions_asm
.type handle_wall_collisions_asm, %function

handle_wall_collisions_asm:
    // x0 = posX
    // x1 = posY
    // x2 = velX
    // x3 = velY
    // w4 = count
    // s0 = radius
    // s1 = w
    // s2 = h
    // w5 = gameOver

    cmp w5, #0
    b.ne wall_done

    uxtw x4, w4        
    mov x5, #0         

    fsub s3, s1, s0    // w - radius
    fsub s4, s2, s0    // h - radius

wall_loop:
    cmp x5, x4
    bge wall_done

    ldr s5, [x0, x5, LSL #2]   // posX[i]
    ldr s6, [x1, x5, LSL #2]   // posY[i]

    ldr s7, [x2, x5, LSL #2]   // velX[i]
    ldr s8, [x3, x5, LSL #2]   // velY[i]

    fcmp s5, s0
    bge check_right

    fmov s5, s0
    fneg s7, s7

check_right:
    fcmp s5, s3
    ble check_top

    fmov s5, s3
    fneg s7, s7

check_top:
    fcmp s6, s0
    bge check_bottom

    fmov s6, s0
    fneg s8, s8

check_bottom:
    fcmp s6, s4
    ble store_values

    fmov s6, s4
    fneg s8, s8

store_values:
    str s5, [x0, x5, LSL #2]
    str s6, [x1, x5, LSL #2]
    str s7, [x2, x5, LSL #2]
    str s8, [x3, x5, LSL #2]

    add x5, x5, #1
    b wall_loop

wall_done:
    ret
