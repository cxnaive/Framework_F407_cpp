/* 演示如何基于can_send/can_recv 构建特质化的通信*/
#ifndef _MINIPC_H
#define _MINIPC_H
#include "can_recv.h"
#include "can_send.h"
#include "datatypes.h"
#include "monitor.h"
#include "pc_data.h"
#include "stdint.h"

class canpc {
   public:
    struct canpc_config {
        BSP_CanTypeDef* device;
        uint16_t send_identifer;
        uint16_t recv_identifer;
        monitor::lost_callback lost_callback;
    };

   private:
    canpc_config config;
    can_send sender;
    can_recv recver;

   public:
    pc_recv* pc_recv_data;
    uint8_t* data_updated;
    canpc(const canpc_config& _config);
    void send(const pc_send& data);
};

// canpc* CanPC_Create(canpc_config* config);
// void CanPC_Send(canpc* obj, canpc_send* data);
#endif