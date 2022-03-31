/* 基于can_send/can_recv 创建的标准化的电机控制机制*/
#ifndef _CAN_MOTOR_H
#define _CAN_MOTOR_H
// #include "circular_queue.h"
#include <bsp_can.h>

#include <queue>

#include "monitor.h"
#include "pid.h"
#include "stdint.h"

class can_motor {
   public:
    enum Motor_Model_e {
        MODEL_3508 = 0,
        MODEL_2006,
        MODEL_6020
    };

    enum Motor_PID_Model_e {
        CURRENT_LOOP = 0,
        SPEED_LOOP,
        POSITION_LOOP
    };  //速度环/位置环/电流环

    enum Motor_FDB_Model_e {
        MOTOR_FDB = 0,
        OTHER_FDB
    };  //反馈模式：由电机自身反馈，由外部变量反馈（如陀螺仪）

    enum Motor_OUTPUT_Model_e {
        MOTOR_OUTPUT_NORMAL = 0,
        MOTOR_OUTPUT_REVERSE
    };  //是否输出反转

    enum Motor_Enable_e {
        MOTOR_ENABLE = 0,
        MOTOR_STOP
    };

    struct can_motor_config {
        BSP_CanTypeDef* can_device;
        uint8_t motor_set_id;  //电调上通过闪灯次数确定的id
        Motor_Model_e motor_model;
        Motor_PID_Model_e motor_pid_model;
        Motor_FDB_Model_e position_fdb_model;
        Motor_FDB_Model_e speed_fdb_model;
        Motor_OUTPUT_Model_e output_model;
        PID::PID_config config_speed;
        PID::PID_config config_position;
        float* speed_pid_fdb;        // 对于速度环OTHER_FDB模式的ref指针，默认值应为NULL
        float* position_pid_fdb;     // 对于位置环OTHER_FDB模式的ref指针，默认值应为NULL
        float* speed_feedforward;    //速度环输入前馈指针，默认值应该为NULL
        float* current_feedforward;  //电流/电压 前馈指针，默认值应该为NULL
        monitor::lost_callback lost_callback;
    };

    //电机ID号管理辅助类
    class can_motor_registry {
       public:
        can_motor* identifier_registry[3][4];  //按照发送can_identifier注册，一个can总线上最多注册12个电机
        can_motor* motor_id_registry[3][8];    //按照电机自身编号注册
        can_motor_registry();
        void reset();
        uint32_t register_motor(can_motor* motor);  //按照电机编号注册电机，反回电机的接收can_identifier
    };

   private:
    static constexpr uint32_t identifiers[3] = {0x200, 0x1FF, 0x2FF};
    static can_motor_registry can_reg[DEVICE_CAN_CNT];  //每一个CAN总线都需要
    can_motor_config config;
    uint32_t motor_rx_id;                                                                              //电机接收反馈的can报文ID
    std::queue<float> pos_delta_queue;                                                                 //计算角速度的循环队列
    float position_sum;                                                                                //队列中所有值的和
    void motor_rx_callback(BSP_CanTypeDef* device, uint32_t identifier, uint8_t* data, uint32_t len);  //回调函数
    void motor_feedback_update(uint8_t* data);                                                         //更新电机反馈
    static void can_send(uint8_t can_id, uint16_t identifier, short motor_data_id1, short motor_data_id2, short motor_data_id3, short motor_data_id4);

   public:
    friend void Motor_Loop();
    Motor_Enable_e motor_enable;
    short fdbPosition;       //电机的编码器反馈值
    short last_fdbPosition;  //电机上次的编码器反馈值
    short fdbSpeed;          //电机反馈的转速/rpm
    short electric_current;  //电机实际转矩电流
    short round;             //电机转过的圈数
    uint8_t temperature;     //电机温度

    float real_position;       //过零处理后的电机转子位置
    float last_real_position;  //上次真实转过的角度
    PID speed_pid;             //速度环pid
    PID position_pid;          //位置环pid
    short current_output;

    float line_speed;  //线速度（m/s，根据角速度算出）
    float velocity;    //用电机编码器计算出来的角速度（单位：度每秒）
    monitor monitor_item;
    can_motor(const can_motor_config& _config);
    ~can_motor();
    void enable();
    void stop();
};

#endif