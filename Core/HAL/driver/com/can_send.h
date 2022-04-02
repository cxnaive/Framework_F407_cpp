#ifndef _CAN_SEND_H
#define _CAN_SEND_H
#include <bsp_can.h>
#include <datatypes.h>
#include <soft_crc.h>
#include <string.h>

#include <vector>
//注意：使用can_send/can_recv进行通信的数据结构，大小不能超过256字节
template <typename T>
class can_send {
   public:
    struct can_send_config {
        BSP_CanTypeDef* device;
        uint16_t can_identifier;
    };

   private:
    can_send_config config;
    std::vector<uint8_t> buffer;
    size_t buf_len;
    size_t data_len;

   public:
    can_send(const can_send_config& _config) {
        config = _config;
        data_len = sizeof(T);
        buf_len = data_len + 5;
        buffer.reserve(buf_len);
        buffer[0] = 's';
        buffer[1] = data_len;
        buffer[buf_len - 1] = 'e';
    }
    void send(const T* data) {
        memcpy(buffer.data() + 2, data, data_len);
        uint16_t crc_now = CRC16::crc16_ccitt.check_sum(buffer.data() + 1, data_len + 1);
        memcpy(buffer.data() + 2 + data_len, &crc_now, 2);
        for (size_t idx = 0; idx < buf_len; idx += 8) {
            uint32_t dlc = buf_len - idx >= 8 ? 8 : buf_len - idx;
            config.device->send(config.can_identifier, buffer.data() + idx, dlc);
        }
    }
    void send(const T& data) {
        send(&data);
    }
};

// can_send* CanSend_Create(can_send_config* config);
// void CanSend_Send(can_send* obj, uint8_t* data);
#endif