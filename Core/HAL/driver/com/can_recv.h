#ifndef _CAN_RECV_H
#define _CAN_RECV_H
#include <bsp_can.h>
#include <datatypes.h>
#include <monitor.h>
#include <stdint.h>
#include <string.h>

//注意：使用can_send/can_recv进行通信的数据结构，大小不能超过256字节
template <typename T>
class can_recv {
   public:
    typedef std::function<void(can_recv<T>*)> can_recv_notifiy;
    struct can_recv_config {
        BSP_CanTypeDef* device;
        uint16_t can_identifier;
        can_recv::can_recv_notifiy notify_func;
        monitor::lost_callback lost_callback;
    };
    general_data data_rx;
    uint8_t recv_status;
    uint8_t recv_once;
    uint8_t data_updated;
    uint8_t data_len;
    T* recv_data;

   private:
    can_recv_config config;
    uint8_t buf_len;
    uint8_t recv_len;
    std::vector<uint8_t> rxbuf;

    void rx_callback(BSP_CanTypeDef* can_device, uint32_t identifier, uint8_t* data, uint32_t len) {
        if (can_device != config.device) return;
        if (identifier != config.can_identifier) return;
        if (data[0] == 's') {
            recv_status = 1;
        }
        if (recv_status) {
            if (recv_len + len > buf_len) {
                recv_status = 0;
                recv_len = 0;
                return;
            }
            memcpy(rxbuf.data() + recv_len, data, len);
            recv_len += len;
            if (rxbuf[recv_len - 1] == 'e' && recv_len == buf_len) {
                if (buffer_check_valid(rxbuf.data() + 1, buf_len - 2, CRC16::crc16_ccitt)) {
                    data_rx.read_buffer(rxbuf.data() + 1);
                    recv_status = 0;
                    recv_len = 0;
                    data_updated = 1;
                    recv_once = 1;
                    monitor_item.reset();
                    if (config.notify_func) {
                        config.notify_func(this);
                    }
                }
            }
        }
    }

   public:
    monitor monitor_item;
    can_recv(const can_recv_config& _config) : monitor_item(config.lost_callback, 5, this) {
        config = _config;
        data_len = sizeof(T);
        data_rx.len = data_len;
        data_rx.data.resize(data_len);
        buf_len = data_len + 5;
        rxbuf.resize(buf_len);
        recv_len = recv_status = data_updated = 0;
        recv_once = 0;
        recv_data = (T*)data_rx.data.data();
        BSP_CanTypeDef::can_rx_callback now_callback = std::bind(&can_recv::rx_callback, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4);
        config.device->register_rx_callback(this, now_callback);
        config.device->add_filter(config.can_identifier);
    }
    ~can_recv() {
        config.device->unregister_rx_callback(this);
        config.device->remove_filter(config.can_identifier);
    }
    bool is_online() {
        return recv_once != 0 && monitor_item.is_online();
    }
};

#endif