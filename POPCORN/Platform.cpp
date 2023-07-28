﻿#include "Platform.h"


// инициализируем переменные в конструкторе
AsPlatform::AsPlatform() : Inner_Width(Normal_Platform_Inner_Width), X_Pos(AsConfig::Border_X_Offset), X_Step(AsConfig::Global_Scale * 2), Platform_State(EPS_Normal),
            Width(Normal_Width), Rolling_Step(0), Highlight_Pen(0), Platform_Circle_Pen(0), Platform_Inner_Pen(0), Platform_Circle_Brush(0),
            Platform_Inner_Brush(0), Platform_Rect{}, Prev_Platform_Rect{}
{
    X_Pos = (AsConfig::Max_X_Pos - Width) / 2;
}

bool AsPlatform::Check_Hit(double next_x_pos, double next_y_pos, ABall* ball)
{
    if (next_y_pos + ball->Radius > AsConfig::Platform_Y_Pos)
    {
        if (next_x_pos + ball->Radius >= X_Pos && next_x_pos - ball->Radius <= (double)(X_Pos + Width))
        {
            ball->Reflect(true);
                return true;
        }
    }
    return false;
}

void AsPlatform::Init()
{
    Highlight_Pen = CreatePen(PS_SOLID, 0, RGB(255, 255, 255));
    AsConfig::Create_Pen_Brush(230, 12, 12, Platform_Circle_Pen, Platform_Circle_Brush);
    AsConfig::Create_Pen_Brush(240, 155, 89, Platform_Inner_Pen, Platform_Inner_Brush);
}

void AsPlatform::Act()
{
    switch (Platform_State)
    {
    case EPS_Meltdown:
    case EPS_Roll_In:
    case EPS_Expand_Roll_In:
         Redraw_Platform();
    }
}

EPlatform_State AsPlatform::Get_State()
{
   return Platform_State;
}

void AsPlatform::Set_State(EPlatform_State new_state)
{
    int len;

    if (Platform_State == new_state)
        return;

    switch (new_state)
    {
    case EPS_Meltdown:
        len = sizeof(Meltdown_Platform_Y_Pos) / sizeof(Meltdown_Platform_Y_Pos[0]);

        for (int i = 0; i < len; i++)
        {
            Meltdown_Platform_Y_Pos[i] = Platform_Rect.bottom;
        }
        break;

    case EPS_Roll_In:
        X_Pos = AsConfig::Max_X_Pos - 1;
        Rolling_Step = Max_Rolling_Step - 1;
        break;
    }

    Platform_State = new_state;
}

void AsPlatform::Redraw_Platform()
{
    int platform_width;

    Prev_Platform_Rect = Platform_Rect;

    if (Platform_State == EPS_Roll_In)
        platform_width = Circle_Size;
    else
        platform_width = Width;

        Platform_Rect.left = X_Pos * AsConfig::Global_Scale;
        Platform_Rect.top = AsConfig::Platform_Y_Pos * AsConfig::Global_Scale;
        Platform_Rect.right = Platform_Rect.left + platform_width * AsConfig::Global_Scale;
        Platform_Rect.bottom = Platform_Rect.top + Height * AsConfig::Global_Scale;
    
    if (Platform_State == EPS_Meltdown)
        Prev_Platform_Rect.bottom = (AsConfig::Max_Y_Pos + 1) * AsConfig::Global_Scale;

    InvalidateRect(AsConfig::Hwnd, &Prev_Platform_Rect, FALSE);
    InvalidateRect(AsConfig::Hwnd, &Platform_Rect, FALSE);
}

// выбор какую платформу рисовать
void AsPlatform::Draw(HDC hdc, RECT& paint_area)
{
    RECT intersection_rect;

    if (!IntersectRect(&intersection_rect, &paint_area, &Platform_Rect))
        return;

    switch (Platform_State)
    { 
    case EPS_Normal:
        Draw_Normal_State(hdc, paint_area);
        break;
    case EPS_Meltdown:
        Draw_Meltdown_State(hdc, paint_area);
        break;
    case EPS_Roll_In:
        Draw_Roll_In_State(hdc, paint_area);
        break;
    case EPS_Expand_Roll_In:
        Draw_Expanding_Roll_In_State(hdc, paint_area);
        break;
    }
}

// очищаем фоном прежнее место
void AsPlatform::Clear_BG(HDC hdc)
{
    SelectObject(hdc, AsConfig::BG_Pen);
    SelectObject(hdc, AsConfig::BG_Brush);
    Rectangle(hdc, Prev_Platform_Rect.left, Prev_Platform_Rect.top, Prev_Platform_Rect.right, Prev_Platform_Rect.bottom);
}

// рисуем блик на шарике
void AsPlatform::Draw_Circle_Highlight(HDC hdc, int x, int y)
{
    SelectObject(hdc, Highlight_Pen);

    Arc(hdc, x + AsConfig::Global_Scale, y + AsConfig::Global_Scale, x + (Circle_Size - 1) * AsConfig::Global_Scale,
        y + (Circle_Size - 1) * AsConfig::Global_Scale, x + 2 * AsConfig::Global_Scale, y + AsConfig::Global_Scale,
        x + AsConfig::Global_Scale, y + 3 * AsConfig::Global_Scale);
}


