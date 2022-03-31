#include "gimbal_board_cmd.h"

#include "bsp_can.h"
#include "bsp_log.h"
#include "bsp_uart.h"
#include "pub_sub.h"
#include "string.h"
void board_com_lost(void* obj) {
}

//此处涉及到lamdba表达式的应用
gimbal_board_cmd::gimbal_board_cmd(bmi088_imu& _imu) : sender([&] {
                                                           //板间通信：发
                                                           can_send::can_send_config config;
                                                           config.device = &BSP_CanTypeDef::can_devices[1];
                                                           config.can_identifier = 0x004;
                                                           return config;
                                                       }()),
                                                       recver([&] {
                                                           //板间通信：收
                                                           can_recv::can_recv_config config;
                                                           config.device = &BSP_CanTypeDef::can_devices[1];
                                                           config.can_identifier = 0x003;
                                                           config.notify_func = NULL;
                                                           config.lost_callback = board_com_lost;
                                                           config.data_len = sizeof(gimbal_board_send);
                                                           return config;
                                                       }()),
                                                       pc([&] {
                                                           //小电脑通信配置
                                                           canpc::canpc_config config;
                                                           config.device = &BSP_CanTypeDef::can_devices[0];
                                                           config.recv_identifer = 0x001;
                                                           return config;
                                                       }()),
                                                       remote([&] {
                                                           dt7Remote::dt7_config config;
                                                           config.uart_device = &BSP_UART_Typedef::uart_ports[UART_REMOTE_PORT];
                                                           return config;
                                                       }()),
                                                       board_buzzer([&] {
                                                           buzzer::buzzer_config config;
                                                           config.pwm_device = &BSP_PWM_Typedef::pwm_ports[PWM_BUZZER_PORT];
                                                           config.music = &buzzer::buzzer_musics[1];
                                                           return config;
                                                       }()),
                                                       gimbal_imu(_imu) {
    robot_mode = robot_stop;
    robot_ready = 0;
    autoaim_mode = auto_aim_off;
    //指针指向接收的实际数据
    board_recv = (chassis_board_send*)recver.data_rx.data.data();
    memset(&gimbal_control, 0, sizeof(cmd_gimbal));
    memset(&shoot_control, 0, sizeof(cmd_shoot));
    memset(&board_send, 0, sizeof(gimbal_board_send));
}

void gimbal_board_cmd::update() {
    //第一步，判断机器人工作模式

    //初始化为RUN
    robot_mode = robot_run;

    //判断板间通信在线
    if (!recver.monitor_item.is_online()) {
        robot_mode = robot_stop;  //板间通信掉线，机器人停止
    }

    //判断云台IMU在线且初始化完成
    if (!gimbal_imu.monitor_item.is_online() || gimbal_imu.bias_init_success == 0) {
        robot_mode = robot_stop;  //云台IMU掉线或尚未初始化完成
    }

    //如果除遥控器之外都已经上线，说明机器人初始化完成，进入ready状态
    if (robot_mode == robot_run) {
        if (!robot_ready) {
            robot_ready = 1;
            board_buzzer.start();  //蜂鸣器播放初始化提示音
        }
    }

    //遥控器判断
    if (!remote.monitor_item.is_online()) {
        //遥控器掉线
        robot_mode = robot_stop;
    } else if (remote.data.input_mode == dt7Remote::RC_Stop) {
        //遥控器处于stop档位
        robot_mode = robot_stop;
    }

    //第二步，机器人控制主要逻辑
    // STOP模式
    if (robot_mode == robot_stop) {
        stop_mode_update();
    }
    // 运行模式
    if (robot_mode == robot_run) {
        //遥控器控制模式
        if (remote.data.input_mode == dt7Remote::RC_Remote) {
            remote_mode_update();
        } else if (remote.data.input_mode == dt7Remote::RC_MouseKey) {
            mouse_key_mode_update();
        }
    }
    //发布命令，板间通信
    send_cmd_and_data();
}

void gimbal_board_cmd::stop_mode_update() {
    board_send.robot_mode = robot_stop;
    board_send.chassis_mode = chassis_stop;
    gimbal_control.gimbal_mode = gimbal_stop;
    shoot_control.friction_mode = friction_stop;
    shoot_control.bullet_mode = bullet_stop;
}

