#include "LivePreview.h"
void RegisterLiveClass()
{
    WNDCLASSEXA wc = {0};
    wc.cbClsExtra = NULL;
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.cbWndExtra = sizeof(LiveProcData*);
    wc.hbrBackground = (HBRUSH)(COLOR_3DSHADOW);//GetSysColorBrush(COLOR_3DFACE);
    wc.hCursor = LoadCursor(GetModuleHandle(NULL),"IDC_ARROW");
    wc.hIcon = LoadIcon(GetModuleHandle(NULL),MAKEINTRESOURCE(1));
    wc.hIconSm = wc.hIcon;
    wc.hInstance = GetModuleHandle(NULL);
    wc.lpfnWndProc = (WNDPROC)LivePreviewProc;
    wc.lpszClassName = "LivePreview";
    wc.lpszMenuName = NULL;
    wc.style = NULL;
    RegisterClassEx(&wc);
}
int minint(int a,int b)
{
    return a<b?a:b;
}
DWORD WINAPI updater(LPVOID in)
{
    LiveProcData &data = *((LiveProcData*)in);
    DWORD n = STILL_ACTIVE;
    while(n==STILL_ACTIVE){Sleep(50);GetExitCodeThread(data.TLthread,&n);}
    n = STILL_ACTIVE;
    while(n==STILL_ACTIVE){Sleep(50);GetExitCodeThread(data.TRthread,&n);}
    n = STILL_ACTIVE;
    while(n==STILL_ACTIVE){Sleep(50);GetExitCodeThread(data.BLthread,&n);}
    n = STILL_ACTIVE;
    while(n==STILL_ACTIVE){Sleep(50);GetExitCodeThread(data.BRthread,&n);}
    CloseHandle(data.TLthread);
    CloseHandle(data.TRthread);
    CloseHandle(data.BLthread);
    CloseHandle(data.BRthread);
    BYTE* pbits = data.BMPptr;
    pbits--;
    for( int iRow=0; iRow<500 ; ++iRow )
        {
            for( int iCol=0; iCol<500*3; ++iCol )
          {

            pbits++;
              switch(iCol%3)
              {
              case 0://Blue Channel
                  *pbits =minint( (int)data.currentRenderable->image->start[iCol/3][500-iRow].z,255);
                break;
              case 1://Green Channel
                  *pbits =minint( (int)data.currentRenderable->image->start[iCol/3][500-iRow].y,255);
                break;
              case 2://Red Channel
                  *pbits =minint( (int)data.currentRenderable->image->start[iCol/3][500-iRow].x,255);
                break;
              }
          }
        }
        RECT r;
        GetClientRect(data.thisW,&r);
        InvalidateRect(data.thisW,&r,true);

    data.TLthread = CreateThread(NULL,NULL,RenderForGui,(void*)&(data.TL),0,NULL);
    data.TRthread = CreateThread(NULL,NULL,RenderForGui,(void*)&(data.TR),0,NULL);
    data.BLthread = CreateThread(NULL,NULL,RenderForGui,(void*)&(data.BL),0,NULL);
    data.BRthread = CreateThread(NULL,NULL,RenderForGui,(void*)&(data.BR),0,NULL);
        data.Updatethread = CreateThread(NULL,NULL,updater,(void*)&data,NULL,NULL);
}
void PaintPreview(HDC hdc,HBITMAP &i,HDC &memDC)
{
    BitBlt(hdc,0,0,500,500,memDC,0,0,SRCCOPY);
}
LRESULT CALLBACK LivePreviewProc(HWND WinHandle, UINT message, WPARAM wparam, LPARAM lparam)
{
    LiveProcData* data;
    if(message==WM_PAINT||message==WM_DESTROY||message == WM_LBUTTONUP)
        data = (LiveProcData*)GetWindowLong(WinHandle,0);
    switch(message)
    {
    case WM_CREATE: // CreateWindowEx last parameter is a ptr to a dynamically allocated renderable
        {

        SetWindowLong(WinHandle,0,(LONG)new LiveProcData);
        data = (LiveProcData*)GetWindowLong(WinHandle,0);
        BITMAPINFO bminh = {0};
        BITMAPINFOHEADER &bmi = bminh.bmiHeader;
        data->thisW = WinHandle;
        bmi.biSize          = sizeof(BITMAPINFOHEADER);
        bmi.biWidth      = 500;
        bmi.biHeight      = 500; //positive number == bottom-up DIB
        bmi.biPlanes          = 1;
        bmi.biBitCount          = 24;
        bmi.biCompression = BI_RGB;
        bmi.biClrUsed                = 256;
        data->currentimage = (HBITMAP)CreateDIBSection(NULL,&bminh,DIB_RGB_COLORS,(void**)&(data->BMPptr),0,0);
        data->currentRenderable = (renderable*)((CREATESTRUCT*)lparam)->lpCreateParams;
        data->currentRenderable->h = 250;
        data->currentRenderable->w = 250;
        data->currentRenderable->image = new d3vecImage(501,501);
        HDC Olddc = GetDC(WinHandle);
        data->memDC = CreateCompatibleDC(Olddc);
        ReleaseDC(WinHandle,Olddc);
        SelectObject(data->memDC,data->currentimage);
        data->TL = *(data->currentRenderable);
        data->TL.sx = 0;
        data->TL.sy = 0;
        data->TR = *(data->currentRenderable);
        data->TR.sx = 250;
        data->TR.sy = 0;
        data->BR = *(data->currentRenderable);
        data->BR.sx = 250;
        data->BR.sy = 250;
        data->BL = *(data->currentRenderable);
        data->BL.sx = 0;
        data->BL.sy = 250;
        data->TLthread = CreateThread(NULL,0,RenderForGui,(void*)&(data->TL),0,NULL);
        data->TRthread = CreateThread(NULL,0,RenderForGui,(void*)&(data->TR),0,NULL);
        data->BLthread = CreateThread(NULL,0,RenderForGui,(void*)&(data->BL),0,NULL);
        data->BRthread = CreateThread(NULL,0,RenderForGui,(void*)&(data->BR),0,NULL);
        data->Updatethread = CreateThread(NULL,NULL,updater,(void*)data,NULL,NULL);
        }
        break;
    case WM_PAINT:
        {

        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(WinHandle,&ps);
        BitBlt(hdc,0,0,500,500,data->memDC,0,0,SRCCOPY);
        EndPaint(WinHandle,&ps);
        }
        break;
    case WM_LBUTTONUP:
        break;
    case WM_DESTROY:
        return 0;
        break;
    }
    return DefWindowProc(WinHandle,message,wparam,lparam);
}
