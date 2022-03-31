/***************注释仅供参考，实际以官方手册为准***************/
/********************遥控接收系统规格参数*********************
 *DJI DESSET 2.4G遥控接收系统   工作频率           2.4GHz ISM *
 *                              通信距离(开阔室外)   1km      *
 *------------------------------------------------------------*
 *DT7遥控器                     特性                7通道     *
 *                              工作电流/电压       120mA@3.7V*
 *                              电池          3.7V2000mA锂电池*
 *------------------------------------------------------------*
 *DR16接收机                    特性          2.4GHz D-Bus协议*
 *                              接收灵敏度(1%PER)   -97dBm    *
 *                              工作电流/电压       145mA@5V  *
 *                              电源                4-8.4V    *
 *                              电压             41mm*29mm*5mm*
 *                              重量                 10g      *
 **************************************************************/

/************************接收机DR16***************************
 *遥控器接收机输出信号为标准DBUS协议数据，当接收机和发射机建立*
 *连接后，接收机便会每隔7ms通过DBus发送一帧数据(18字节)       *
 *                                                            *
 *DBus通信参数如下:                                           *
 *                                                            *
 *DBus参数            数值                                    *
 *波特率             100Kbps                                  *
 *单元数据长度         9                                      *
 *奇偶校验位         偶校验                                   *
 *结束位               2                                      *
 *流控                 无                                     *
 **************************************************************/

/*************************发射机DT7***************************
 *                                                            *
 *   -----------------------------------------------------    *
 *   |     (上-1)                               (上-1)   |    *
 *   |SW_L|(中-3)                          SW_R|(中-3)   |    *
 *   |     (下-2)                               (下-2)   |    *
 *   |                                                   |    *
 *   |    | ^ |                                | ^ |     |    *
 *   |    | 3 |左摇杆                     右摇杆| 1 |     |    *
 *   | ---     ---                          ---     ---  |    *
 *   |<           2>                       <           0>|    *
 *   | ---     ---                          ---     ---  |    *
 *   |    |   |                                |   |     |    *
 *   |    |   |                                |   |     |    *
 *   |                                                   |    *
 *   -----------------------------------------------------    *
 *                                                            *
 **************************遥控器信息**************************
 *域            通道0      通道1   通道2   通道3   S1     S2  *
 *偏移            0          11      22      33    44     46  *
 *长度(bit)      11          11      11      11     2      2  *
 *符号位         无          无      无      无    无     无  *
 *范围           ***********最大值1684*********    *最大值3*  *
 *               *          中间值1024        *    *最小值1*  *
 *               ***********最小值364**********               *
 *功能                                            1:上   1:上 *
 *                                                2:下   2:下 *
 *                                                3:中   3:中 *
 *                                                            *
 ***************************鼠标信息***************************
 *域         鼠标x轴   鼠标y轴   鼠标z轴   鼠标左键   鼠标右键*
 *偏移         48        64        80        86        94   *
 *长度         16        16        16        8         8    *
 *符号位       有        有        有        无         无   *
 *范围         ******最大值32767*****        ***最大值1***   *
 *             *     最小值-32768   *        ***最小值0***   *
 *             ******静止值0*********                        *
 *功能       ***鼠标在XYZ轴的移动速度***   *鼠标左右键是否按下*
 *           *  负值表示往左移动       *   *    0:没按下      *
 *           ***正值表示往右移动********   *****1:按下*********
 *                                                           *
 *                                                           *
 ***************************键盘信息***************************
 *域              按键                                       *
 *偏移            102                                        *
 *长度            16                                         *
 *符号位          无                                          *
 *范围            位值标识                                    *
 *功能            每个按键对应一个bit                          *
 *                Bit 0:W键                                   *
 *                Bit 1:S键                                   *
 *                Bit 2:A键                                   *
 *                Bit 3:D键                                   *
 *                Bit 4:Shift键                               *
 *                Bit 5:Ctrl键                                *
 *                Bit 6:Q键                                   *
 *                Bit 7:E键                                   *
 *                Bit 8:R键                                   *
 *                Bit 9:F键                                   *
 *                Bit10:G键                                   *
 *                Bit11:Z键                                   *
 *                Bit12:X键                                   *
 *                Bit13:C键                                   *
 *                Bit14:V键                                   *
 *                Bit15:B键                                   *
 **************************************************************/
#ifndef _DT7_DR16_H_
#define _DT7_DR16_H_

#include "bsp_uart.h"
#include "monitor.h"
#include "stdint.h"

#define DT7_RX_SIZE 18
//通道中间值为1024
#define CHx_BIAS 1024

class dt7Remote {
   public:
    struct dt7_rx_data_rc {
        int16_t ch0;
        int16_t ch1;
        int16_t ch2;
        int16_t ch3;
        int16_t ch4;
        int8_t s1;  //左拨杆
        int8_t s2;  //右拨杆
    };

    struct dt7_rx_data_mouse {
        int16_t x;
        int16_t y;
        int16_t z;
        uint8_t press_l;
        uint8_t press_r;
    };

    struct dt7_rx_data_key {
        uint8_t w;
        uint8_t s;
        uint8_t d;
        uint8_t a;
        uint8_t shift;
        uint8_t ctrl;
        uint8_t q;
        uint8_t e;
        uint8_t r;
        uint8_t f;
        uint8_t g;
        uint8_t z;
        uint8_t x;
        uint8_t c;
        uint8_t v;
        uint8_t b;
    };

    enum rc_mode {
        RC_MouseKey = 1,
        RC_Stop,
        RC_Remote
    };  // 统一所有机器人的stop模式对应遥控器拨杆

    struct dt7_rx_data {
        dt7_rx_data_rc rc;        //拨杆
        dt7_rx_data_mouse mouse;  //键鼠
        dt7_rx_data_key key_down;
        dt7_rx_data_key key_single_press_cnt;  // 不含shift与ctrl
        dt7_rx_data_key key_shift_press_cnt;
        dt7_rx_data_key key_ctrl_press_cnt;
        rc_mode input_mode;
    };

    struct dt7_config {
        BSP_UART_Typedef* uart_device;
        monitor::lost_callback lost_callback;
    };

   private:
    uint8_t primary_data[DT7_RX_SIZE];  // 未解算的原始数据
    dt7_config config;
    void dt7_rx_callback(BSP_UART_Typedef* uart_device, uint8_t* data, uint32_t len);
    void dt7_data_solve();

   public:
    monitor monitor_item;
    dt7_rx_data data;       // 最近一次收到的数据
    dt7_rx_data last_data;  // 上一次收到的数据
    dt7Remote(const dt7_config& _config);
    ~dt7Remote();
};
#endif