#pragma once
#include <Windows.h>  // подключение библиотеки Windows
#include "Border.h"
#include "Level.h"
#include "Platform.h"

const int Timer_ID = WM_USER + 1;

enum EKey_Type
{
    EKT_Left,
    EKT_Right,
    EKT_Space
};

enum EGame_State
{
    EGS_Test_Ball,

    EGS_Play_Level,
    EGS_Lost_Ball,
    EGS_Restart_Level
};

// собственные классы будем писать с буквой A (префикс s - означает single class)
class AsEngine
{
public:
    AsEngine(); // конструктор

    void Init_Engine(HWND hwnd);
    void Draw_Frame(HDC hdc, RECT& paint_area);
    int On_Key_Down(EKey_Type key_type);
    int On_Timer();
    
private:
    EGame_State Game_State;

    ABall Ball;
    ALevel Level;
    AsPlatform Platform;
    AsBorder Border;
};