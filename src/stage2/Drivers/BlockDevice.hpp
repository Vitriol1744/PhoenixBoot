#pragma once

#include <stdint.h>

class Partition;

constexpr const uint8_t MAX_DRIVE_COUNT = 16;

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

class BlockDevice
{
    public:
        inline BlockDevice() = default;
        inline BlockDevice(uint8_t index, uint8_t type, uint16_t cylinders, uint16_t sectors, uint16_t heads)
            : index(index), type(type), cylinders(cylinders), sectors(sectors), heads(heads) { }

        inline void CHS(uint32_t lba, uint16_t& cylinder, uint16_t& sector, uint16_t& head)
        {
            cylinder = static_cast<uint16_t>((lba / sectors) / heads);
            sector = static_cast<uint16_t>((lba % sectors) + 1);
            head = static_cast<uint16_t>((lba / sectors) % heads);
        }
        inline constexpr uint32_t LBA(uint16_t cylinder, uint16_t sector, uint16_t head)
        {
            return (cylinder * heads + head) * sectors + (sector - 1);
        }

        bool GetPartition(uint32_t partitionIndex, uint64_t& lbaStart, uint64_t& lbaEnd);
        bool Read(void* buffer, uint64_t offset, uint64_t bytes);

    private:
        uint8_t index;
        uint8_t type;
        uint16_t cylinders; 
        uint16_t sectors; 
        uint16_t heads; 
};
