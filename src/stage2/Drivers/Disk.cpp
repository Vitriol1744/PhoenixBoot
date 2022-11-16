#include "Disk.hpp"

//TODO: Temp
#include "Arch/x86/x86.h"

#include "Drivers/Terminal.hpp"

#include "Utility/libc.hpp"
#include "Utility/Partition.hpp"
#include "Utility/PhysicalMemoryManager.hpp"

Disk Disk::drives[MAX_DRIVE_COUNT] = {Disk()};
uint32_t Disk::driveCount = 0;

#pragma pack(push, 1)
struct MBR_Entry
{
    uint8_t bootIndicator;
    uint8_t startCHS[3];
    uint8_t systemID;
    uint8_t endCHS[3];
    uint32_t firstSector;
    uint32_t sectorCount;
};
#pragma pack(pop)

struct MBR
{
    char code[446];
    MBR_Entry entries[4];
};

void Disk::DetectAllDrives()
{
    uint8_t* temporaryBuffer = reinterpret_cast<uint8_t*>(PhysicalMemoryManager::AllocateBelow1M(512));
    for (uint32_t i = 0x80; i < 0x8f; i++)
    {
        uint8_t driveType;
        uint16_t cylinders, sectors, heads;
        if (!get_drive_parameters(i, &driveType, &cylinders, &sectors, &heads)) continue;
        if (!reset_disk(i)) continue;
        if (!read_sectors(i, 0, 1, 0, temporaryBuffer, 1)) continue;

        drives[driveCount++] = Disk(i, driveType, cylinders, sectors, heads);
    }
    PhysicalMemoryManager::FreeBelow1M(512);
}

Partition Disk::GetPartition(uint32_t partitionIndex)
{
    //TODO: Add Extended MBR partitions support
    //TODO: Add GPT Support
    MBR mbr;
    Read(&mbr, 0, 512);
    MBR_Entry* entry = &mbr.entries[partitionIndex];
    
    uint16_t cylinder = entry->startCHS[1] & 0b11000000 << 2;
    cylinder |= entry->startCHS[2];
    uint16_t sector = entry->startCHS[1] & 0b00111111;
    uint16_t head = entry->startCHS[0];
    uint32_t partitionStartLBA  = LBA(cylinder, sector, head);
    
    cylinder = entry->endCHS[1] & 0b11000000 << 2;
    cylinder |= entry->endCHS[2];
    sector = entry->endCHS[1] & 0b00111111;
    head = entry->endCHS[0];
    uint32_t partitionEndLBA    = LBA(cylinder, sector, head);
    
    return Partition(*this, partitionStartLBA, partitionEndLBA);
}
bool Disk::Read(void* buffer, uint64_t offset, uint64_t bytes)
{
    uint8_t* temp = reinterpret_cast<uint8_t*>(PhysicalMemoryManager::AllocateBelow1M(512));
    if (!temp) return false;

    uint16_t cylinder, sector, head;
    size_t bytesRead = 0;
    while (bytesRead < bytes)
    {
        size_t lba = (static_cast<uint32_t>(offset) + bytesRead) / 512;

        CHS(lba, cylinder, sector, head);
        if (!read_sectors(index, cylinder, sector, head, temp, 1))
        {
            PhysicalMemoryManager::FreeBelow1M(512);
            return false;
        }

        size_t toRead = static_cast<size_t>(bytes) - bytesRead;
        size_t off = (offset + bytesRead) % 512;
        if (toRead > 512 - off) toRead = 512 - off;

        memcpy(reinterpret_cast<uint8_t*>(buffer) + bytesRead, temp + off, toRead);
        bytesRead += toRead;
    }
    
    PhysicalMemoryManager::FreeBelow1M(512);
    return true;
}        
