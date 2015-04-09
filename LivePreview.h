#ifndef LIVEPREVIEW_H_INCLUDED
#define LIVEPREVIEW_H_INCLUDED
#include <windows.h>
#include "main.h"
void PaintPreview(HDC hdc,HBITMAP &i,HDC &memDC);
void RegisterLiveClass();
LRESULT CALLBACK LivePreviewProc(HWND WinHandle, UINT message, WPARAM wparam, LPARAM lparam);
/*
typedef struct
{
    BVH inBVH;
    Camera precam;
    d3vecImage* image;
    int sx,sy,w,h;
    HANDLE renderMutex;
}renderable;
*/
typedef struct
{
    renderable* currentRenderable;
    renderable TL;
    renderable TR;
    renderable BL;
    renderable BR;
    HBITMAP currentimage;
    BYTE* BMPptr;
    HANDLE ExitEvent;
    HANDLE Mutex;
    HANDLE TLthread;
    HANDLE TRthread;
    HANDLE BRthread;
    HANDLE BLthread;
    HANDLE Updatethread;
    HDC memDC;
    HWND thisW;
}LiveProcData;

#endif // LIVEPREVIEW_H_INCLUDED
