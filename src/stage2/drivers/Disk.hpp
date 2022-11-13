#pragma once

#include <cstdint>

class Partition;

constexpr const uint8_t MAX_DRIVE_COUNT = 16;

class Disk
{
    public:
        inline Disk() = default;
        inline Disk(uint8_t index, uint8_t type, uint16_t cylinders, uint16_t sectors, uint16_t heads)
            : index(index), type(type), cylinders(cylinders), sectors(sectors), heads(heads) { }

        static void DetectAllDrives();

        inline static Disk* GetDrives() { return drives; }
        inline static uint32_t GetDriveCount() { return driveCount; }

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

        Partition GetPartition(uint32_t partitionIndex);
        bool Read(void* buffer, uint64_t offset, uint64_t bytes);

    private:
        static Disk drives[MAX_DRIVE_COUNT];
        static uint32_t driveCount;

        uint8_t index;
        uint8_t type;
        uint16_t cylinders; 
        uint16_t sectors; 
        uint16_t heads; 
};
