section .text
extern raiseException

%macro exception 1
align 16
exception%1:
    cld
%if %1 == 8 || %1 == 10 || %1 == 11 || %1 == 12 || %1 == 13 || %1 == 14 || %1 == 17 || %1 == 30
    pop eax
%else
    xor eax, eax
%endif
    push ebp
    mov ebp, esp
    push eax
    push %1
    call raiseException
    iret
%endmacro

%assign i 0
%rep 32
exception i
%assign i i+1
%endrep

section .rodata
%macro get_exception_addr 1
dd exception%1
%endmacro

global exceptions
exceptions:
%assign i 0
%rep 32
get_exception_addr i
%assign i i+1
%endrep