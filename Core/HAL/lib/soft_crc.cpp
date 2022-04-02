#include "soft_crc.h"
#include "stdint.h"
#include "stddef.h"
uint16_t bit_reverse(uint16_t x) {
    uint16_t result = 0;
    for (int i = 0; i < 16; ++i) {
        uint16_t nowbit = (x >> i) & 1;
        result |= (nowbit << (15 - i));
    }
    return result;
}

uint8_t bit_reverse(uint8_t x) {
    uint8_t result = 0;
    for (int i = 0; i < 8; ++i) {
        uint8_t nowbit = (x >> i) & 1;
        result |= (nowbit << (7 - i));
    }
    return result;
}

CRC16::CRC16(uint16_t poly) {
    uint16_t table_poly = bit_reverse(poly);
    for (uint16_t i = 0; i < 256; ++i) {
        uint16_t crc = 0, c = i;
        for (uint16_t j = 0; j < 8; ++j) {
            if ((crc ^ c) & 0x0001) {
                crc = (crc >> 1) ^ table_poly;
            } else
                crc = crc >> 1;
            c = c >> 1;
        }
        table[i] = crc;
    }
}

uint16_t CRC16::check_sum(const uint8_t* data, uint32_t num_bytes, uint16_t crc_init) {
    uint16_t crc = crc_init;
    while (num_bytes--) crc = (crc >> 8) ^ table[(crc ^ *data++) & 0xff];
    return crc;
}

uint16_t CRC16::check_sum(const std::vector<uint8_t>& data, uint16_t crc_init) {
    uint16_t crc = crc_init;
    for (size_t i = 0; i < data.size(); ++i) {
        crc = (crc >> 8) ^ table[(crc ^ data[i]) & 0xff];
    }
    return crc;
}
//根据裁判系统协议，用CRC16_CCITT
CRC16 CRC16::crc16_ccitt = CRC16(0x1021);

CRC8::CRC8(uint8_t poly) {
    uint8_t table_poly = bit_reverse(poly);
    for (size_t i = 0; i < 256; ++i) {
        uint8_t crc = 0, c = i;
        for (uint8_t j = 0; j < 8; ++j) {
            if ((crc ^ c) & 0x01) {
                crc = (crc >> 1) ^ table_poly;
            } else
                crc = crc >> 1;
            c = c >> 1;
        }
        table[i] = crc;
    }
}
uint8_t CRC8::check_sum(const uint8_t* data, uint32_t num_bytes, uint8_t crc_init) {
    uint8_t crc = crc_init;
    while (num_bytes--) crc = table[(crc ^ *data++)];
    return crc;
}
uint8_t CRC8::check_sum(const std::vector<uint8_t>& data, uint8_t crc_init) {
    uint8_t crc = crc_init;
    for (size_t i = 0; i < data.size(); ++i) {
        crc = table[crc ^ data[i]];
    }
    return crc;
}

//根据裁判系统协议，使用CRC8_MAXIM多项式
CRC8 CRC8::crc8_maxim = CRC8(0x31);