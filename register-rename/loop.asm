bits 64
section .data

section .text
; parameters - rdi, rsi, rdx, rcx, r8, r9
; must be restored - rbx, rbp, rsp, r12, r13, r14, r15

; rdi - array a
; rsi - array b
; rdx - array count

; for (int i = 0; i < size; i++)
;    a[i] = b[i];

global loop_basic
loop_basic:
    mov rax, [rsi]      ; x = *b
    mov [rdi], rax      ; *a = x
    add rdi, 8          ; a++
    add rsi, 8          ; b++
    dec rdx             ; size--
    jnz loop_basic

    mov rax, 1
    ret

global loop_unrolled
loop_unrolled:
.loop:
    mov rax, [rsi]
    mov [rdi], rax

    mov rcx, [rsi + 1 * 8]
    mov [rdi + 1 * 8], rcx

    mov r8, [rsi + 2 * 8]
    mov [rdi + 2 * 8], r8

    mov r9, [rsi + 3 * 8]
    mov [rdi + 3 * 8], r9

    add rdi, 32
    add rsi, 32
    sub rdx, 4
    jnz .loop

    mov rax, 1
    ret

global loop_unrolled_rax
loop_unrolled_rax:
.loop:
    mov rax, [rsi]
    mov [rdi], rax

    mov rax, [rsi + 1 * 8]
    mov [rdi + 1 * 8], rax

    mov rax, [rsi + 2 * 8]
    mov [rdi + 2 * 8], rax

    mov rax, [rsi + 3 * 8]
    mov [rdi + 3 * 8], rax

    add rdi, 32
    add rsi, 32
    sub rdx, 4
    jnz .loop

    mov rax, 1
    ret
