#pragma once

#include "common.hpp"

#include "Drivers/BlockDevice.hpp"

#include "Filesystem/File.hpp"
#include "Filesystem/EchFsFile.hpp"

#include "Utility/PhysicalMemoryManager.hpp"

static constexpr const uint32_t MAX_VOLUMES = 256;

enum class Filesystem
{
    Unknown,
    EchFS,
};

class Volume
{
    public:
        Volume() = default;
        Volume(BlockDevice& blockDevice, uint64_t partitionIndex, uint64_t lbaStart, uint64_t lbaEnd);

        static void DetectVolumes();
        inline static Volume* GetVolumes() { return volumes; }
        inline static uint32_t GetVolumeCount() { return volumeCount; }

        inline File* OpenFile(const char* filename)
        {
            File* ret;
            switch (filesystem)
            {
                case Filesystem::EchFS:
                    ret = reinterpret_cast<File*>(PhysicalMemoryManager::Allocate(sizeof(EchFsFile)));
                    new(ret)EchFsFile(this);
                    break;

                default:
                    ret = nullptr;
                    break;
            }

            if (ret != nullptr && !ret->Open(filename)) ret = nullptr;
            return ret;
        }

        inline bool Read(void* buffer, uint64_t offset, uint64_t bytes) { return blockDevice.Read(buffer, lbaStart * 512 + offset, bytes); }

    private:
        static Volume volumes[MAX_VOLUMES];
        static uint32_t volumeCount;

        BlockDevice blockDevice;
        uint64_t partitionIndex;
        uint64_t lbaStart;
        uint64_t lbaEnd;
        Filesystem filesystem;
};