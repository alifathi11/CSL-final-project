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