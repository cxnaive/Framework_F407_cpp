#ifndef _DATATYPES_H
#define _DATATYPES_H

#include <vector>
#include "soft_crc.h"
#include "stdint.h"

#pragma pack(1)
class general_data {
   public:
    uint8_t len;
    std::vector<uint8_t> data;
    uint16_t crc16;
    void to_buffer(uint8_t* buffer);
    void read_buffer(uint8_t* buffer);
    uint8_t check_read_buffer(uint8_t* buffer, uint8_t buffer_size, CRC16& checker);
};

#pragma pack()
uint8_t buffer_check_valid(uint8_t* buffer, uint32_t buffer_size, CRC16& checker);

#endif