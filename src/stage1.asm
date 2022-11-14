use16
org 0x7c00

%define STAGE2_SEGMENT 0x0000
%define STAGE2_OFFSET 0x7e00

; skip bios parameter block
jmp short start
nop
bpb_oem_identifier: times 8 db 0
bpb_bytes_per_sector: dw 512
bpb_sectors_per_cluster: db 0
bpb_reserved_sectors: dw 0
bpb_fat_count: db 0
bpb_root_directory_entries: dw 0
bpb_total_sectors: dw 0
bpb_media_descriptor_type: db 0
bpb_sectors_per_fat: dw 0
bpb_sectors_per_track: dw 0
bpb_total_heads: dw 0
bpb_hidden_sector_count: dd 0
bpb_large_sector_count: dd 0
ebr_drive_number: db 0
ebr_reserved: db 0
ebr_signature: db 0
ebr_volume_id: dd 0
ebr_volume_label: db 'PhoenixBoot'
ebr_system_label: dq 0

start:
    cli
    cld
    ; set up segment registers
    xor ax, ax
    mov ds, ax
    mov es, ax
    mov ss, ax
    ; set up stack
    mov sp, 0x7c00
    mov bp, sp
    ; set up code segment
    jmp 0x0000:.reload_cs

.stage1_msg: db 0xa, 'PhoenixBoot - Stage1', 0xd, 0xa, 0x0
.reload_cs:
    mov si, .stage1_msg
    call print_string
check_int13_ext:
    ; query int 0x13 extensions support
    push dx
    mov ah, 0x41
    mov bx, 0x55aa
    int 0x13
    jc .error
    cmp bx, 0xaa55
    je .continue
.error:
    mov si, .error_msg
    call print_string
    jmp $
.error_msg: db 'int 0x13 not supported', 0x0
.continue:
    pop dx

    ; read stage2 bootloader
    ; disk read routine
    mov ah, 0x02
    ; stage2 size in sectors
    mov al, stage2_size / 512 + 1
    ; low 8 bits cylinder number
    mov ch, 0
    ; sector number (bits 0-5), high two bits of cylinder number(bits 6-7)
    mov cl, 2
    ; head number
    mov dh, 0

    mov di, STAGE2_SEGMENT
    mov es, di
    mov bx, STAGE2_OFFSET
    ; retry 3 times
    mov di, 3
read_stage2:
    ; save GPR's because BIOS might change them
    pusha
    stc
    int 0x13
    popa
    ; carry is set on error
    jnc .done

    ; reset disk and retry loading stage2
    call .disk_reset
    dec di
    test di, di
    jnz read_stage2
    jmp .error

.disk_reset:
    mov ah, 0x00
    int 0x13
    jc .error
    ret
.error:
    mov si, .error_msg
    call print_string
    mov ah, 0x00
    ; wait for keypress
    int 0x16
    ; reboot
    jmp 0xffff:0x0000
.error_msg: db 'Disk read error! Press any key to reboot...', 0x0
.done:
    ; disable a20(just for tests, we will enable it once in protected mode)
    mov ax, 0x2400
    int 0x15

switch_to_protected_mode:
    lgdt [gdtr]

    mov eax, cr0
    or al, 1
    mov cr0, eax
    jmp dword 0x08:protected_mode_main

protected_mode_main:
use32
    ; set up segment registers for protected mode
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    xor ebp, ebp

    ; set up 32 bit stack
    mov esp, 0x7c00
    push dword stage2_size
    ; disk number(low 8 bits)
    and dx, 0xff
    push dword edx
    ; push invalid return address
    push dword 0x0000
    ; jump to stage2
    jmp STAGE2_OFFSET

use16
print_string:
    push ax
    push bx
    push si

    mov ah, 0x0e
    xor bx, bx
.print_char:
    lodsb
    int 0x10
    cmp al, 0
    jne .print_char

    pop si
    pop bx
    pop ax
    ret

%include 'gdt.inc'

; partition table begins here
%if ($ - $$) >= 446
    %fatal 'Bootloader code exceeds 446 bytes'
%endif

times 510 - ($ - $$) db 0
dw 0xaa55

stage2_bin_start:
incbin 'stage2.bin'
stage2_bin_end:
stage2_size equ (stage2_bin_end - stage2_bin_start)