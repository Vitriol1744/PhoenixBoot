section .text
extern raiseException

%macro exception 1
align 16
exception%1:
    cld
    enter 0, 0
    push %1
    call raiseException
    leave
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