void gimbal_board_cmd::remote_mode_update() {
    //云台控制参数
    gimbal_control.gimbal_mode = gimbal_run;
    gimbal_control.yaw -= 0.04f * ((float)remote.data.rc.ch2 - CHx_BIAS);
    gimbal_control.pitch = -1.0f * ((float)remote.data.rc.ch3 - CHx_BIAS);
    gimbal_control.rotate_feedforward = 0;  //目前没有使用小陀螺前馈

    //底盘控制参数
    if (remote.data.rc.s1 == 1) {
        //小陀螺模式
        board_send.chassis_mode = chassis_rotate_run;
        board_send.chassis_speed.vy = 16.0 * 0.6 * (float)(remote.data.rc.ch1 - CHx_BIAS);
        board_send.chassis_speed.vx = 16.0 * 0.6 * (float)(remote.data.rc.ch0 - CHx_BIAS);
    } else {
        //底盘跟随模式
        board_send.chassis_mode = chassis_run_follow_offset;
        board_send.chassis_speed.vy = 16.0 * (float)(remote.data.rc.ch1 - CHx_BIAS);
        board_send.chassis_speed.vx = 16.0 * (float)(remote.data.rc.ch0 - CHx_BIAS);
    }

    //发射机构控制
    if (remote.data.rc.s1 == 2) {
        shoot_control.bullet_mode = bullet_stop;      //在发射停止位关闭播弹轮
        shoot_control.friction_mode = friction_stop;  //在发射停止位关闭摩擦轮
        //弹仓盖控制
        if (remote.data.rc.ch4 > CHx_BIAS + 400) shoot_control.mag_mode = magazine_open;
        if (remote.data.rc.ch4 < CHx_BIAS - 400) shoot_control.mag_mode = magazine_close;
    } else {
        //发弹控制
        shoot_control.friction_mode = friction_run;                                //开摩擦轮
        shoot_control.bullet_mode = bullet_continuous;                             //连续发射
        shoot_control.fire_rate = 0.01f * (float)(remote.data.rc.ch4 - CHx_BIAS);  //射频
        shoot_control.heat_limit_remain = board_recv->heat_limit_remain;           //下板传回的热量剩余
        shoot_control.bullet_speed = board_recv->bullet_speed_max;                 //下板传回的子弹速度上限
    }
}

