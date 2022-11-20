#pragma once

#include <stdint.h>

struct SegOffsetAddress
{
    uint16_t segment;
    uint16_t offset;
} __attribute__((packed));

struct VbeControllerInfo
{
    uint8_t vbeSignature[4];                        // VBE signature - 'VESA'
    uint16_t vbeVersion;                            // VBE version
    SegOffsetAddress oemStringPointer;              // OEM string pointer
    uint8_t capabilities[4];                        // Graphics Controller capabilities
    SegOffsetAddress videoModesPointer;             // Video Mode list pointer
    uint16_t totalMemory;                           // Total memory in 64kb memory blocks
    uint16_t softwareRevision;                      // VBE implementation software revision
    SegOffsetAddress vendorNameStringPointer;       // Product name string pointer
    SegOffsetAddress productNameStringPoitner;      // Product name string pointer
    SegOffsetAddress productRevisionStringPointer;  // Product revision string pointer
    uint8_t reserved[222];                          // Reserved
    uint8_t oemData[256];                           // Data Area for OEM strings
} __attribute__((packed));

struct VbeModeInfo
{
    uint16_t attributes;    // Mode attributes
    uint8_t windowA;        // Deprecated
    uint8_t windowB;        // Deprecated
    uint16_t granularity;   // Deprecated
    uint16_t windowSize;    // Window size
    uint16_t segmentA;      
    uint16_t segmentB;
    uint32_t winFuncPtr;    // Deprecated
    uint16_t pitch;         // Bytes per line
    uint16_t width;         // Framebuffer width
    uint16_t height;        // Framebuffer height
    uint8_t wChar;          // Unused
    uint8_t yChar;          // Unused
    uint8_t planes;         
    uint8_t bpp;            // Bits per Pixel
    uint8_t banks;          // Deprecated
    uint8_t memoryModel;
    uint8_t bankSize;       // Deprecated
    uint8_t imagePages;
    uint8_t reserved0;

    uint8_t redMask;
    uint8_t redPosition;
    uint8_t greenMask;
    uint8_t greenPosition;
    uint8_t blueMask;
    uint8_t bluePosition;
    uint8_t reservedMask;
    uint8_t reservedPosition;
    uint8_t directColorAttributes;

    uint32_t framebufferBase;
    uint32_t framebuffermemoryOffset;
    uint16_t framebuffermemorySize;
    uint8_t reserved1[206];
} __attribute__((packed));

extern "C"
{
    bool __attribute__((cdecl)) vbe_get_controller_info(VbeControllerInfo* buffer);
    bool __attribute__((cdecl)) vbe_get_mode_info(uint16_t modeIndex, VbeModeInfo* vbeModeInfo);
    bool __attribute__((cdecl)) vbe_set_video_mode(uint16_t modeIndex);
}

bool vbeInitializeGraphicsMode(VbeModeInfo& modeInfo);
