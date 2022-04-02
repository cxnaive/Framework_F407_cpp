#ifndef _CHASSIS_H
#define _CHASSIS_H
#include <bmi088.h>
#include <can_motor.h>
#include <robot_def.h>
#include <super_cap_wuli.h>
class chassis {
   private:
    bmi088_imu imu;
    super_cap_wuli super_cap;
    can_motor lf, rf, lb, rb;  // forward back left right

    cmd_chassis* cmd_data;       //接收到的指令数据
    upload_chassis upload_data;  //回传的数据
    float offset_x, offset_y;    // 旋转中心距离底盘的距离，云台位于正中心时默认设为0
    void mecanum_calculate(float vx, float vy, float rotate);
    void chassis_calculate();   // 将基于offset的速度映射到实际底盘坐标系的方向上
    float auto_rotate_param();  //自动计算小陀螺转速
   public:
    chassis();
    void update();
};

#endif