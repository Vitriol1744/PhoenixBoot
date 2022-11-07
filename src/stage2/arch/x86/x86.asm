use32
%include 'x86_cpu_modes.inc'

%macro LinearToSegOffset 4
    mov %3, %1

    shr %3, 4
    mov %2, %4
    mov %3, %1

    and %3, 0x0f
%endmacro

global get_drive_parameters
get_drive_parameters:
    enter 0, 0
    GoRealMode

    ; save non-scratch registers
    push esi
    push bx
    push es
    push di

    mov dl, [bp + 8]
    mov ah, 0x08
    mov di, 0
    mov es, di
    stc
    int 0x13

    ; return value
    mov eax, 1
    sbb eax, 0

    LinearToSegOffset [bp + 12], es, esi, si
    mov [es:si], bl

    mov bl, ch
    mov bh, cl
    shr bh, 6
    inc bx

    LinearToSegOffset [bp + 16], es, esi, si
    mov [es:si], bx

    xor ch, ch
    and cl, 0x3f

    LinearToSegOffset [bp + 20], es, esi, si
    mov [es:si], cx

    ; load non-scratch registers
    pop di
    pop es
    pop bx
    pop esi

    ; return value
    push eax
    GoProtectedMode
    pop eax

    leave
    ret