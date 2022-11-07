#pragma once

#include <cstdint>

void __attribute__((cdecl)) get_drive_parameters(uint8_t drive, uint8_t* driveType, uint16_t* cylinders, uint16_t* sectors, uint16_t* heads);