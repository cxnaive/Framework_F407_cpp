#include "can_recv.h"

#include "bsp_can.h"
#include "soft_crc.h"
#include "stdio.h"
#include "string.h"

can_recv::can_recv(const can_recv_config &_config) : monitor_item(config.lost_callback, 20, this) {
    config = _config;
    BSP_CanTypeDef::can_rx_callback now_callback = std::bind(&can_recv::rx_callback, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4);
    config.device->register_rx_callback(this, now_callback);
    config.device->add_filter(config.can_identifier);
    data_rx.len = config.data_len;
    data_rx.data.resize(config.data_len);
    buf_len = config.data_len + 5;
    rxbuf.resize(buf_len);
    recv_len = recv_status = data_updated = 0;
}

can_recv::~can_recv(){
    config.device->unregister_rx_callback(this);
    config.device->remove_filter(config.can_identifier);
}

void can_recv::rx_callback(BSP_CanTypeDef *can_device, uint32_t identifier, uint8_t *data, uint32_t len) {
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
                monitor_item.reset();
                if (config.notify_func) {
                    config.notify_func(this);
                }
            }
        }
    }
}