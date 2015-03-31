#ifndef COLORPICKER_H_INCLUDED
#define COLORPICKER_H_INCLUDED

void SetBlockColor(HWND block,COLORREF c);
LRESULT CALLBACK ColorBlockProc(HWND WinHandle, UINT message, WPARAM wparam, LPARAM lparam);
void SetBlockColor(HWND parent,HMENU objindex,COLORREF c);
LRESULT CALLBACK ColorPickerProc(HWND WinHandle, UINT message, WPARAM wparam, LPARAM lparam);

typedef struct
{
    COLORREF currentcolor;
    HWND* handlevar = NULL;
} ColorBlockData;

#endif // COLORPICKER_H_INCLUDED
