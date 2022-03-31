#include <stdlib.h>
#include <ws2812b.h>

#include "bsp_pwm.h"
#include "string.h"

#define BIT_1 140u
#define BIT_0 70u
//预定义颜色
ws2812::color_rgb red = {20, 0, 0};
ws2812::color_rgb green = {0, 10, 0};
ws2812::color_rgb purple = {14, 4, 23};
ws2812::color_rgb blue = {0, 0, 20};
ws2812::color_rgb off = {0, 0, 0};

// 工具函数，转换rgb值到一个长度24的uint16数组（TIMER的CCR寄存器为16位）
void ws2812::rgb_2_raw(uint16_t* buff, color_rgb color) {
    for (uint16_t i = 0; i < 8; ++i) {
        buff[i] = ((color.g << i) & (0x80)) ? BIT_1 : BIT_0;
        buff[i + 8] = ((color.r << i) & (0x80)) ? BIT_1 : BIT_0;
        buff[i + 16] = ((color.b << i) & (0x80)) ? BIT_1 : BIT_0;
    }
}
// 转换一个数列
void ws2812::array_2_raw(uint16_t* buff, const color_rgb* color_buff, uint32_t len) {
    for (uint32_t i = 0; i < len; ++i) {
        rgb_2_raw(buff + i * 24, color_buff[i]);
    }
}

ws2812::ws2812(const ws2812_config& _config) {
    send_len = 3 + 24 * config.max_len + 1;
    buffer.resize(send_len);
    buffer[0] = buffer[1] = buffer[2] = buffer[send_len - 1] = 0;
    frame_start = buffer.data() + 3;
}

void ws2812::send_frame() {
    config.pwm_device->start_ccr_dma((uint32_t*)buffer.data(), send_len);
}

void ws2812::set_all(const color_rgb& color){
    for (size_t i = 0; i < config.max_len; ++i) {
        rgb_2_raw(frame_start + i * 24, color);
    }
    send_frame();
}

void ws2812::close_all(){
    set_all(off);
}

void ws2812::set_array(const color_rgb* color_buffer,uint32_t len){
    array_2_raw(frame_start,color_buffer,len);
    send_frame();
}

void ws2812::set_array(const std::vector<color_rgb>& color_buffer){
    set_array(color_buffer.data(),color_buffer.size());
}