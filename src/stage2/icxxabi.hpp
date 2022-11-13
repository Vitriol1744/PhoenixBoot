#pragma once

using DestructorFunction = void(*)(void*);

extern "C"
{
    int     __cxa_atexit(DestructorFunction func, void* objptr, void* dso);
    void    __cxa_finalize(void* f);
}