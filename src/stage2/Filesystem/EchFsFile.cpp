#include "EchFsFile.hpp"

#include "Filesystem/Volume.hpp"

#include "Utility/libc.hpp"

struct EchFSIdentityTable
{
    uint32_t jumpInstruction;
    uint8_t signature[8];
    uint64_t blockCount;
    // in blocks
    uint64_t mainDirectoryLength;
    uint64_t bytesPerBlock;
    uint64_t reserved;
    uint64_t partitionUUID[2];
} __attribute__((packed));

struct EchFSDirectoryEntry
{
    uint64_t directoryID;
    uint8_t objectType;
    uint8_t name[201];
    uint64_t unixATime;
    uint64_t unixMTime;
    uint16_t permissions;
    uint16_t ownerID;
    uint16_t groupID;
    uint64_t unixCTime;
    uint64_t startingBlock;
    uint64_t fileSize;
} __attribute__((packed));

bool EchFsFile::Open(const char* filename)
{
    EchFSIdentityTable identityTable;
    if (volume->Read(&identityTable, 0, sizeof(identityTable)));

    uint32_t blockSize = identityTable.bytesPerBlock;

    //TODO: Write unit tests
    uint32_t allocationTableOffset = 16;
    uint32_t allocationTableSize = (uint32_t)(identityTable.blockCount * sizeof(uint64_t) + blockSize - 1) / (uint32_t)blockSize;
    uint32_t mainDirectoryOffset = allocationTableOffset + allocationTableSize;
    uint32_t mainDirectoryLength = identityTable.mainDirectoryLength;
    
    //TODO: Add support for subdirectories
    EchFSDirectoryEntry directoryEntry;
    for (uint64_t i = 0; i < mainDirectoryLength * blockSize; i += sizeof(EchFSDirectoryEntry))
    {
        uint64_t offset = mainDirectoryOffset * blockSize + i;
        volume->Read(&directoryEntry, offset, sizeof(EchFSDirectoryEntry));
        if (strcmp((char*)directoryEntry.name, filename) == 0)
        {
            startLBA = directoryEntry.startingBlock;
            size = directoryEntry.fileSize;
            #if 0
            LOG_INFO("file size in blocks: %d\n", (uint32_t)size / blockSize + 1);
            #endif
            break;
        }
    }

    #if 0
    static bool initialized = false;
    if (!initialized)
    {
        LOG_INFO("bytes per block: %d\n", identityTable.bytesPerBlock);
        LOG_INFO("block count: %d\n", identityTable.blockCount);
        LOG_INFO("allocation table size: %d\n", allocationTableSize);
        LOG_INFO("allocation table start: %d\n", allocationTableOffset);
        LOG_INFO("main directory start: %d\n", mainDirectoryOffset);
        LOG_INFO("main directory size: %d\n", mainDirectoryLength);
        LOG_INFO("reserved blocks: %d\n", identityTable.reserved);
        LOG_INFO("usable blocks: %d\n", identityTable.blockCount - identityTable.reserved);
        initialized = true;
    }
    #endif

    return true;
}

bool EchFsFile::Read(void* buffer, uint64_t bytes)
{
    if (bytes > size - filePtr) return false;

    //FIXME: Block is not necessarily 512 bytes
    if (!volume->Read(buffer, startLBA * 512 + filePtr, bytes)) return false;
    filePtr += bytes;

    return true;
}

