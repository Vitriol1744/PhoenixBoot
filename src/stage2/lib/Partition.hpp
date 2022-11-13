#pragma once

#include <cstdint>

#include "drivers/Disk.hpp"

#include "filesystem/EchFsFile.hpp"

#include "lib/PhysicalMemoryManager.hpp"

#include <new>

enum class Filesystem
{
    Unknown,
    EchFS,
};

class File;

class Partition
{
    public:
        inline Partition(Disk& disk, uint32_t partitionStartLBA, uint32_t partitionEndLBA)
            : disk(disk), partitionStartLBA(partitionStartLBA), partitionEndLBA(partitionEndLBA) 
        {
            uint8_t firstBlock[512];
            Read(firstBlock, 0, 512);
            
            if (strncmp((char*)firstBlock + 4, "_ECH_FS_", 8) == 0) filesystem = Filesystem::EchFS;
        }

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

            if (!ret->Open(filename)) ret = nullptr;
            return ret;
        }

        inline bool Read(void* buffer, uint64_t offset, uint64_t bytes) { return disk.Read(buffer, partitionStartLBA * 512 + offset, bytes); }

    private:
        Disk disk;
        uint32_t partitionStartLBA;
        uint32_t partitionEndLBA;
        Filesystem filesystem;
};
