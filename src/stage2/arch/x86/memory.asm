%include 'x86_cpu_modes.inc'

%macro LinearToSegOffset 4
    mov %3, %1

    shr %3, 4
    mov %2, %4
    mov %3, %1

    and %3, 0x0f
%endmacro

use32
global e820_get_next_entry
e820_get_next_entry:
    enter 0, 0
    GoRealMode
    push ebx
    push ecx
    push edx
    push esi
    push edi
    push ds
    push es
    
    mov eax, 0xe820
    mov edx, 0x534d4150
    LinearToSegOffset [bp + 12], ds, esi, si
    mov ebx, ds:[si]
    mov ecx, 24
    LinearToSegOffset [bp + 8], es, edi, di
    
    int 0x15
    jc .error
    cmp eax, 0x534d4150
    jne .error
.success:
    mov ds:[si], ebx
    mov eax, ecx
    jmp .exit
.error:
    mov eax, -1
.exit:
    pop es
    pop ds
    pop edi
    pop esi
    pop edx
    pop ecx
    pop ebx

    push  eax
    GoProtectedMode
    pop eax
    leave
    ret