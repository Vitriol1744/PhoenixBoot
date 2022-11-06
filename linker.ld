ENTRY(Stage2Main)
OUTPUT_FORMAT("binary")

SECTIONS
{
    . = 0x7e00;

    .entry  : { __entry_start = .;      *(.entry)   }
    .text   : { __text_start = .;       *(.text)    }
    .data   : { __data_start = .;       *(.data)    }
    .rodata : { __rodata_start = .;     *(.rodata)  }
    .bss    : { __bss_start = .;        *(.bss);    }    __bss_end = .;

    .init_array : 
    {
        PROVIDE_HIDDEN (__init_array_start = .);
        KEEP (*(SORT_BY_INIT_PRIORITY(.init_array.*) SORT_BY_INIT_PRIORITY(.ctors.*)))
        KEEP (*(.init_array .ctors))
        PROVIDE_HIDDEN (__init_array_end = .);
    }
}