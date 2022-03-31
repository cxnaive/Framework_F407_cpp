#include <datatypes.h>
#include "string.h"
uint8_t buffer_check_valid(uint8_t* buffer, uint32_t buffer_size, CRC16& checker) {
    if (buffer[0] + (uint32_t)3 != buffer_size) return 0;
    uint16_t crc_val;
    memcpy(&crc_val, buffer + buffer_size - 2, 2);
    uint16_t crc_chk = checker.check_sum(buffer, buffer_size - 2);
    return crc_chk == crc_val;
}

void general_data::to_buffer(uint8_t* buffer) {
    buffer[0] = len;
    memcpy(buffer + 1, data.data(), len);
    memcpy(buffer + 1 + len, &crc16, 2);
}
void general_data::read_buffer(uint8_t* buffer) {
    len = buffer[0];
    data.resize(len);
    memcpy(data.data(), buffer + 1, len);
    memcpy(&crc16, buffer + 1 + len, 2);
}
uint8_t general_data::check_read_buffer(uint8_t* buffer, uint8_t buffer_size, CRC16& checker) {
    uint8_t valid = buffer_check_valid(buffer, buffer_size, checker);
    if (!valid) return valid;
    read_buffer(buffer);
    return valid;
}