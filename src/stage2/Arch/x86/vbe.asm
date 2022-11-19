%include 'x86_cpu_modes.inc'

global vbe_get_controller_info
vbe_get_controller_info:
    enter 0, 0

    push es
    GoRealMode
    mov ax, 0x4f00
    LinearToSegOffset [bp + 8], es, edi, di
    xchg bx, bx
    int 0x10

    mov di, ax
    xor eax, eax
    cmp di, 0x004f
    jne .done
    or eax, 1
.done:
    push eax
    GoProtectedMode
    pop eax
    pop es

    leave
    ret

global vbe_get_mode_info
vbe_get_mode_info:
    enter 0, 0
    push es
    GoRealMode

    mov ax, 0x4f01
    mov cx, [bp + 8]
    LinearToSegOffset [bp + 12], es, edi, di
    int 0x10

    mov di, ax
    xor eax, eax
    cmp di, 0x004f
    jne .done
    or eax, 1
.done:
    push eax
    GoProtectedMode
    pop eax
    pop es

    leave
    ret

global vbe_set_video_mode
vbe_set_video_mode:
    enter 0, 0 
    GoRealMode

    mov ax, 0x4f02
    mov bx, [bp + 8]
    int 0x10

    mov di, ax
    xor eax, eax
    cmp ax, 0x004f
    jne .done
    or eax, 1
.done:
    push eax
    GoProtectedMode
    pop eax

    leave
    ret
