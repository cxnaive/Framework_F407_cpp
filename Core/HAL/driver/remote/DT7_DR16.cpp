#include "DT7_DR16.h"

#include "string.h"
//键盘对应的通信协议数据
#define Key_W 0x0001
#define Key_S 0x0002
#define Key_D 0x0004
#define Key_A 0x0008
#define Key_Shift 0x0010
#define Key_ECtrl 0x0020
#define Key_Q 0x0040
#define Key_E 0x0080
#define Key_R 0x0100
#define Key_F 0x0200
#define Key_G 0x0400
#define Key_Z 0x0800
#define Key_X 0x1000
#define Key_C 0x2000
#define Key_V 0x4000
#define Key_B 0x8000

dt7Remote::dt7Remote(const dt7_config& _config) : monitor_item(_config.lost_callback, 10, this) {
    config = _config;
    BSP_UART_Typedef::uart_rx_callback now_callback = std::bind(&dt7Remote::dt7_rx_callback, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
    config.uart_device->register_rx_callback(this, now_callback);
    memset(&data, 0, sizeof(dt7_rx_data));
    memset(&last_data, 0, sizeof(dt7_rx_data));
}

dt7Remote::~dt7Remote() {
    config.uart_device->unregister_rx_callback(this);
}

void dt7Remote::dt7_rx_callback(BSP_UART_Typedef* uart_device, uint8_t* data, uint32_t len) {
    if (len == DT7_RX_SIZE && uart_device == config.uart_device) {
        memcpy(primary_data, data, DT7_RX_SIZE);
        monitor_item.reset();
        dt7_data_solve();
    }
}

void dt7Remote::dt7_data_solve() {
    // last赋值
    last_data = data;
    // 拨杆值解算
    data.rc.ch0 = ((int16_t)primary_data[0] | ((int16_t)primary_data[1] << 8)) & 0x07FF;
    data.rc.ch1 = (((int16_t)primary_data[1] >> 3) | ((int16_t)primary_data[2] << 5)) & 0x07FF;
    data.rc.ch2 = (((int16_t)primary_data[2] >> 6) | ((int16_t)primary_data[3] << 2) | ((int16_t)primary_data[4] << 10)) & 0x07FF;
    data.rc.ch3 = (((int16_t)primary_data[4] >> 1) | ((int16_t)primary_data[5] << 7)) & 0x07FF;
    data.rc.ch4 = ((int16_t)primary_data[16] | ((int16_t)primary_data[17] << 8)) & 0x07FF;
    data.rc.s1 = ((primary_data[5] >> 4) & 0x000C) >> 2;
    data.rc.s2 = ((primary_data[5] >> 4) & 0x0003);
    // 定义统一输入模式
    if (data.rc.s2 == 2) data.input_mode = RC_Stop;
    if (data.rc.s2 == 3) data.input_mode = RC_Remote;
    if (data.rc.s2 == 1) data.input_mode = RC_MouseKey;

    // 鼠标值解算
    data.mouse.x = ((int16_t)primary_data[6]) | ((int16_t)primary_data[7] << 8);
    data.mouse.y = ((int16_t)primary_data[8]) | ((int16_t)primary_data[9] << 8);
    data.mouse.z = ((int16_t)primary_data[10]) | ((int16_t)primary_data[11] << 8);
    data.mouse.press_l = primary_data[12];
    data.mouse.press_r = primary_data[13];
    // 键盘按键解算-是否按下
    uint16_t keyboard = (int16_t)primary_data[14] | ((int16_t)primary_data[15] << 8);
    data.key_down.w = (keyboard & Key_W) > 0;
    data.key_down.s = (keyboard & Key_S) > 0;
    data.key_down.d = (keyboard & Key_D) > 0;
    data.key_down.a = (keyboard & Key_A) > 0;
    data.key_down.shift = (keyboard & Key_Shift) > 0;
    data.key_down.ctrl = (keyboard & Key_ECtrl) > 0;
    data.key_down.q = (keyboard & Key_Q) > 0;
    data.key_down.e = (keyboard & Key_E) > 0;
    data.key_down.r = (keyboard & Key_R) > 0;
    data.key_down.f = (keyboard & Key_F) > 0;
    data.key_down.g = (keyboard & Key_G) > 0;
    data.key_down.z = (keyboard & Key_Z) > 0;
    data.key_down.x = (keyboard & Key_X) > 0;
    data.key_down.c = (keyboard & Key_C) > 0;
    data.key_down.v = (keyboard & Key_V) > 0;
    data.key_down.b = (keyboard & Key_B) > 0;

    // 排除ctrl和shift都按下的情况
    if (!(data.key_down.shift && data.key_down.ctrl)) {
        // shift+
        if (data.key_down.shift) {
            if (data.key_down.w && !last_data.key_down.w) data.key_shift_press_cnt.w++;
            if (data.key_down.s && !last_data.key_down.s) data.key_shift_press_cnt.s++;
            if (data.key_down.d && !last_data.key_down.d) data.key_shift_press_cnt.d++;
            if (data.key_down.a && !last_data.key_down.a) data.key_shift_press_cnt.a++;
            if (data.key_down.q && !last_data.key_down.q) data.key_shift_press_cnt.q++;
            if (data.key_down.e && !last_data.key_down.e) data.key_shift_press_cnt.e++;
            if (data.key_down.r && !last_data.key_down.r) data.key_shift_press_cnt.r++;
            if (data.key_down.f && !last_data.key_down.f) data.key_shift_press_cnt.f++;
            if (data.key_down.g && !last_data.key_down.g) data.key_shift_press_cnt.g++;
            if (data.key_down.z && !last_data.key_down.z) data.key_shift_press_cnt.z++;
            if (data.key_down.x && !last_data.key_down.x) data.key_shift_press_cnt.x++;
            if (data.key_down.c && !last_data.key_down.c) data.key_shift_press_cnt.c++;
            if (data.key_down.v && !last_data.key_down.v) data.key_shift_press_cnt.v++;
            if (data.key_down.b && !last_data.key_down.b) data.key_shift_press_cnt.b++;
        } else if (data.key_down.ctrl) {  // ctrl
            if (data.key_down.w && !last_data.key_down.w) data.key_ctrl_press_cnt.w++;
            if (data.key_down.s && !last_data.key_down.s) data.key_ctrl_press_cnt.s++;
            if (data.key_down.d && !last_data.key_down.d) data.key_ctrl_press_cnt.d++;
            if (data.key_down.a && !last_data.key_down.a) data.key_ctrl_press_cnt.a++;
            if (data.key_down.q && !last_data.key_down.q) data.key_ctrl_press_cnt.q++;
            if (data.key_down.e && !last_data.key_down.e) data.key_ctrl_press_cnt.e++;
            if (data.key_down.r && !last_data.key_down.r) data.key_ctrl_press_cnt.r++;
            if (data.key_down.f && !last_data.key_down.f) data.key_ctrl_press_cnt.f++;
            if (data.key_down.g && !last_data.key_down.g) data.key_ctrl_press_cnt.g++;
            if (data.key_down.z && !last_data.key_down.z) data.key_ctrl_press_cnt.z++;
            if (data.key_down.x && !last_data.key_down.x) data.key_ctrl_press_cnt.x++;
            if (data.key_down.c && !last_data.key_down.c) data.key_ctrl_press_cnt.c++;
            if (data.key_down.v && !last_data.key_down.v) data.key_ctrl_press_cnt.v++;
            if (data.key_down.b && !last_data.key_down.b) data.key_ctrl_press_cnt.b++;
        } else {
            // ctrl shift都不按
            if (data.key_down.w && !last_data.key_down.w) data.key_single_press_cnt.w++;
            if (data.key_down.s && !last_data.key_down.s) data.key_single_press_cnt.s++;
            if (data.key_down.d && !last_data.key_down.d) data.key_single_press_cnt.d++;
            if (data.key_down.a && !last_data.key_down.a) data.key_single_press_cnt.a++;
            if (data.key_down.q && !last_data.key_down.q) data.key_single_press_cnt.q++;
            if (data.key_down.e && !last_data.key_down.e) data.key_single_press_cnt.e++;
            if (data.key_down.r && !last_data.key_down.r) data.key_single_press_cnt.r++;
            if (data.key_down.f && !last_data.key_down.f) data.key_single_press_cnt.f++;
            if (data.key_down.g && !last_data.key_down.g) data.key_single_press_cnt.g++;
            if (data.key_down.z && !last_data.key_down.z) data.key_single_press_cnt.z++;
            if (data.key_down.x && !last_data.key_down.x) data.key_single_press_cnt.x++;
            if (data.key_down.c && !last_data.key_down.c) data.key_single_press_cnt.c++;
            if (data.key_down.v && !last_data.key_down.v) data.key_single_press_cnt.v++;
            if (data.key_down.b && !last_data.key_down.b) data.key_single_press_cnt.b++;
        }
    }
}