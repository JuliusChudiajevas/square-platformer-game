#ifndef RENDERSTATE_H_
#define RENDERSTATE_H_
#include <windows.h>
struct RenderState {
    int height, widht;
    void *memory;

    BITMAPINFO bitmapInfo;
};
#endif