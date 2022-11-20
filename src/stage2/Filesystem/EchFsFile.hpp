#pragma once

#include "Filesystem/File.hpp"

class Partition;

class EchFsFile : public File
{
    public:
        EchFsFile(Volume* volume) : File(volume) { }
        ~EchFsFile() = default;

        virtual bool Open(const char* filename) override;
        virtual bool Read(void* buffer, uint64_t bytes) override;

    private:
        uint32_t startLBA;
        uint32_t filePtr = 0;
};