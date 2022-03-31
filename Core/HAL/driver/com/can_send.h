#ifndef _CAN_SEND_H
#define _CAN_SEND_H
#include <vector>

#include "bsp_can.h"
#include "datatypes.h"

class can_send {
   public:
    struct can_send_config {
        BSP_CanTypeDef* device;
        uint16_t can_identifier;
    };

   private:
    can_send_config config;
    std::vector<uint8_t> buffer;

   public:
    can_send(const can_send_config& _config);
    void send(const uint8_t* data, uint8_t len);
    void send(const std::vector<uint8_t>& data);
};

// can_send* CanSend_Create(can_send_config* config);
// void CanSend_Send(can_send* obj, uint8_t* data);
#endif