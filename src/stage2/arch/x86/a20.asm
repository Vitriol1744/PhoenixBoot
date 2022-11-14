%include 'x86_cpu_modes.inc'

use32
global a20_check
a20_check:
    enter 0, 0
    pushad

    mov edi, 0x112345
    mov esi, 0x012345
    mov [esi], esi
    mov [edi], edi
    cmpsd

    popad
    mov eax, 0
    je .done
    inc eax
    
.done:
    leave
    ret
 

global a20_enable
a20_enable:
    enter 0, 0
    ; if a20 is already enabled, exit
    call a20_check
    cmp eax, 1
    je .exit
.bios_a20_gate:
    GoRealMode
    xchg bx, bx
    ; query a20 gate support
    mov ax, 0x2403
    int 0x15
    jc .bios_a20_gate_finish
    cmp ah, 0x86
    je .bios_a20_gate_finish
    ; enable a20
    mov ah, 0x2401
    int 0x15
.bios_a20_gate_finish:
    GoProtectedMode
    call a20_check
    cmp eax, 1
    je .exit
.keyboard_a20:
    cli
 
    call    .a20wait1
    mov     al, 0xad
    out     0x64, al
 
    call    .a20wait1
    mov     al, 0xD0
    out     0x64, al
 
    call    .a20wait2
    in      al, 0x60
    push    eax
 
    call    .a20wait1
    mov     al, 0xd1
    out     0x64, al
 
    call    .a20wait1
    pop     eax
    or      al, 2
    out     0x60, al
 
    call    .a20wait1
    mov     al, 0xae
    out     0x64, al
 
    call    .a20wait1
    sti
    call a20_check
    cmp eax, 1
    je .exit
    jmp .fast_a20
 
.a20wait1:
    in      al, 0x64
    test    al, 2
    jnz     .a20wait1
    ret
.a20wait2:
    in      al, 0x64
    test    al, 1
    jz      .a20wait2
    ret
.fast_a20:
    in al, 0x92
    or al, 0b10
    out 0x92, al
    call a20_check
    je .exit
    in al, 0xee
.exit:
    ; we want to return a20 gate status
    call a20_check
    leave
    ret