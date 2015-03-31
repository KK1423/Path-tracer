#include <windows.h>
#include <iostream>
#include "main.h"
#include "ColorPicker.h"
LRESULT CALLBACK MessageHandler(HWND WinHandle, UINT message, WPARAM wparam, LPARAM lparam);
//LRESULT CALLBACK ColorPickerProc(HWND WinHandle, UINT message, WPARAM wparam, LPARAM lparam);
void MakeButton(LPCSTR caption,DWORD style,int a,int b,int c,int d, int number,HWND parent,LPVOID state,LPCSTR type = "BUTTON",DWORD i = WS_EX_WINDOWEDGE)
{
    CreateWindowEx(i,type,caption,style,a,b,c,d,parent,(HMENU)number, GetModuleHandle(NULL),state);
}
int WINAPI WinMain(HINSTANCE WinHandle,HINSTANCE prevWinHandle, LPSTR lpCmdLine, int nShowCmd)
{
    WNDCLASSA wc = {0};
    wc.lpszClassName = TEXT( "tracerclss" );
    wc.hInstance     = WinHandle ;
    wc.hbrBackground = GetSysColorBrush(COLOR_3DFACE);
    wc.lpfnWndProc   = (WNDPROC)MessageHandler ;
    wc.hCursor       = LoadCursor(0, IDC_ARROW);
    wc.hIcon         = LoadIcon(WinHandle,MAKEINTRESOURCE(1));


    RegisterClass(&wc);
    CreateWindow( wc.lpszClassName, "Pathtracer",
                  WS_OVERLAPPEDWINDOW^WS_THICKFRAME | WS_VISIBLE,
                  CW_USEDEFAULT, CW_USEDEFAULT, 600, 600, 0, 0, WinHandle, 0);

    MSG msg;
    ZeroMemory(&msg,sizeof(MSG));
    while(GetMessage(&msg,NULL,0,0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return 0;

}
#define CameraGroup 1
#define XUP 2
#define YUP 3
#define ZUP 4
#define XFO 5
#define YFO 6
#define ZFO 7
#define CUS 8
#define AZI 9
#define ALT 10
#define APR 11
#define FDI 12
#define UPD 13
#define ObjectListGroup 14
#define OLB 15
#define ADD 16
#define DEL 17
#define MaterialEditor 18
#define ALP 19
#define DIF 20
#define IOR 21
#define EMI 22
#define CBL 23
using namespace std;
HWND blockhandle = NULL;
LRESULT CALLBACK MessageHandler(HWND WinHandle, UINT message, WPARAM wparam, LPARAM lparam)
{
    cout<<(int)message<<endl;
    switch(message)
    {
    case WM_CTLCOLORSTATIC:
    {
        if(GetDlgCtrlID((HWND)lparam)==14)
        {
        HDC hdcStatic = (HDC) wparam;
        SetTextColor(hdcStatic, RGB(0,0,0));
        SetBkMode (hdcStatic, BLACKNESS);
        return (LRESULT)GetStockObject(NULL_BRUSH);
        }
        break;

    }
    case WM_CREATE:
    {
        CreateWindowEx(WS_EX_WINDOWEDGE,
                       "BUTTON",
                       "Camera Settings",
                       BS_GROUPBOX|WS_CHILD|WS_VISIBLE,
                       5,5,290,145,
                       WinHandle,
                       (HMENU)CameraGroup,
                       GetModuleHandleA(NULL),
                       0
                      );
        MakeButton("XUP",BS_AUTORADIOBUTTON|WS_GROUP|WS_CHILD|WS_VISIBLE|WS_TABSTOP,10,30,100,20,XUP,WinHandle,0);
        MakeButton("YUP",BS_AUTORADIOBUTTON|WS_CHILD|WS_VISIBLE|WS_TABSTOP,110,30,100,20,YUP,WinHandle,0);
        MakeButton("ZUP",BS_AUTORADIOBUTTON|WS_CHILD|WS_VISIBLE|WS_TABSTOP,210,30,80,20,ZUP,WinHandle,0);
        CheckDlgButton(WinHandle,ZUP,BST_CHECKED);

        MakeButton("XFo",BS_AUTORADIOBUTTON|WS_GROUP|WS_CHILD|WS_VISIBLE|WS_TABSTOP,10,50,100,20,XFO,WinHandle,0);
        CheckDlgButton(WinHandle,XFO,BST_CHECKED);
        MakeButton("YFo",BS_AUTORADIOBUTTON|WS_CHILD|WS_VISIBLE|WS_TABSTOP,110,50,100,20,YFO,WinHandle,0);
        MakeButton("ZFo",BS_AUTORADIOBUTTON|WS_CHILD|WS_VISIBLE|WS_TABSTOP,210,50,80,20,ZFO,WinHandle,0);

        MakeButton("Custom:",BS_AUTOCHECKBOX|WS_GROUP|WS_CHILD|WS_VISIBLE|WS_TABSTOP,10,70,100,20,CUS,WinHandle,0);

        MakeButton("",WS_CHILD|WS_VISIBLE|WS_TABSTOP|ES_AUTOHSCROLL,160,70,30,20,AZI,WinHandle,0,"EDIT",WS_EX_CLIENTEDGE|WS_EX_WINDOWEDGE);
        MakeButton("",WS_CHILD|WS_VISIBLE|WS_TABSTOP|ES_AUTOHSCROLL,260,70,30,20,ALT,WinHandle,0,"EDIT",WS_EX_CLIENTEDGE|WS_EX_WINDOWEDGE);
        CreateWindow("STATIC","AZI:",WS_VISIBLE|WS_CHILD,110,70,30,20,WinHandle,NULL,GetModuleHandle(NULL),NULL);
        CreateWindow("STATIC","ALT:",WS_VISIBLE|WS_CHILD,210,70,40,20,WinHandle,NULL,GetModuleHandle(NULL),NULL);

        MakeButton("",WS_CHILD|WS_VISIBLE|WS_TABSTOP|ES_AUTOHSCROLL,110,90,50,20,APR,WinHandle,0,"EDIT",WS_EX_CLIENTEDGE|WS_EX_WINDOWEDGE);
        MakeButton("",WS_CHILD|WS_VISIBLE|WS_TABSTOP|ES_AUTOHSCROLL,240,90,50,20,FDI,WinHandle,0,"EDIT",WS_EX_CLIENTEDGE|WS_EX_WINDOWEDGE);
        CreateWindow("STATIC","Aperture",WS_VISIBLE|WS_CHILD,10,90,100,20,WinHandle,NULL,GetModuleHandle(NULL),NULL);
        CreateWindow("STATIC","Focus",WS_VISIBLE|WS_CHILD,160,90,70,20,WinHandle,NULL,GetModuleHandle(NULL),NULL);

        MakeButton("Update",BS_DEFPUSHBUTTON|WS_GROUP|WS_CHILD|WS_VISIBLE,10,115,280,30,UPD,WinHandle,0);

        CreateWindowEx(WS_EX_WINDOWEDGE,
                       "BUTTON",
                       "Objects",
                       BS_GROUPBOX|WS_CHILD|WS_VISIBLE,
                       5,150,290,145,
                       WinHandle,
                       (HMENU)ObjectListGroup,
                       GetModuleHandle(NULL),
                       0
                      );


        CreateWindowEx(WS_EX_CLIENTEDGE|WS_EX_WINDOWEDGE,"listbox","",WS_CHILD|WS_VISIBLE|WS_VSCROLL|ES_AUTOVSCROLL,10,175,280,100,WinHandle,(HMENU)OLB,GetModuleHandle(NULL),0);


        MakeButton("Add",BS_DEFPUSHBUTTON|WS_GROUP|WS_CHILD|WS_VISIBLE,10,264,135,25,ADD,WinHandle,0);
        MakeButton("Delete",BS_DEFPUSHBUTTON|WS_GROUP|WS_CHILD|WS_VISIBLE,150,264,135,25,DEL,WinHandle,0);

        CreateWindowEx(WS_EX_WINDOWEDGE,
                       "BUTTON",
                       "Material",
                       BS_GROUPBOX|WS_CHILD|WS_VISIBLE,
                       5,295,290,267,
                       WinHandle,
                       (HMENU)MaterialEditor,
                       GetModuleHandle(NULL),
                       0
                      );

        MakeButton("",WS_CHILD|WS_VISIBLE|WS_TABSTOP|ES_AUTOHSCROLL,95,320,50,20,ALP,WinHandle,0,"EDIT",WS_EX_CLIENTEDGE|WS_EX_WINDOWEDGE);
        MakeButton("",WS_CHILD|WS_VISIBLE|WS_TABSTOP|ES_AUTOHSCROLL,95,340,50,20,DIF,WinHandle,0,"EDIT",WS_EX_CLIENTEDGE|WS_EX_WINDOWEDGE);
        MakeButton("",WS_CHILD|WS_VISIBLE|WS_TABSTOP|ES_AUTOHSCROLL,95,360,50,20,IOR,WinHandle,0,"EDIT",WS_EX_CLIENTEDGE|WS_EX_WINDOWEDGE);
        MakeButton("",WS_CHILD|WS_VISIBLE|WS_TABSTOP|ES_AUTOHSCROLL,95,380,50,20,EMI,WinHandle,0,"EDIT",WS_EX_CLIENTEDGE|WS_EX_WINDOWEDGE);
        CreateWindow("STATIC","Alpha:",WS_VISIBLE|WS_CHILD,10,320,75,20,WinHandle,NULL,GetModuleHandle(NULL),NULL);
        CreateWindow("STATIC","Diffusion:",WS_VISIBLE|WS_CHILD,10,340,75,20,WinHandle,NULL,GetModuleHandle(NULL),NULL);
        CreateWindow("STATIC","IOR:",WS_VISIBLE|WS_CHILD,10,360,75,20,WinHandle,NULL,GetModuleHandle(NULL),NULL);
        CreateWindow("STATIC","Intensity:",WS_VISIBLE|WS_CHILD,10,380,75,20,WinHandle,NULL,GetModuleHandle(NULL),NULL);
        CreateWindow("STATIC","Color:",WS_VISIBLE|WS_CHILD,10,400,75,20,WinHandle,NULL,GetModuleHandle(NULL),NULL);

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


        WNDCLASSEXA ColorBlock = {0};
        ColorBlock.cbClsExtra = NULL;
        ColorBlock.cbSize = sizeof(WNDCLASSEX);
        ColorBlock.cbWndExtra = sizeof(ColorBlockData*);
        ColorBlock.hbrBackground = (HBRUSH)(COLOR_3DSHADOW);//GetSysColorBrush(COLOR_3DFACE);
        ColorBlock.hCursor = LoadCursor(GetModuleHandle(NULL),"IDC_ARROW");
        ColorBlock.hIcon = LoadIcon(GetModuleHandle(NULL),MAKEINTRESOURCE(1));
        ColorBlock.hIconSm = ColorBlock.hIcon;
        ColorBlock.hInstance = GetModuleHandle(NULL);
        ColorBlock.lpfnWndProc = (WNDPROC)ColorBlockProc;
        ColorBlock.lpszClassName = "ColorBlock";
        ColorBlock.lpszMenuName = NULL;
        ColorBlock.style = NULL;
        RegisterClassEx(&ColorBlock);
       /* CreateWindowEx(NULL,ColorBlock.lpszClassName,"",WS_VISIBLE|WS_CHILD, 95,400,50,20, WinHandle,(HMENU) CBL,GetModuleHandle(NULL),0);
        SetWindowLong(GetDlgItem(WinHandle,CBL),0,(LONG) new ColorBlockData);
        SetBlockColor(WinHandle,(HMENU)CBL,RGB(255,127,0));*/

        CreateWindowEx( WS_EX_CLIENTEDGE,ColorBlock.lpszClassName, "Pathtracer",
                  WS_VISIBLE | WS_CHILD,
                  95, 400, 50, 20, WinHandle, (HMENU) CBL, GetModuleHandle(NULL), &blockhandle);

    }
    break;
    case WM_COMMAND:
        {
            switch(LOWORD(wparam))
            {
            case UPD:
                {
                    if(blockhandle!=NULL)
                            SetBlockColor(blockhandle,RGB(rand()%255,rand()%255,rand()%255));
                    break;
                }
            case ADD:
                {

                }
            }
            break;
        }
    case WM_POWERBROADCAST:
        SetBlockColor(WinHandle,(HMENU)CBL,RGB(0,255,0));
        break;
    case WM_DESTROY:
    {
        PostQuitMessage(0);
        return 0;
        break;
    }
    }
    return DefWindowProc(WinHandle,message,wparam,lparam);
}
