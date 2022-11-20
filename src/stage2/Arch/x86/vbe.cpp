#include "vbe.hpp"

#include "Utility/libc.hpp"
#include "Utility/Logger.hpp"
#include "Utility/PhysicalMemoryManager.hpp"

#define SegOff2Linear(seg, off) (seg << 4 + off)
#define DIFF(a, b) (a - b > 0 ? a - b : b - a)

#define MEMORY_MODEL_PACKED_PIXEL       0x04
#define MEMORY_MODEL_DIRECT_COLOR       0x06

uint16_t findMode(uint32_t width, uint32_t height, uint32_t bpp)
{
    VbeControllerInfo* controllerInfo = (VbeControllerInfo*)PhysicalMemoryManager::AllocateBelow1M(sizeof(VbeControllerInfo));
    uint16_t best = 0x13;

    if (!vbe_get_controller_info(controllerInfo)) return best;
    
    uint32_t bestpixdiff = DIFF(320 * 200, width * height);
    uint32_t bestdepthdiff = 8 >= bpp ? 8 - bpp : (bpp - 8) * 2;
    
    uint16_t* modes = (uint16_t*)SegOff2Linear(controllerInfo->videoModesPointer.segment, controllerInfo->videoModesPointer.offset);
    VbeModeInfo* modeInfo = (VbeModeInfo*)0x2000;
    for (uint16_t i = 0; modes[i] != 0xffff ; i++) 
    {
        if (!vbe_get_mode_info(i, modeInfo)) continue;

        // Is Graphics Mode
        if ((modeInfo->attributes & 0x90) != 0x90) continue;
    
        // Check if this is a packed pixel or direct color mode
        if (modeInfo->memoryModel != MEMORY_MODEL_PACKED_PIXEL && modeInfo->memoryModel != MEMORY_MODEL_DIRECT_COLOR) continue;

        if (width == modeInfo->width && height == modeInfo->height && bpp == modeInfo->bpp) return i;
    
        uint32_t pixdiff = DIFF(modeInfo->width * modeInfo->height, width * height);
        uint32_t depthdiff = (modeInfo->bpp >= bpp) ? modeInfo->bpp - bpp : (bpp - modeInfo->bpp) * 2;
        if (pixdiff < bestpixdiff || (bestpixdiff == pixdiff && depthdiff < bestdepthdiff)) 
        {
            best = i;
            bestpixdiff = pixdiff;
            bestdepthdiff = depthdiff;
        }
    }

    if (width == 640 && height == 480 && bpp == 1) return 0x11;
    return best;
}

bool vbeInitializeGraphicsMode(VbeModeInfo& _modeInfo)
{
    uint16_t bestMode = findMode(1024, 768, 32);
    VbeModeInfo modeInfo;
    vbe_get_mode_info(bestMode, &modeInfo);
    
    LOG_INFO("Width: %d, Height: %d, BitsPerPixel: %d", modeInfo.width, modeInfo.height, modeInfo.bpp);

    vbe_set_video_mode(bestMode);
    memcpy(&_modeInfo, &modeInfo, sizeof(VbeModeInfo));
    return true;
}