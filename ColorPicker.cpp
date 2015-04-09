#include <windows.h>
#include "ColorPicker.h"
#include <math.h>
struct BitMAPINFO
{
        BITMAPINFOHEADER bmiHeader;
        RGBQUAD                bmiColors[256];
} ;
COLORREF Normalizecref(COLORREF in)
{
    float factor = 255.0/sqrt(GetRValue(in)*GetRValue(in)+GetGValue(in)*GetGValue(in)+GetBValue(in)*GetBValue(in));
    return RGB(GetRValue(in)*factor,GetGValue(in)*factor,GetBValue(in)*factor);
}
void DisplayDIB(HWND hwnd, HDC hdc, COLORREF current)
{

        HDC memDC;
        HBITMAP hBM ;
        BitMAPINFO bi = {0};

        bi.bmiHeader.biSize          = sizeof(BITMAPINFOHEADER);
        bi.bmiHeader.biWidth      = 255;
        bi.bmiHeader.biHeight      = 315; //positive number == bottom-up DIB
        bi.bmiHeader.biPlanes          = 1;
        bi.bmiHeader.biBitCount          = 24;
        bi.bmiHeader.biCompression = BI_RGB;
        bi.bmiHeader.biClrUsed                = 256;

            BYTE* pBits;
        hBM = (HBITMAP)CreateDIBSection( hdc, (BITMAPINFO*)&bi, DIB_RGB_COLORS, (VOID**)&pBits, 0, 0 );
        BYTE* cache = pBits;
        pBits--;
        int intensity = GetBValue(current)+GetRValue(current)+GetGValue(current);
        for( int iRow=0; iRow<315 ; ++iRow )
        {
            for( int iCol=0; iCol<3*256; ++iCol )
          {

                  pBits++;
              if(iCol<3*255&&iRow<255)
              {
              switch(iCol%3)
              {
              case 0://Blue Channel
                  *pBits = (int)(iCol*(1/3.0)) %255;
                break;
              case 1://Green Channel
                  *pBits = (int)(iRow*(1)) %255;
                break;
              case 2://Red Channel
                  *pBits = intensity-(((iCol/3)%255)+iRow%255);
                break;
              }
              }
              if(iRow>=255)
              {
              switch(iCol%3)
              {
              case 0://Blue Channel
                  *pBits = (int)iCol/3;
                break;
              case 1://Green Channel
                  *pBits = (int)iCol/3;
                break;
              case 2://Red Channel
                  *pBits = (int)iCol/3;
                break;
              }
              }
          }
        }
        bool dotmask[8][8] = {
        {0,0,0,1,1,0,0,0},
        {0,0,1,1,1,1,0,0},
        {0,1,1,1,1,1,1,0},
        {1,1,1,1,1,1,1,1},
        {1,1,1,1,1,1,1,1},
        {0,1,1,1,1,1,1,0},
        {0,0,1,1,1,1,0,0},
        {0,0,0,1,1,0,0,0}
        };
        //current = Normalizecref(current);
        int dotx = GetBValue(current)-4;
        int doty = GetGValue(current)-4;
        for(int x= 0;x<8;x++)
        {
            for(int y = 0;y<8;y++)
            {
                if(dotmask[x][y]&&(doty+y>0&&doty+y<255&&dotx+x>0&&dotx+x<255))
                {
                    BYTE* pixelstart = cache +( (doty+y)*768 + (dotx+x)*3);
                    *pixelstart = 255-*pixelstart;
                    *(pixelstart+1) = 255 - *(pixelstart+1);
                    *(pixelstart+2) = 255 - *(pixelstart+2);
                }
            }
        }

        memDC = CreateCompatibleDC( hdc );
        SelectObject( memDC,hBM );
        BitBlt( hdc,0, 0, 500, 500, memDC, 0, 0, SRCCOPY );

        free(pBits);
        DeleteDC(memDC);
        DeleteObject(hBM);
}

int max(int a , int b)
{
    return a>b?a:b;
}

int min(int a , int b)
{
    return a<b?a:b;
}


