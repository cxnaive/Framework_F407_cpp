#include "super_cap_wuli.h"

#include "bsp_can.h"
#include "bsp_log.h"
#include "stdlib.h"
#include "string.h"

std::vector<super_cap_wuli*> super_cap_wuli::supercaps;

super_cap_wuli::super_cap_wuli(const super_cap_wuli_config& _config) : monitor_item(_config.lost_callback, 5, this) {
    config = _config;
    BSP_CanTypeDef::can_rx_callback now_callback = std::bind(&super_cap_wuli::super_cap_rx_callback, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4);
    config.can_device->register_rx_callback(this, now_callback);
    config.can_device->add_filter(config.super_cap_wuli_rx_id);
    power_set = 0;
    voltage_input_fdb = voltage_cap_fdb = current_input_fdb = power_set_fdb = 0;
}

super_cap_wuli::~super_cap_wuli() {
    config.can_device->unregister_rx_callback(this);
    config.can_device->remove_filter(config.super_cap_wuli_rx_id);
    for (auto it = supercaps.begin(); it != supercaps.end(); ++it) {
        if (*it == this) {
            supercaps.erase(it);
            return;
        }
    }
}

void super_cap_wuli::super_cap_rx_callback(BSP_CanTypeDef* can_device, uint32_t identifier, uint8_t* data, uint32_t len) {
    if (config.can_device != can_device) return;
    if (identifier != config.super_cap_wuli_rx_id) return;
    monitor_item.reset();
    feedback_data_update(data);
}

void super_cap_wuli::feedback_data_update(uint8_t* data) {
    static uint16_t super_cap_wuli_raw[4];
    memcpy(super_cap_wuli_raw, data, sizeof(super_cap_wuli_raw));
    voltage_input_fdb = (float)super_cap_wuli_raw[0] / 100.f;
    voltage_cap_fdb = (float)super_cap_wuli_raw[1] / 100.f;
    current_input_fdb = (float)super_cap_wuli_raw[2] / 100.f;
    power_set_fdb = (float)super_cap_wuli_raw[3] / 100.f;
}

void super_cap_wuli::set_power(float _power) {
    power_set = _power;
}

void super_cap_wuli::send_set_power() {
    static uint8_t temp_data[8] = {0};
    uint16_t power_temp = power_set * 100;
    temp_data[0] = (uint8_t)(power_temp >> 8);
    temp_data[1] = (uint8_t)(power_temp);
    config.can_device->send(config.super_cap_wuli_tx_id, temp_data, 8);
}

void Supercap_wuli_Loop() {
    for (super_cap_wuli* cap : super_cap_wuli::supercaps) {
        cap->send_set_power();
    }
}