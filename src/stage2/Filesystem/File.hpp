#pragma once

#include <stdint.h>

class Partition;

class File
{
    public:
        File(Partition* part) { this->part = part; }
        virtual ~File() = default;

        virtual bool Open(const char* filename) = 0;
        virtual bool Read(void* buffer, uint64_t bytes) = 0;
        bool ReadAll(void* buffer) { return Read(buffer, size); }

        inline uint64_t GetSize() const { return size; }

    protected:
        Partition* part;
        uint64_t size;
};