void SetBlockColor(HWND block,COLORREF c)
{
    ColorBlockData* ptr = (ColorBlockData *)GetWindowLong(block,0);
    ptr->currentcolor = c;

            RECT b;
            GetClientRect(block,&b);
            InvalidateRect(block,&b,true);
}
void SetBlockColor(HWND parent,HMENU objindex,COLORREF c)
{
    SetBlockColor(GetDlgItem(parent,(int) objindex),c);
}
#define Cpicker 27
#include "main.h"
LRESULT CALLBACK ColorBlockProc(HWND WinHandle,UINT message, WPARAM wparam,LPARAM lparam)
{
    switch(message)
    {
    case WM_DESTROY:
        {
            *(((ColorBlockData *)GetWindowLong(WinHandle,0))->handlevar) = NULL;
            return 0;
        }
    case WM_CREATE:
        {

        WNDCLASSEX CPickerclass = {0};
        CPickerclass.cbClsExtra = NULL;
        CPickerclass.cbSize = sizeof(WNDCLASSEX);
        CPickerclass.cbWndExtra = sizeof(COLORREF*);
        CPickerclass.hbrBackground = (HBRUSH)(COLOR_3DSHADOW);
        CPickerclass.hCursor = LoadCursor(GetModuleHandle(NULL),"IDC_ARROW");
        CPickerclass.hIcon = LoadIcon(GetModuleHandle(NULL),MAKEINTRESOURCE(1));
        CPickerclass.hIconSm = CPickerclass.hIcon;
        CPickerclass.hInstance = GetModuleHandle(NULL);
        CPickerclass.lpfnWndProc = (WNDPROC)ColorPickerProc;
        CPickerclass.lpszClassName = "CPicker";
        CPickerclass.lpszMenuName = NULL;
        CPickerclass.style = NULL;
        RegisterClassEx(&CPickerclass);

            CREATESTRUCT* in = (CREATESTRUCT*)lparam;
            SetWindowLong(WinHandle,0,(LONG) new ColorBlockData);
            ((ColorBlockData *)GetWindowLong(WinHandle,0))->handlevar = (HWND*)(in->lpCreateParams);
            *((HWND*)(in->lpCreateParams)) = WinHandle;
            break;
        }
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            RECT rc;
            HDC hdc;
            GetClientRect(WinHandle,&rc);
            hdc = BeginPaint(WinHandle,&ps);
            //DisplayDIB(WinHandle,hdc);
            SetBkMode(hdc, OPAQUE);
            FillRect(hdc,&rc,CreateSolidBrush(((ColorBlockData *)GetWindowLong(WinHandle,0))->currentcolor));
            //FillRect(hdc,&rc,CreateSolidBrush(RGB(127,0,255)));
            EndPaint(WinHandle,&ps);
            break;
        }
    case WM_LBUTTONUP:
        {

            if(CreateWindowEx(WS_EX_CLIENTEDGE|WS_EX_OVERLAPPEDWINDOW|WS_EX_APPWINDOW,"CPicker","Pick A Color",WS_VISIBLE|WS_DLGFRAME|WS_SYSMENU|WS_MINIMIZEBOX,CW_USEDEFAULT,CW_USEDEFAULT,276,365,WinHandle,NULL,GetModuleHandle(NULL),(LPVOID)GetDlgCtrlID(WinHandle))==0)
            MessageBox(WinHandle,"safd",intToString(GetLastError()).c_str(),MB_ICONERROR);
            break;
        }
    }
    return DefWindowProc(WinHandle,message,wparam,lparam);
}


LRESULT CALLBACK ColorPickerProc(HWND WinHandlel, UINT message, WPARAM wparam, LPARAM lparam)
{
    ColorPickerData* data;
    if(message == WM_LBUTTONDOWN||message==WM_LBUTTONUP||message==WM_MOUSELEAVE||message==WM_MOUSEMOVE||message==WM_PAINT||message==WM_DESTROY)
        data = (ColorPickerData*)GetWindowLong(WinHandlel,0);

    switch(message)
    {
    case WM_CREATE:
        {
            SetWindowLong(WinHandlel,0,(LONG)new ColorPickerData({RGB(127,127,127),(int)((CREATESTRUCT*)lparam)->lpCreateParams,false}));
            return 0;
        }
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            RECT rc;
            HDC hdc;
            GetClientRect(WinHandlel,&rc);
            hdc = BeginPaint(WinHandlel,&ps);
            DisplayDIB(WinHandlel,hdc,data->currentcolor);
            EndPaint(WinHandlel,&ps);
            break;
        }
    case WM_LBUTTONDOWN:
        if(!data->hidden)
           {
            ShowCursor(false);
            data->hidden =true;
           }
    case WM_MOUSEMOVE:
        {
            if(data->hidden){
            TRACKMOUSEEVENT tme = {sizeof(TRACKMOUSEEVENT),2,WinHandlel,HOVER_DEFAULT};
            TrackMouseEvent(&tme);}
        if((!(wparam&1) ))//|| HIWORD(lparam)<60)
            break;

            RECT fd;
            GetClientRect(WinHandlel,&fd);
            COLORREF* ptr = (COLORREF *)GetWindowLong(WinHandlel,0);
            if(HIWORD(lparam)>=60)
            *ptr = RGB((GetRValue(*ptr)+GetGValue(*ptr)+GetBValue(*ptr))-(315-HIWORD(lparam)+LOWORD(lparam)),315-HIWORD(lparam),LOWORD(lparam));
            else
            {
                if(GetRValue(*ptr)==0&&GetGValue(*ptr)==0&&GetBValue(*ptr)==0)
                    *ptr = RGB(1,1,1);
                float factor = LOWORD(lparam)*3/(float)(GetRValue(*ptr)+GetGValue(*ptr)+GetBValue(*ptr));
                *ptr = RGB(min((int)((float)GetRValue(*ptr)*factor) ,255),
                           min((int)((float)GetGValue(*ptr)*factor) ,255),
                           min((int)((float)GetBValue(*ptr)*factor) ,255)
                           );
            }
            SetBlockColor(GetWindow(WinHandlel,GW_OWNER),(HMENU)((ColorPickerData*)GetWindowLong(WinHandlel,0))->ownerblockid,((ColorPickerData*)GetWindowLong(WinHandlel,0))->currentcolor);
            InvalidateRect(WinHandlel,&fd,false);
            break;

        }

    case WM_MOUSELEAVE:
        MessageBeep(MB_ICONERROR);
    case WM_LBUTTONUP:
        if(data->hidden)
           {
            ShowCursor(true);
            data->hidden =false;
           }
        if(message == WM_MOUSELEAVE)
            return 0;
        break;
    case WM_DESTROY:
        {
            delete  data;
        }
    }
    return DefWindowProc(WinHandlel,message,wparam,lparam);
}