void gimbal_board_cmd::mouse_key_mode_update() {
    // r:小陀螺模式
    if (remote.data.key_single_press_cnt.r != remote.last_data.key_single_press_cnt.r) {
        if (board_send.chassis_mode != chassis_rotate_run) {
            board_send.chassis_mode = chassis_rotate_run;  //小陀螺模式
            gimbal_control.gimbal_mode = gimbal_run;       //云台正常模式
        } else {
            board_send.chassis_mode = chassis_run_follow_offset;  //底盘跟随云台
            gimbal_control.gimbal_mode = gimbal_run;              //云台正常模式
        }
    }
    // v:云台底盘独立模式
    if (remote.data.key_single_press_cnt.v != remote.last_data.key_single_press_cnt.v) {
        if (board_send.chassis_mode != chassis_run || gimbal_control.gimbal_mode != gimbal_run) {
            board_send.chassis_mode = chassis_run;    //底盘独立模式
            gimbal_control.gimbal_mode = gimbal_run;  //云台正常模式
        } else {
            board_send.chassis_mode = chassis_run_follow_offset;  //底盘跟随云台
            gimbal_control.gimbal_mode = gimbal_run;              //云台正常模式
        }
    }
    // x:云台跟随底盘模式
    if (remote.data.key_single_press_cnt.x != remote.last_data.key_single_press_cnt.x) {
        if (board_send.chassis_mode != chassis_run || gimbal_control.gimbal_mode != gimbal_middle) {
            board_send.chassis_mode = chassis_run;    //底盘独立模式
            gimbal_control.gimbal_mode = gimbal_run;  //云台跟随底盘模式
        } else {
            board_send.chassis_mode = chassis_run_follow_offset;  //底盘跟随云台
            gimbal_control.gimbal_mode = gimbal_run;              //云台正常模式
        }
    }
    // z:爬坡模式（待添加）
    // f:正常自瞄模式
    if (remote.data.key_single_press_cnt.f != remote.last_data.key_single_press_cnt.f) {
        if (autoaim_mode != auto_aim_normal) {
            autoaim_mode = auto_aim_normal;
        } else {
            autoaim_mode = auto_aim_off;
        }
    }
    // g:小符
    if (remote.data.key_single_press_cnt.f != remote.last_data.key_single_press_cnt.f) {
        if (autoaim_mode != auto_aim_buff_small) {
            autoaim_mode = auto_aim_buff_small;
        } else {
            autoaim_mode = auto_aim_off;
        }
    }
    // b:大符
    if (remote.data.key_single_press_cnt.f != remote.last_data.key_single_press_cnt.f) {
        if (autoaim_mode != auto_aim_buff_big) {
            autoaim_mode = auto_aim_buff_big;
        } else {
            autoaim_mode = auto_aim_off;
        }
    }
    // c:开关弹仓
    if (remote.data.key_single_press_cnt.c != remote.last_data.key_single_press_cnt.c) {
        if (shoot_control.mag_mode == magazine_open) {
            shoot_control.mag_mode = magazine_close;
        } else {
            shoot_control.mag_mode = magazine_open;
        }
    }
    //底盘控制参数
    // wasd
    if (remote.data.key_down.w) board_send.chassis_speed.vy = 8000;
    if (remote.data.key_down.s) board_send.chassis_speed.vy = -8000;
    if (remote.data.key_down.d) board_send.chassis_speed.vx = 8000;
    if (remote.data.key_down.a) board_send.chassis_speed.vx = -8000;

    //按住ctrl减速
    if (remote.data.key_down.ctrl) {
        board_send.chassis_speed.vx /= 3.0;
        board_send.chassis_speed.vy /= 3.0;
    }

    //按住shift加速
    if (remote.data.key_down.shift) {
        board_send.chassis_speed.vx *= 3.0;
        board_send.chassis_speed.vy *= 3.0;
    }

    // q和e
    if (remote.data.key_down.q) {
        if (board_send.chassis_mode == chassis_run) {
            board_send.chassis_speed.rotate = -90;
        } else if (board_send.chassis_mode == chassis_run_follow_offset) {
            gimbal_control.yaw -= 15;  //通过控制云台yaw轴带动底盘
        }
    }

    if (remote.data.key_down.e) {
        if (board_send.chassis_mode == chassis_run) {
            board_send.chassis_speed.rotate = 90;
        } else if (board_send.chassis_mode == chassis_run_follow_offset) {
            gimbal_control.yaw += 15;  //通过控制云台yaw轴带动底盘
        }
    }

    //云台控制参数
    if (gimbal_control.gimbal_mode == gimbal_run) {
        if (autoaim_mode == auto_aim_off) {
            gimbal_control.yaw -= 0.5f * (0.7f * (remote.data.mouse.x) + 0.3f * (remote.last_data.mouse.x));
            gimbal_control.pitch -= -0.1f * ((float)remote.data.mouse.y);
        } else {
            //计算真实yaw值
            float yaw_target = pc.pc_recv_data->yaw * 8192.0 / 2 / pi + gimbal_imu.data.round * 8192.0;
            if (pc.pc_recv_data->yaw - gimbal_imu.data.euler[2] > pi) yaw_target -= 8192;
            if (pc.pc_recv_data->yaw - gimbal_imu.data.euler[2] < -pi) yaw_target += 8192;
            gimbal_control.yaw = yaw_target;
            gimbal_control.pitch = pc.pc_recv_data->roll * 8192.0 / 2 / pi;  // 根据当前情况决定，pitch轴反馈为陀螺仪roll
        }
    } else if (gimbal_control.gimbal_mode == gimbal_middle) {
        //云台跟随底盘模式，待完善
    }

    //发射机构控制参数
    if (remote.data.rc.s1 == 2) {
        shoot_control.bullet_mode = bullet_stop;
        shoot_control.friction_mode = friction_stop;
    } else {
        //发弹控制，单发，双发, 射频和小电脑控制待完善
        shoot_control.friction_mode = friction_run;                       //开摩擦轮
        shoot_control.heat_limit_remain = board_recv->heat_limit_remain;  //下板传回的热量剩余
        shoot_control.bullet_speed = board_recv->bullet_speed_max;        //下板传回的子弹速度上限
        shoot_control.fire_rate = 3;                                      //固定射频
        if (remote.data.mouse.press_l) {
            shoot_control.bullet_mode = bullet_continuous;
        } else {
            shoot_control.bullet_mode = bullet_stop;
        }
    }
}

void gimbal_board_cmd::send_cmd_and_data() {
    static publisher<cmd_gimbal*> gimbal_puber("gimbal");
    static publisher<cmd_shoot*> shoot_puber("shoot");
    gimbal_puber.push(&gimbal_control);
    shoot_puber.push(&shoot_control);
    sender.send((uint8_t*)&board_send, sizeof(board_send));
}