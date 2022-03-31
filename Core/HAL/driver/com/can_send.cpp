#include "can_send.h"

#include "soft_crc.h"
#include "string.h"

can_send::can_send(const can_send_config& _config) {
    config = _config;
}

void can_send::send(const uint8_t* data, uint8_t len) {
    size_t buf_len = len + 5;
    buffer.reserve(buf_len);
    buffer[0] = 's';
    buffer[1] = len;
    buffer[buf_len - 1] = 'e';
    memcpy(buffer.data() + 2, data, len);
    uint16_t crc_now = CRC16::crc16_ccitt.check_sum(buffer.data() + 1,len + 1);
    memcpy(buffer.data() + 2 + len, &crc_now, 2);
    for (size_t idx = 0; idx < buf_len; idx += 8) {
        uint32_t dlc = buf_len - idx >= 8 ? 8 : buf_len - idx;
        config.device->send(config.can_identifier,buffer.data() + idx,dlc);
    }
}

void can_send::send(const std::vector<uint8_t>& data){
    send(data.data(),data.size());
}