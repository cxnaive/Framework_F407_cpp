#ifndef _BSP_CAN_H
#define _BSP_CAN_H
#include <can.h>

#include <functional>
#include <unordered_map>
#include <vector>

#include "stdint.h"
#define DEVICE_CAN_CNT 2
#define FILTER_MAX_CNT (4 * 14)

class BSP_CanTypeDef {
   public:
    typedef std::function<void(BSP_CanTypeDef *can_device, uint32_t identifier, uint8_t *data, uint32_t len)> can_rx_callback;

   private:
    CAN_HandleTypeDef *device;  //总线设备
    uint32_t tx_mailbox;
    std::unordered_map<void *, can_rx_callback> call_backs;
    uint32_t bank_prefix;              //不同can对应的过滤器相关参数值
    uint16_t filters[FILTER_MAX_CNT];  //按标准帧ID定义
   public:
    uint8_t id;
    void init(CAN_HandleTypeDef *_device, uint32_t _bank_prefix, uint8_t _id);
    //  仅对HAL库的can发送函数进行了形式上的封装，实际参数依然依赖外部传参
    void send(uint16_t identifier, uint8_t *data, uint32_t len);
    void add_filter(uint32_t filter);
    void remove_filter(uint32_t filter);
    // 注册回调函数
    void register_rx_callback(void *identifier, can_rx_callback func);
    // 注销回调函数
    void unregister_rx_callback(void *identifier);
    // 友元函数，使之访问private成员
    friend void BSP_CAN_Init();
    friend void BSP_CAN_FifoMsg_Callback(CAN_HandleTypeDef *hcan, uint32_t fifo);
    // 静态数据成员
    static BSP_CanTypeDef can_devices[DEVICE_CAN_CNT];  //定义对应数量的can外设，即代码中定义can总线
   private:
    void update_filter(uint32_t filter_index);
};

void BSP_CAN_Init();
#endif