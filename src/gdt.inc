gdtr:
    dw .end - .start - 1
    dd .start

.start:
    ; null segment
    dq 0

    ; 32-bit code segment
    dw 0xffff                   ; limit (bits 0-15)
    dw 0                        ; base (bits 0-15)
    db 0                        ; base (bits 16-23)
    db 0b10011010               ; access (present(bit 7) | code_or_data_segment(bit 4) | executable(bit 3) | readable(bit 0))
    ; flags: 4k pages | 32bit segment
    db (0b1100 << 4) | 0b1111   ; flags | limit (bits 16-19)
    db 0                        ; base (bits 24-31)

    ; 32-bit data segment
    dw 0xffff                   ; limit (bits 0-15)
    dw 0                        ; base (bits 0-15) 
    db 0                        ; base (bits 16-23)
    db 0b10010010               ; access (present(bit 7) | code_or_data_segment(bit 4) | writeable(bit 1))
    ; 4k pages | 32bit segment
    db (0b1100 << 4) | 0b1111   ; flags | limit (bits 16-19)
    db 0                        ; base (bits 24-31)

    ; 16-bit code segment
    dw 0xffff                   ; limit (bits 0-15)
    dw 0                        ; base (bits 0-15)
    db 0                        ; base (bits 16-23)
    db 0b10011010               ; access (present(bit 7) | code_or_data_segment(bit 4) | executable(bit 3) | readable(bit 0))
    db (0b0000 << 4) | 0b1111   ; flags | limit
    db 0                        ; base (bits 24-31)

    ; 16-bit data segment
    dw 0xffff                   ; limit (bits 0-15)
    dw 0                        ; base (bits 0-15)
    db 0                        ; base (bits 16-23)
    db 0b10010010               ; access (present(bit 7) | code_or_data_segment(bit 4) | writeable(bit 1))
    db (0b0000 << 4) | 0b1111   ; flags | limit
    db 0                        ; base (bits 24-31)
.end:
