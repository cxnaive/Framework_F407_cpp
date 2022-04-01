#ifndef _GIMBAL_H_
#define _GIMBAL_H_
#include "bmi088.h"
#include "can_motor.h"
#include "robot_def.h"
class gimbal {
   private:
    bmi088_imu imu;  //云台IMU
    can_motor pitch;
    can_motor yaw;
    upload_gimbal upload_data;  //回传数据

   public:
    gimbal();
    void update();
};
#endif