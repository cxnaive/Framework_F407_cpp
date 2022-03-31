#ifndef _PC_DATA_H
#define _PC_DATA_H
#include <stdint.h>
#pragma pack(1)
struct pc_send {
    float euler[3];
    uint8_t auto_mode_flag;
};

struct pc_recv {
    float roll;
    float pitch;
    float yaw;
    float wait_time;  // 开火延迟时间
};
#pragma pack()

#endif