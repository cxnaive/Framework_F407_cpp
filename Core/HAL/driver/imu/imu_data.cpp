#include <imu_data.h>
#include <common.h>
#include <string.h>
void imu_data::update_euler(float _euler[3]){
    euler[0] = _euler[0];
    euler[1] = _euler[1];
    euler[2] = _euler[2];
    euler_deg[0] = euler[0] * RAD2DEG;
    euler_deg[1] = euler[1] * RAD2DEG;
    euler_deg[2] = euler[2] * RAD2DEG;
    euler_8192[0] = euler_deg[0] * 8192 / 360;
    euler_8192[1] = euler_deg[1] * 8192 / 360;
    euler_8192[2] = euler_deg[2] * 8192 / 360;

    if(euler_8192[2] - last_yaw > 4096) round--;
    if(euler_8192[2] - last_yaw < -4096) round++;
    yaw_8192_real = euler_8192[2] + round * 8192;
    last_yaw = euler_8192[2];
}

void imu_data::reset(){
    memset(this,0,sizeof(imu_data));
}