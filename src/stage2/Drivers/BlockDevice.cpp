#include "BlockDevice.hpp"

//TODO: Temp
#include "Arch/x86/x86.hpp"

#include "Drivers/Terminal.hpp"

#include "Utility/libc.hpp"
#include "Utility/PhysicalMemoryManager.hpp"

bool BlockDevice::GetPartition(uint32_t partitionIndex, uint64_t& lbaStart, uint64_t& lbaEnd)
{
    MBR mbr;
    Read(&mbr, 0, 512);
    MBR_Entry* entry = &mbr.entries[partitionIndex];
    printf("PartitionIndex: %d, systemID: %d\n", partitionIndex, entry->systemID);
    if (entry->systemID == 0) return false;
    
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
    
    lbaStart = partitionStartLBA;
    lbaEnd = partitionEndLBA;
    return true;
    //TODO: Add Extended MBR partitions support
    //TODO: Add GPT Support
}
bool BlockDevice::Read(void* buffer, uint64_t offset, uint64_t bytes)
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
