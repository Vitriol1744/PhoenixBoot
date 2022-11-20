#include "Volume.hpp"

#include "Arch/arch.hpp"
#include "Arch/x86/x86.hpp"

#include "Utility/libc.hpp"
#include "Utility/PhysicalMemoryManager.hpp"

Volume Volume::volumes[MAX_VOLUMES] = {};
uint32_t Volume::volumeCount = 0;

Volume::Volume(Disk& _disk, uint64_t _partitionIndex, uint64_t _lbaStart, uint64_t _lbaEnd)
{
    this->disk = _disk;
    this->partitionIndex = _partitionIndex;
    this->lbaStart = _lbaStart;
    this->lbaEnd = _lbaEnd;

    uint8_t firstBlock[512];
    if (!Read(firstBlock, 0, 512))
    {
        printf("Failed to read the partition!\n");
        return;
    }
    if (strncmp((char*)firstBlock + 4, "_ECH_FS_", 8) == 0) filesystem = Filesystem::EchFS;
    else printf("Failed to find ECHFS signature!\n");
}

void Volume::DetectVolumes()
{
    uint8_t* temporaryBuffer = reinterpret_cast<uint8_t*>(PhysicalMemoryManager::AllocateBelow1M(512));
    for (uint32_t i = 0x80; i < 0x8f; i++)
    {
        uint8_t driveType;
        uint16_t cylinders, sectors, heads;
        if (!get_drive_parameters(i, &driveType, &cylinders, &sectors, &heads)) continue;
        if (!reset_disk(i)) continue;
        if (!read_sectors(i, 0, 1, 0, temporaryBuffer, 1)) continue;
        Disk disk(i, driveType, cylinders, sectors, heads);

        for (uint32_t part = 0; part < 1; part++)
        {
            uint64_t lbaStart = 0, lbaEnd = 0;
            if (disk.GetPartition(part, lbaStart, lbaEnd))
            {
                volumes[volumeCount++] = Volume(disk, part, lbaStart, lbaEnd);
            }
            else goto loop_end;
        }
    }
loop_end:
    PhysicalMemoryManager::FreeBelow1M(512);
}