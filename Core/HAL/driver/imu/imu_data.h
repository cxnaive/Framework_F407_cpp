#ifndef _IMU_DATA_H_
#define _IMU_DATA_H_

//此处欧拉角定义为 绕固定参考坐标轴旋转X-Y-Z 也就是 RPY角 Roll Pitch Yaw
class imu_data {
   public:
    float accel[3];  // X Y Z加速度
    //按陀螺仪原始角速度定义 roll pitch yaw
    float gyro[3];        // 角速度 弧度每秒
    float euler[3];       // 欧拉角 弧度
    float gyro_deg[3];    //角速度 度每秒
    float euler_deg[3];   // 欧拉角 角度
    float euler_8192[3];  // 欧拉角 编码器版 0-8192

    int round;
    float yaw_8192_real;
    float last_yaw;
    imu_data() { reset(); }
    void update_euler(float _euler[3]);
    void reset();
};

#endif