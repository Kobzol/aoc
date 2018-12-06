bits 64
section .data

section .text
; parameters - rdi, rsi, rdx, rcx, r8, r9
; must be restored - rbx, rbp, rsp, r12, r13, r14, r15

; rdi - array a
; rsi - array b
; rdx - array count

global loop_basic
loop_basic:
    mov rax, [rsi]
    mov [rdi], rax
    add rdi, 8
    add rsi, 8
    dec rdx
    jnz loop_basic

    mov rax, 1
    ret

global loop_unrolled
loop_unrolled:
    xor rcx, rcx
    xor r8, r8
    xor r9, r9
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
    push rbx
    ;mov ebx, 111          ; Start marker bytes
    ;db 0x64, 0x67, 0x90   ; Start marker bytes
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

    ;mov ebx, 222          ; End marker bytes
    ;db 0x64, 0x67, 0x90   ; End marker bytes

    mov rax, 1
    pop rbx
    ret
