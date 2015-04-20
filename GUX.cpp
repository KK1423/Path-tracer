#include <windows.h>
#include <iostream>
#include <math.h>
#include "main.h"
#include "ColorPicker.h"
#include "LivePreview.h"
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
float parsefrfloat(std::string in)
{
    int exp = in.find_first_of('.');
    bool negflag = false;
    if(exp<0)
        exp = in.length();
    exp--;
    int traverse = 0;
    float out = 0.0;
    while(traverse<in.length())
    {
        in.length();
        if(in[traverse]!='.')
        {
            if(in[traverse]=='-')
            {
                out*=-1;
                negflag=true;
            }else
            {
                out+=powf(10.0,(float)exp)*((int)in[traverse]-48)*(negflag?-1:1);
            }
            exp--;
        }
        traverse++;
    }
    return out;
}
Scene guiScene(0,0,0,0);
BVH guiBvh;
Camera guiCamera;
#define CameraGroup 1
#define XUP 2
#define YUP 3
#define ZUP 4
#define XFO 5
#define YFO 6
#define ZFO 7
#define PFO 8
#define PUP 9
#define FOV 10
#define CAX 28
#define CAY 29
#define CAZ 30
#define CUS 11
#define AZI 12
#define ALT 13
#define APR 14
#define FDI 15
#define UPD 16
#define ObjectListGroup 17
#define OLB 18
#define ADD 19
#define DEL 20
#define MaterialEditor 21
#define ALP 22
#define DIF 23
#define IOR 24
#define EMI 25
#define CBL 26
#define AMC 40
#define PositionEditor 35
#define OBX 31
#define OBY 32
#define OBZ 33
#define OBU 34
#define SCA 36
#define PRE 37
#define REN 38
#define BHV 39
#define PRD 41
#define PAU 42
using namespace std;
HWND blockhandle = NULL;
HWND previeww = NULL;
LRESULT CALLBACK MessageHandler(HWND WinHandle, UINT message, WPARAM wparam, LPARAM lparam)
{
    cout<<(int)message<<endl;
    switch(message)
    {
    case WM_CTLCOLORSTATIC:
    {
        if(GetDlgCtrlID((HWND)lparam)==17)
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
                       5,5,290,185,
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


        MakeButton("+FO",BS_AUTOCHECKBOX|WS_GROUP|WS_CHILD|WS_VISIBLE|WS_TABSTOP,10,70,100,20,PFO,WinHandle,0);
        MakeButton("+UP",BS_AUTOCHECKBOX|WS_GROUP|WS_CHILD|WS_VISIBLE|WS_TABSTOP,110,70,100,20,PUP,WinHandle,0);
        MakeButton("0.7",WS_CHILD|WS_VISIBLE|WS_TABSTOP|ES_AUTOHSCROLL,260,70,30,20,FOV,WinHandle,0,"EDIT",WS_EX_CLIENTEDGE|WS_EX_WINDOWEDGE);
        CreateWindow("STATIC","FOV:",WS_VISIBLE|WS_CHILD,210,70,40,20,WinHandle,NULL,GetModuleHandle(NULL),NULL);


        MakeButton("-6",WS_CHILD|WS_VISIBLE|WS_TABSTOP|ES_AUTOHSCROLL, 60,90,30,20,CAX,WinHandle,0,"EDIT",WS_EX_CLIENTEDGE|WS_EX_WINDOWEDGE);
        MakeButton("0",WS_CHILD|WS_VISIBLE|WS_TABSTOP|ES_AUTOHSCROLL,160,90,30,20,CAY,WinHandle,0,"EDIT",WS_EX_CLIENTEDGE|WS_EX_WINDOWEDGE);
        MakeButton("2",WS_CHILD|WS_VISIBLE|WS_TABSTOP|ES_AUTOHSCROLL,260,90,30,20,CAZ,WinHandle,0,"EDIT",WS_EX_CLIENTEDGE|WS_EX_WINDOWEDGE);
        CreateWindow("STATIC","X:",WS_VISIBLE|WS_CHILD, 10,90,40,20,WinHandle,NULL,GetModuleHandle(NULL),NULL);
        CreateWindow("STATIC","Y:",WS_VISIBLE|WS_CHILD,110,90,40,20,WinHandle,NULL,GetModuleHandle(NULL),NULL);
        CreateWindow("STATIC","Z:",WS_VISIBLE|WS_CHILD,210,90,40,20,WinHandle,NULL,GetModuleHandle(NULL),NULL);

        MakeButton("Custom:",BS_AUTOCHECKBOX|WS_GROUP|WS_CHILD|WS_VISIBLE|WS_TABSTOP,10,110,100,20,CUS,WinHandle,0);
        CheckDlgButton(WinHandle,CUS,BST_CHECKED);
        MakeButton("180",WS_CHILD|WS_VISIBLE|WS_TABSTOP|ES_AUTOHSCROLL,160,110,30,20,AZI,WinHandle,0,"EDIT",WS_EX_CLIENTEDGE|WS_EX_WINDOWEDGE);
        MakeButton("0",WS_CHILD|WS_VISIBLE|WS_TABSTOP|ES_AUTOHSCROLL,260,110,30,20,ALT,WinHandle,0,"EDIT",WS_EX_CLIENTEDGE|WS_EX_WINDOWEDGE);
        CreateWindow("STATIC","AZI:",WS_VISIBLE|WS_CHILD,110,110,30,20,WinHandle,NULL,GetModuleHandle(NULL),NULL);
        CreateWindow("STATIC","ALT:",WS_VISIBLE|WS_CHILD,210,110,40,20,WinHandle,NULL,GetModuleHandle(NULL),NULL);

        MakeButton("0.01",WS_CHILD|WS_VISIBLE|WS_TABSTOP|ES_AUTOHSCROLL,110,130,50,20,APR,WinHandle,0,"EDIT",WS_EX_CLIENTEDGE|WS_EX_WINDOWEDGE);
        MakeButton("6",WS_CHILD|WS_VISIBLE|WS_TABSTOP|ES_AUTOHSCROLL,240,130,50,20,FDI,WinHandle,0,"EDIT",WS_EX_CLIENTEDGE|WS_EX_WINDOWEDGE);
        CreateWindow("STATIC","Aperture",WS_VISIBLE|WS_CHILD,10,130,100,20,WinHandle,NULL,GetModuleHandle(NULL),NULL);
        CreateWindow("STATIC","Focus",WS_VISIBLE|WS_CHILD,160,130,70,20,WinHandle,NULL,GetModuleHandle(NULL),NULL);

        MakeButton("Update",BS_DEFPUSHBUTTON|WS_GROUP|WS_CHILD|WS_VISIBLE,10,155,280,30,UPD,WinHandle,0);

        CreateWindowEx(WS_EX_WINDOWEDGE,
                       "BUTTON",
                       "Objects",
                       BS_GROUPBOX|WS_CHILD|WS_VISIBLE,
                       5,190,290,145,
                       WinHandle,
                       (HMENU)ObjectListGroup,
                       GetModuleHandle(NULL),
                       0
                      );


        CreateWindowEx(WS_EX_CLIENTEDGE|WS_EX_WINDOWEDGE,"listbox","",WS_CHILD|WS_VISIBLE|WS_VSCROLL|ES_AUTOVSCROLL,10,215,280,100,WinHandle,(HMENU)OLB,GetModuleHandle(NULL),0);


        MakeButton("Add",BS_DEFPUSHBUTTON|WS_GROUP|WS_CHILD|WS_VISIBLE,10,304,135,25,ADD,WinHandle,0);
        MakeButton("Delete",BS_DEFPUSHBUTTON|WS_GROUP|WS_CHILD|WS_VISIBLE,150,304,135,25,DEL,WinHandle,0);

        CreateWindowEx(WS_EX_WINDOWEDGE,
                       "BUTTON",
                       "Material",
                       BS_GROUPBOX|WS_CHILD|WS_VISIBLE,
                       5,335,140,227,
                       WinHandle,
                       (HMENU)MaterialEditor,
                       GetModuleHandle(NULL),
                       0
                      );

        MakeButton("",WS_CHILD|WS_VISIBLE|WS_TABSTOP|ES_AUTOHSCROLL,95,360,40,20,ALP,WinHandle,0,"EDIT",WS_EX_CLIENTEDGE|WS_EX_WINDOWEDGE);
        MakeButton("1.0",WS_CHILD|WS_VISIBLE|WS_TABSTOP|ES_AUTOHSCROLL,95,380,40,20,DIF,WinHandle,0,"EDIT",WS_EX_CLIENTEDGE|WS_EX_WINDOWEDGE);
        MakeButton("",WS_CHILD|WS_VISIBLE|WS_TABSTOP|ES_AUTOHSCROLL,95,400,40,20,IOR,WinHandle,0,"EDIT",WS_EX_CLIENTEDGE|WS_EX_WINDOWEDGE);
        MakeButton("",WS_CHILD|WS_VISIBLE|WS_TABSTOP|ES_AUTOHSCROLL,95,420,40,20,EMI,WinHandle,0,"EDIT",WS_EX_CLIENTEDGE|WS_EX_WINDOWEDGE);
        CreateWindow("STATIC","Alpha:",WS_VISIBLE|WS_CHILD,10,360,75,20,WinHandle,NULL,GetModuleHandle(NULL),NULL);
        CreateWindow("STATIC","Diffusion:",WS_VISIBLE|WS_CHILD,10,380,75,20,WinHandle,NULL,GetModuleHandle(NULL),NULL);
        CreateWindow("STATIC","IOR:",WS_VISIBLE|WS_CHILD,10,400,75,20,WinHandle,NULL,GetModuleHandle(NULL),NULL);
        CreateWindow("STATIC","Intensity:",WS_VISIBLE|WS_CHILD,10,420,75,20,WinHandle,NULL,GetModuleHandle(NULL),NULL);
        CreateWindow("STATIC","Color:",WS_VISIBLE|WS_CHILD,10,440,75,20,WinHandle,NULL,GetModuleHandle(NULL),NULL);
        CreateWindow("STATIC","Ambient",WS_VISIBLE|WS_CHILD,10,460,75,20,WinHandle,NULL,GetModuleHandle(NULL),NULL);


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
        CreateWindowEx( WS_EX_CLIENTEDGE,ColorBlock.lpszClassName, "Pathtracer",
                  WS_VISIBLE | WS_CHILD,
                  95, 440, 40, 20, WinHandle, (HMENU) CBL, GetModuleHandle(NULL), &blockhandle);
        CreateWindowEx( WS_EX_CLIENTEDGE,ColorBlock.lpszClassName, "Pathtracer",
                  WS_VISIBLE | WS_CHILD,
                  95, 460, 40, 20, WinHandle, (HMENU) AMC, GetModuleHandle(NULL), &blockhandle);



        CreateWindowEx(WS_EX_WINDOWEDGE,
                       "BUTTON",
                       "Position",
                       BS_GROUPBOX|WS_CHILD|WS_VISIBLE,
                       150,335,140,227,
                       WinHandle,
                       (HMENU)PositionEditor,
                       GetModuleHandle(NULL),
                       0
                      );


        CreateWindow("STATIC","X:",WS_VISIBLE|WS_CHILD,170,360,75,20,WinHandle,NULL,GetModuleHandle(NULL),NULL);
        CreateWindow("STATIC","Y:",WS_VISIBLE|WS_CHILD,170,380,75,20,WinHandle,NULL,GetModuleHandle(NULL),NULL);
        CreateWindow("STATIC","Z:",WS_VISIBLE|WS_CHILD,170,400,75,20,WinHandle,NULL,GetModuleHandle(NULL),NULL);
        MakeButton("",WS_CHILD|WS_VISIBLE|WS_TABSTOP|ES_AUTOHSCROLL,235,360,50,20,OBX,WinHandle,0,"EDIT",WS_EX_CLIENTEDGE|WS_EX_WINDOWEDGE);
        MakeButton("",WS_CHILD|WS_VISIBLE|WS_TABSTOP|ES_AUTOHSCROLL,235,380,50,20,OBY,WinHandle,0,"EDIT",WS_EX_CLIENTEDGE|WS_EX_WINDOWEDGE);
        MakeButton("",WS_CHILD|WS_VISIBLE|WS_TABSTOP|ES_AUTOHSCROLL,235,400,50,20,OBZ,WinHandle,0,"EDIT",WS_EX_CLIENTEDGE|WS_EX_WINDOWEDGE);
        CreateWindow("STATIC","Scale:",WS_VISIBLE|WS_CHILD,170,420,75,20,WinHandle,NULL,GetModuleHandle(NULL),NULL);
        MakeButton("",WS_CHILD|WS_VISIBLE|WS_TABSTOP|ES_AUTOHSCROLL,235,420,50,20,SCA,WinHandle,0,"EDIT",WS_EX_CLIENTEDGE|WS_EX_WINDOWEDGE);

        MakeButton("Update",BS_DEFPUSHBUTTON|WS_GROUP|WS_CHILD|WS_VISIBLE,155,440,130,40,OBU,WinHandle,0);

        MakeButton("Generate BVH",BS_PUSHBUTTON|WS_GROUP|WS_CHILD|WS_VISIBLE,305,30,280,40,BHV,WinHandle,0);
        MakeButton("Preview",BS_PUSHBUTTON|WS_GROUP|WS_CHILD|WS_VISIBLE,305,70,280,40,PRE,WinHandle,0);
        MakeButton("Render",BS_PUSHBUTTON|WS_GROUP|WS_CHILD|WS_VISIBLE,305,110,280,40,REN,WinHandle,0);
        MakeButton("Update",BS_PUSHBUTTON|WS_GROUP|WS_CHILD|WS_VISIBLE,305,150,280,40,PRD,WinHandle,0);
        MakeButton("Pause",BS_PUSHBUTTON|WS_GROUP|WS_CHILD|WS_VISIBLE,305,190,280,40,PAU,WinHandle,0);
        RegisterLiveClass();



    }
    break;
    case WM_COMMAND:
        {
            switch(LOWORD(wparam))
            {
            case UPD:
                {

                    char buffer[15] = {0};
                    //if(guiCamera == 0)guiCamera = new Camera;
                    if(IsDlgButtonChecked(WinHandle,CUS))
                    {
                        GetWindowText(GetDlgItem(WinHandle,AZI),buffer,15);
                        float Azimuth = parsefrfloat(string(buffer));
                        GetWindowText(GetDlgItem(WinHandle,ALT),buffer,15);
                        float Attitude = parsefrfloat(string(buffer));
                        float cosAz = cosf(Azimuth*0.0174532925);
                        float cosAt = cosf(Attitude*0.0174532925);
                        float sinAz = sinf(Azimuth*0.0174532925);
                        float sinAt = sinf(Attitude*0.0174532925);
                        guiCamera.mx = d3Vector(-cosAz*cosAt,sinAz,sinAt).normalize();
                        guiCamera.my = d3Vector(-sinAz,-cosAz*cosAt,0).normalize();
                        guiCamera.mz = d3Vector(guiCamera.my.crossproduct(guiCamera.mx)).normalize();

                    }else
                    {
                        guiCamera.mx = d3Vector(IsDlgButtonChecked(WinHandle,XFO)?1:0,IsDlgButtonChecked(WinHandle,YFO)?1:0,IsDlgButtonChecked(WinHandle,ZFO)?1:0).normalize().scalarmultiply(IsDlgButtonChecked(WinHandle,PFO)?1:-1);
                        guiCamera.mz = d3Vector(IsDlgButtonChecked(WinHandle,XUP)?1:0,IsDlgButtonChecked(WinHandle,YUP)?1:0,IsDlgButtonChecked(WinHandle,ZUP)?1:0).normalize().scalarmultiply(IsDlgButtonChecked(WinHandle,PUP)?1:-1);
                        guiCamera.my = (guiCamera.mx.crossproduct(guiCamera.mz)).normalize();
                    }
                        GetWindowText(GetDlgItem(WinHandle,CAX),buffer,15);
                        guiCamera.start = d3Vector(parsefrfloat(string(buffer)));
                        GetWindowText(GetDlgItem(WinHandle,CAY),buffer,15);
                        guiCamera.start.y = parsefrfloat(string(buffer));
                        GetWindowText(GetDlgItem(WinHandle,CAZ),buffer,15);
                        guiCamera.start.z = parsefrfloat(string(buffer));
                        GetWindowText(GetDlgItem(WinHandle,FDI),buffer,15);
                        guiCamera.fdistance = parsefrfloat(string(buffer));
                        GetWindowText(GetDlgItem(WinHandle,APR),buffer,15);
                        guiCamera.recipjitter = 400.0/parsefrfloat(string(buffer));
                        GetWindowText(GetDlgItem(WinHandle,FOV),buffer,15);
                        guiCamera.flength = parsefrfloat(string(buffer));
                        Camera cadcADS = guiCamera;

                        int a;
                        int b;
                        a = a+b;
                    break;
                }
            case ADD:
                {
                    OPENFILENAMEA ofn = {0};
                    char buffer[250] = {0};
                    char buffr[250] = {0};
                    ofn.lStructSize = sizeof(ofn);
                    ofn.hwndOwner = WinHandle;
                    ofn.lpstrFile = buffer;
                    ofn.lpstrFilter = "Wavefront OBJ\0*.obj*\0\0";
                    ofn.nFilterIndex = 1;
                    ofn.nMaxFile = 250;
                    ofn.Flags = OFN_FILEMUSTEXIST;
                    GetOpenFileName(&ofn);
                    //MessageBoxA(WinHandle,buffer,"dsafasd",MB_ICONERROR);
                    Material m(0,0,0,0,0,0);
                    GetWindowText(GetDlgItem(WinHandle,ALP),buffr,15);
                    m.alpha = parsefrfloat(string(buffr));
                    GetWindowText(GetDlgItem(WinHandle,DIF),buffr,15);
                    m.diffuseglossy = parsefrfloat(string(buffr));
                    GetWindowText(GetDlgItem(WinHandle,IOR),buffr,15);
                    m.ior = parsefrfloat(string(buffr));
                    GetWindowText(GetDlgItem(WinHandle,EMI),buffr,15);
                    m.emission = parsefrfloat(string(buffr));
                    COLORREF surfacecolor = ((ColorBlockData*)GetWindowLong(GetDlgItem(WinHandle,CBL),0))->currentcolor;
                    m.r = GetRValue(surfacecolor);
                    m.g = GetGValue(surfacecolor);
                    m.b = GetBValue(surfacecolor);
                    guiScene = generateScene(buffer,m);
                    surfacecolor = ((ColorBlockData*)GetWindowLong(GetDlgItem(WinHandle,AMC),0))->currentcolor;
                    guiScene.ambientcolor = d3Vector(GetRValue(surfacecolor),GetGValue(surfacecolor),GetBValue(surfacecolor));
                    guiBvh = BVH(guiScene);
                    break;
                }
            case BHV:
                {
                    guiBvh = BVH(guiScene);
                    break;
                }
            case PRE:
                {
                    renderable* r = new renderable;
                    r->inBVH = guiBvh;
                    r->precam = guiCamera;
                    r->inBVH.spherepointer = new Sphere(10,0,0,15,Material(1,1,1,255,255,255,5));
                    r->inBVH.spherepointer->nextsphere = 0;
                    previeww = CreateWindowEx(WS_EX_CLIENTEDGE|WS_EX_OVERLAPPEDWINDOW|WS_EX_APPWINDOW|WS_EX_TOPMOST,"LivePreview","Preview",WS_VISIBLE|WS_DLGFRAME|WS_SYSMENU|WS_MINIMIZEBOX,CW_USEDEFAULT,CW_USEDEFAULT,510,539,WinHandle,NULL,GetModuleHandle(NULL),r);
                    break;
                }
            case OBU:
                {
                    char buffer[15] = {0};
                    Triangle* trptr = guiScene.trianglepointer;
                    d3Vector o;
                    float s;
                    GetWindowText(GetDlgItem(WinHandle,OBX),buffer,15);
                    o.x = parsefrfloat(string(buffer));
                    GetWindowText(GetDlgItem(WinHandle,OBY),buffer,15);
                    o.y = parsefrfloat(string(buffer));
                    GetWindowText(GetDlgItem(WinHandle,OBZ),buffer,15);
                    o.z = parsefrfloat(string(buffer));
                    GetWindowText(GetDlgItem(WinHandle,SCA),buffer,15);
                    s = parsefrfloat(string(buffer));
                    d3Vector ca,cb,cc;
                    while(trptr->notnullflag)
                    {
                        ca = (trptr->a.scalarmultiply(s)).add(o);
                        cb = (trptr->b.scalarmultiply(s)).add(o);
                        cc = (trptr->c.scalarmultiply(s)).add(o);
                        *trptr = Triangle(ca,cb,cc,trptr->material);
                        trptr++;
                    }
                    //int pnum = guiBvh.passnumber;
                    guiBvh = BVH(guiScene);
                    //guiBvh.passnumber = pnum;
                    /*renderable* r = new renderable;
                    r->inBVH = BVH(guiScene);
                    r->precam = guiCamera;
                    r->inBVH.passnumber = 0;
                    r->inBVH.spherepointer = new Sphere(10,0,0,15,Material(1,1,1,255,255,255,5));
                    r->inBVH.spherepointer->nextsphere = 0;
                    SendMessage(previeww,UPDATEMSG,0,(LPARAM)r);
                    */
                    SendMessageA(WinHandle,WM_COMMAND,UPD,0);
                    renderable* r = new renderable;
                    r->inBVH = guiBvh;
                    r->precam = guiCamera;
                    r->inBVH.passnumber = 0;
                    r->inBVH.spherepointer = new Sphere(10,0,0,15,Material(1,1,1,255,255,255,5));
                    r->inBVH.spherepointer->nextsphere = 0;
                    SendMessage(previeww,UPDATEMSG,0,(LPARAM)r);
                    break;
                }
            case PAU:
                {
                    SendMessage(previeww,PAUSEMSG,0,0);
                }
                break;
            case PRD:
                {
                    SendMessageA(WinHandle,WM_COMMAND,UPD,0);
                    renderable* r = new renderable;
                    r->inBVH = guiBvh;
                    r->precam = guiCamera;
                    r->inBVH.passnumber = 0;
                    r->inBVH.spherepointer = new Sphere(10,0,0,15,Material(1,1,1,255,255,255,5));
                    r->inBVH.spherepointer->nextsphere = 0;
                    SendMessage(previeww,UPDATEMSG,0,(LPARAM)r);
                    break;
                }
            case CAX:
            case AZI:
            case ALT:
            case CAY:
            case CAZ:
            case FDI:
            case APR:
            case FOV:
                {
                    if(HIWORD(wparam)==EN_CHANGE)
                    {
                        SendMessageA(WinHandle,WM_COMMAND,PRD,0);
                    }
                    break;
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
