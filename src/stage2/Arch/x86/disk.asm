use32
%include 'x86_cpu_modes.inc'

global reset_disk
reset_disk:
    enter 0, 0
    GoRealMode

    ; disk reset routine
    mov ah, 0x00
    ; drive number
    mov dl, [bp + 8]
    stc
    int 0x13

    ; return 0 on failure
    mov eax, 0
    jc .done
    or eax, 1
.done:
    ; save return value
    push eax
    GoProtectedMode
    ; load return value
    pop eax

    leave
    ret

global get_drive_parameters
get_drive_parameters:
    enter 0, 0
    GoRealMode

    ; save non-scratch registers
    push bx
    push es

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

    mov cl, dh
    inc cx

    LinearToSegOffset [bp + 24], es, esi, si
    mov [es:si], cx

    ; load non-scratch registers
    pop es
    pop bx

    ; save return value
    push eax
    GoProtectedMode
    ; load return value
    pop eax

    leave
    ret

global read_sectors
read_sectors:
    enter 0, 0
    GoRealMode

    ; save non-scratch registers
    push ebx
    push es

    ; read sectors routine
    mov ah, 0x02
    ; drive number
    mov dl, [bp + 8]
    ; low 8 bits of cylinder number
    mov ch, [bp + 12]
    ; high 2 bits of cylinder number (bits 6-7)
    mov cl, [bp + 13]
    shl cl, 6
    ; sector number (bits 0-5)
    mov al, [bp + 16]
    and al, 0x3f
    or cl, al
    ; head number
    mov dh, [bp + 20]
    ; sectors to read count
    mov al, [bp + 28]
    ; buffer
    LinearToSegOffset [bp + 24], es, ebx, bx
    stc
    int 0x13

    push eax
    mov eax, 0
    jc .done
    ; if successful, set return value to the number of sectors transferred
    pop eax
.done:
    ; load non-scratch registers
    pop es
    pop ebx

    ; save return value
    push eax
    GoProtectedMode
    ; load return value
    pop eax

    leave
    ret