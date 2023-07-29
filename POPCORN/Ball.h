﻿#pragma once
#include "Config.h"

enum EBall_State
{
    EBS_Normal,
    EBS_Lost,
    EBS_On_Platform
};

struct test
{
    int test = 1;
    double d = 3.1415;
};

class ABall;
class AHit_Checker
{
public:
    virtual bool Check_Hit(double next_x_pos, double next_y_pos, ABall* ball) = 0;

    bool Hit_Circle_On_Line(double y, double next_x_pos, double left_x, double right_x, double radius, double& x);
};

class ABall
{
public:
    ABall(); // конструктор

    void Init();
    void Draw(HDC hdc, RECT& paint_area);
    void Move();
    void Set_For_Test();
    bool Is_Test_Finished();
    EBall_State Get_State();
    void Set_State(EBall_State new_state, double x_pos, double y_pos = Start_Ball_Y_Pos);
    double Get_Direction();
    void Set_Direction(double new_direction);
    void Reflect(bool from_horizontal);
    bool Is_Moving_Up();
    bool Is_Moving_Left();

    double Ball_Speed;

    static void Add_Hit_Checker(AHit_Checker* hit_checker);

    static const double Radius;

private:
    void Redraw_Ball();

    EBall_State Ball_State;
    HPEN Ball_Pen;
    HBRUSH Ball_Brush;
    
    double Rest_Distance;
    double Ball_Direction;

    bool Testing_Is_Active;
    int Test_Iteration;
    double Rest_Test_Distance;

    double Center_X_Pos, Center_Y_Pos;

    RECT Ball_Rect, Prev_Ball_Rect;

    static const double Start_Ball_Y_Pos;
    static int Hit_Checkers_Count;
    static AHit_Checker* Hit_Checkers[3];
};

