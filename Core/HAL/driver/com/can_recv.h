#ifndef _CAN_RECV_H
#define _CAN_RECV_H
#include "bsp_can.h"
#include "datatypes.h"
#include "monitor.h"
#include "stdint.h"

class can_recv {
   public:
    typedef std::function<void(can_recv*)> can_recv_notifiy;
    struct can_recv_config {
        BSP_CanTypeDef* device;
        uint8_t data_len;
        uint16_t can_identifier;
        can_recv::can_recv_notifiy notify_func;
        monitor::lost_callback lost_callback;
    };
    general_data data_rx;
    uint8_t recv_status;
    uint8_t data_updated;

   private:
    can_recv_config config;
    uint8_t buf_len;
    uint8_t recv_len;
    std::vector<uint8_t> rxbuf;
    void rx_callback(BSP_CanTypeDef* can_device, uint32_t identifier, uint8_t* data, uint32_t len);

   public:
    monitor monitor_item;
    can_recv(const can_recv_config& _config);
    ~can_recv();
};

#endif