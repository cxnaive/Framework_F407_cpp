#ifndef _ROBOT_DEF_H_
#define _ROBOT_DEF_H_

// 该文件规定了模块和板间通信的具体形式

// 双板方案，定义主控类型，上板或下板
// 按照要烧录的主控类型 **必须**定义且仅定义一个 另一个注释
#define GIMBAL_BOARD
// #define CHASSIS_BOARD

#include <imu_data.h>

#include "stdint.h"
#pragma pack(1)
// 对模块是否掉线的定义
enum Module_status {
    module_lost = 0,
    module_run
};

// 机器人总模式
enum Robot_mode {
    robot_stop = 0,
    robot_run
};

// 底盘运行模式
enum Chassis_mode {
    chassis_stop = 0,
    chassis_run,               //底盘云台分离模式
    chassis_rotate_run,        //小陀螺模式
    chassis_run_follow_offset  //底盘跟随云台模式
};

//拨弹轮运行模式
enum Bullet_mode {
    bullet_stop = 0,
    bullet_reverse,    //反转，卡弹处理
    bullet_single,     //单发
    bullet_double,     //双发
    bullet_trible,     //三发
    bullet_continuous  //连发
};

//摩擦轮运行模式
enum Friction_mode {
    friction_stop = 0,  // 关闭摩擦轮
    friction_run,       // 开启摩擦轮
};

//弹仓盖模式
enum Magazine_mode {
    magazine_close = 0,  //开弹仓
    magazine_open        //关弹仓
};

//云台模式
enum Gimbal_mode {
    gimbal_stop = 0,
    gimbal_run,     //正常云台模式
    gimbal_middle,  //云台归中，跟随底盘模式
};

//自瞄模式
enum AutoAim_mode {
    auto_aim_off = 0,     //关闭自瞄
    auto_aim_normal,      //正常自瞄
    auto_aim_buff_small,  //小能量机关
    auto_aim_buff_big     //大能量机关
};

// 机器人模块控制量定义
// 对底盘速度的控制量
struct cmd_chassis_speed {
    float vx;            // 单位 mm/s
    float vy;            // 单位 mm/s
    float rotate;        // 单位 度每秒
    float offset_angle;  // vy与底盘正前方的夹角，范围-180 +180度，方向顺时针，即底盘y方向旋转该角度到达vy方向
};

// 对底盘功率的控制量
struct cmd_chassis_power {
    uint8_t power_limit;    //裁判系统功率限制
    uint16_t power_buffer;  //裁判系统当前缓冲功率
};

// 对底盘的控制量
struct cmd_chassis {
    Chassis_mode mode;
    cmd_chassis_speed speed;
    cmd_chassis_power power;
};

// 对发射机构的控制量
struct cmd_shoot {
    Friction_mode friction_mode;
    Bullet_mode bullet_mode;
    Magazine_mode mag_mode;
    uint16_t bullet_speed;       //弹速 m/s
    uint16_t heat_limit_remain;  //剩余热量
    float fire_rate;             //射频（发/秒）
};

// 对云台的控制量
struct cmd_gimbal {
    Gimbal_mode gimbal_mode;
    float yaw;
    float pitch;

    float rotate_feedforward;  //小陀螺前馈
};

// 云台回传cmd的数据
struct upload_gimbal {
    Module_status gimbal_status;
    imu_data* gimbal_imu;
    short* yaw_encoder;
};

//底盘模块回传cmd的数据
struct upload_chassis {
    Module_status chassis_status;
    imu_data* chassis_imu;
};

// 板间通信定义

// 云台板发送数据
struct gimbal_board_send {
    Robot_mode robot_mode;            // 遥控器在云台主控 在stop模式与云台重要模块掉线的情况下使底盘断电
    Chassis_mode chassis_mode;        // 底盘模式
    cmd_chassis_speed chassis_speed;  // 底盘速度控制
};

// 底盘板发送数据
struct chassis_board_send {
    Module_status chassis_board_status;  //底盘状态
    float gyro_yaw;                      //底盘主控imu yaw角速度数据
    uint16_t bullet_speed_max;           //裁判系统弹速
    uint16_t heat_limit_remain;          //剩余热量
};
#pragma pack()
#endif