# 麦轮机器人代码
**本工程同时为该机器人云台主控和底盘主控，由app层robot_def.h中宏定义进行切换**
**烧录前务必保证该宏定义与目标主控对应，不能两个同时解注释，烧录错误代码造成的后果未经过评估**

## 仓库说明
本机器人代码为基于2022Framework_F407的步兵机器人，主体由C++开发，C/C++混合编译

## 待办
功率控制，斜坡函数，裁判系统

## 说明
main函数位于 Core/Src/main.c

前置学习要求：
- 电控组视频，学习stm32相关知识。
- C++与面向对象基础，大量使用C++11相关内容
- 电控新框架相关知识，在ones和git上

**框架相关资料可见 Tutorials 文件夹**

本代码中vscode代码格式统一为：

{ BasedOnStyle: Google, IndentWidth: 4, TabWidth: 4, ColumnLimit: 0 }
