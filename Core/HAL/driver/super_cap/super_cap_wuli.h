#ifndef _SUPER_CAP_WULI_H
#define _SUPER_CAP_WULI_H

#include "bsp_can.h"
#include "monitor.h"
#include "stdint.h"
#include "warpper.h"
#include <vector>
#define SUPER_CAP_WULI_RX_ID 0x211  //雾列超级电容控制板
#define SUPER_CAP_WULI_TX_ID 0x210

class super_cap_wuli {
   public:
    struct super_cap_wuli_config {
        BSP_CanTypeDef* can_device;
        uint16_t super_cap_wuli_tx_id;
        uint16_t super_cap_wuli_rx_id;
        monitor::lost_callback lost_callback;
    };

   private:
    static std::vector<super_cap_wuli*> supercaps;
    super_cap_wuli_config config;
    float power_set;  // 用户设定值
    void super_cap_rx_callback(BSP_CanTypeDef* can_device, uint32_t identifier, uint8_t* data, uint32_t len);
    void feedback_data_update(uint8_t *data);
    void send_set_power();

   public:
    friend void Supercap_wuli_Loop();
    float voltage_input_fdb;  // 返回值 电容组输入电压
    float voltage_cap_fdb;    // 返回值 电容输出电压
    float current_input_fdb;  // 返回值 电容输入电流
    float power_set_fdb;      // 返回的用户设定值 应与设定值相同
    monitor monitor_item;
    super_cap_wuli(const super_cap_wuli_config& _config);
    ~super_cap_wuli();
    void set_power(float _power);
};
#endif