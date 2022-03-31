#ifndef _SOFT_CRC_H
#define _SOFT_CRC_H
#include <vector>

#include "stdint.h"
class CRC16 {
   public:
    uint16_t table[256];
    CRC16(uint16_t poly);
    uint16_t check_sum(const uint8_t* data, uint32_t num_bytes, uint16_t crc_init = 0xFFFF);
    uint16_t check_sum(const std::vector<uint8_t>& data, uint16_t crc_init = 0xFFFF);
    //预定义CRC解算
    static CRC16 crc16_ccitt;
};

class CRC8 {
   public:
    uint8_t table[256];
    CRC8(uint8_t poly);
    uint8_t check_sum(const uint8_t* data, uint32_t num_bytes, uint8_t crc_init = 0xFF);
    uint8_t check_sum(const std::vector<uint8_t>& data, uint8_t crc_init = 0xFF);
    //预定义CRC解算
    static CRC8 crc8_maxim;
};

#endif