// рисуем платформу в нормальном состоянии
void AsPlatform::Draw_Normal_State(HDC hdc, RECT& paint_area)
{
    int x = X_Pos;
    int y = AsConfig::Platform_Y_Pos;
    RECT intersection_rect;

    Clear_BG(hdc);

    // рисуем боковые шарики
    SelectObject(hdc, Platform_Circle_Pen);
    SelectObject(hdc, Platform_Circle_Brush);
    Ellipse(hdc, x * AsConfig::Global_Scale, y * AsConfig::Global_Scale, (x + Circle_Size) * AsConfig::Global_Scale, (y + Circle_Size) * AsConfig::Global_Scale);
    Ellipse(hdc, (x + Inner_Width) * AsConfig::Global_Scale, y * AsConfig::Global_Scale,
                 (x + Circle_Size + Inner_Width) * AsConfig::Global_Scale, (y + Circle_Size) * AsConfig::Global_Scale);

    // рисуем блик на шарике
    Draw_Circle_Highlight(hdc, x * AsConfig::Global_Scale, y * AsConfig::Global_Scale);

    // рисуем среднюю часть платформы
    SelectObject(hdc, Platform_Inner_Pen);
    SelectObject(hdc, Platform_Inner_Brush);
    RoundRect(hdc, (x + 4) * AsConfig::Global_Scale, (y + 1) * AsConfig::Global_Scale, (x + 4 + Inner_Width - 1) * AsConfig::Global_Scale,
                   (y + 1 + 5) * AsConfig::Global_Scale, 4 * AsConfig::Global_Scale, 4 * AsConfig::Global_Scale);
}


// рисуем платформу в расплавленном состоянии
void AsPlatform::Draw_Meltdown_State(HDC hdc, RECT& paint_area)
{
    int x, y, y_offset;
    int area_width, area_height, max_platform_y;
    int moved_columns_count = 0;
    COLORREF pixel;
    COLORREF bg_pixel = RGB(AsConfig::BG_Color.R, AsConfig::BG_Color.G, AsConfig::BG_Color.B);
    RECT intersection_rect;

    area_width = Width * AsConfig::Global_Scale;
    area_height = Height * AsConfig::Global_Scale + 1;
    max_platform_y = AsConfig::Max_Y_Pos * AsConfig::Global_Scale + area_height;

    for (int i = 0; i < area_width; i++)
    {
        if (Meltdown_Platform_Y_Pos[i] > max_platform_y)
            continue;

            ++moved_columns_count;

        y_offset = AsConfig::Rand(Meltdown_Speed) + 1;
        x = Platform_Rect.left + i;

        for (int j = 0; j < area_height; j++)
        {
            y = Meltdown_Platform_Y_Pos[i] - j;
            pixel = GetPixel(hdc, x, y);
            SetPixel(hdc, x, y + y_offset, pixel);
        }

        for (int j = 0; j < y_offset; j++)
        {
            y = Meltdown_Platform_Y_Pos[i] - area_height + 1 + j;
            SetPixel(hdc, x, y, bg_pixel);
        }

        Meltdown_Platform_Y_Pos[i] += y_offset;
    }

    if (moved_columns_count == 0)
        Platform_State = EPS_Missing; // вся платформа сдвинулась за пределы окна
}


// рисуем выкатывающуюся платформу
void AsPlatform::Draw_Roll_In_State(HDC hdc, RECT& paint_area)
{
    int x = X_Pos * AsConfig::Global_Scale;
    int y = AsConfig::Platform_Y_Pos * AsConfig::Global_Scale;
    int roller_size = Circle_Size * AsConfig::Global_Scale;
    double alpha;
    XFORM xform, old_xform;

    Clear_BG(hdc);

    // 1. Шарик
    SelectObject(hdc, Platform_Circle_Pen);
    SelectObject(hdc, Platform_Circle_Brush);
    Ellipse(hdc, x, y, x + roller_size, y + roller_size);

    // 2. Разделительная линия
    SetGraphicsMode(hdc, GM_ADVANCED);
    alpha = -2.0 * M_PI / (double)Max_Rolling_Step * (double)Rolling_Step;

    xform.eM11 = (float)cos(alpha);
    xform.eM12 = (float)sin(alpha);
    xform.eM21 = (float)-sin(alpha);
    xform.eM22 = (float)cos(alpha);
    xform.eDx = (float)(x + roller_size) / 2;
    xform.eDy = (float)(y + roller_size) / 2;
    GetWorldTransform(hdc, &old_xform);
    SetWorldTransform(hdc, &xform);

    SelectObject(hdc, AsConfig::BG_Pen);
    SelectObject(hdc, AsConfig::BG_Brush);
    Rectangle(hdc, -AsConfig::Global_Scale / 2, -roller_size / 2, AsConfig::Global_Scale / 2, roller_size / 2);
  
    SetWorldTransform(hdc, &old_xform);

    // 3. Блик
    Draw_Circle_Highlight(hdc, x, y);

    ++Rolling_Step;

    if (Rolling_Step >= Max_Rolling_Step)
        Rolling_Step -= Max_Rolling_Step;

    X_Pos -= Rolling_Platform_Speed;

    if (X_Pos <= Roll_In_Platform_End_X_Pos)
    {
        X_Pos += Rolling_Platform_Speed;
        Platform_State = EPS_Expand_Roll_In;
        Inner_Width = 1;
    }
}


// рисуем расширяющуюся после выкатывания платформу
void AsPlatform::Draw_Expanding_Roll_In_State(HDC hdc, RECT& paint_area)
{
    Draw_Normal_State(hdc, paint_area);
    --X_Pos;
    Inner_Width += 2;

    if (Inner_Width >= Normal_Platform_Inner_Width)
    {
        Inner_Width = Normal_Platform_Inner_Width;
        Platform_State = EPS_Ready;
        Redraw_Platform();
    } 
}


