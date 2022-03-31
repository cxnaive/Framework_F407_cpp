#ifndef _H_WS2812B_H
#define _H_WS2812B_H

#include <vector>

#include "bsp_pwm.h"
#include "stdint.h"

class ws2812 {
   public:
    struct color_rgb {
        uint8_t r, g, b;
    };

    struct ws2812_config {
        BSP_PWM_Typedef* pwm_device;
        uint32_t max_len;
    };

   private:
    std::vector<uint16_t> buffer;
    uint16_t* frame_start;
    uint16_t send_len;
    ws2812_config config;

    static void rgb_2_raw(uint16_t* buff, color_rgb color);
    static void array_2_raw(uint16_t* buff, const color_rgb* color_buff, uint32_t len);
    void send_frame();

   public:
    static color_rgb red, green, purple, blue, off;
    ws2812(const ws2812_config& _config);
    void set_all(const color_rgb& color);
    void close_all();
    void set_array(const color_rgb* color_buffer, uint32_t len);
    void set_array(const std::vector<color_rgb>& color_buffer);
};

// extern color_rgb red;
// extern color_rgb green;
// extern color_rgb purple;
// extern color_rgb blue;
// extern color_rgb off;

// ws2812* ws2812_create(ws2812_config* config);
// void ws2812_close_all(ws2812* obj);
// void ws2812_set_all(ws2812* obj, color_rgb color);
// void ws2812_set_array(ws2812* obj, color_rgb* color_buffer, uint32_t len);

#endif