/**
 ******************************************************************************
 * 文件名        : pid.c
 * 文件描述      ：PID控制算法
 * 创建时间      ：2019.11.9
 * 作者          ：刘文熠
 *-----------------------------------------------------------------------------
 * 最近修改时间  ：2021.5.8
 * 修改人        ：刘思宇
 * 内容          ：加入PI分离模式
 ******************************************************************************
 * 1.本代码基于STMF427IIT6开发，编译环境为Keil 5，基于FreeRTOS进行开发。
 * 2.本代码只适用于RoboMaster机器人，不建议用于其他用途
 * 3.本代码包含大量中文注释，请以UTF-8编码格式打开
 * 4.本代码最终解释权归哈尔滨工业大学（深圳）南工骁鹰战队Critical HIT所有
 *
 * Copyright (c) 哈尔滨工业大学（深圳）南工骁鹰战队Critical HIT 版权所有
 ******************************************************************************
 */

#include "pid.h"

#include "math.h"
#include "string.h"

/**
 * @brief 符号函数
 * @param input 输入
 * @retval 输入的符号，正负1或0
 */
float_t fsgn(float input) {
    return (input != 0.0f ? (input < 0.0f ? -1.0f : 1.0f) : 0.0f);
}

PID::PID() {
    PID(pos_pid_config(0, 0, 0, 0, 0));
}

PID::PID(const PID_config& _config) {
    memset(this, 0, sizeof(PID));
    config = _config;
}

void PID::reset() {
    PID_config temp = config;
    memset(this, 0, sizeof(PID));
    config = temp;
}

/**
 * @brief PID计算函数，位置式和增量式合在一起
 * @param PID结构体
 * @retval None
 */
void PID::calc() {
    error[2] = error[1];   //上上次误差
    error[1] = error[0];   //上次误差
    error[0] = ref - fdb;  //本次误差

    //位置式PID
    if (config.PID_Mode == PID_POSITION) {
        error_sum += error[0];  //积分上限判断
        if (error_sum > config.error_max) error_sum = config.error_max;
        if (error_sum < -config.error_max) error_sum = -config.error_max;

        output = config.KP * error[0] + config.KI * error_sum + config.KD * (error[0] - error[1]);
    }
    //增量式PID
    else if (config.PID_Mode == PID_DELTA) {
        output += config.KP * (error[0] - error[1]) + config.KI * (error[0] - 2.0f * error[1] + error[2]) + config.KI * error[0];
    }

    // PI分离模式 用于摩擦轮和拨弹
    else if (config.PID_Mode == PID_COMP_POSITION) {
        error_delta = error[0] - error[1];
        // bangbang
        if (error[0] > config.range_rough) {
            output = config.outputMax;
            error_sum = 0;
        } else if (error[0] < -config.range_rough) {
            output = -config.outputMax;
            error_sum = 0;
        } else if (fabsf(error[0]) < config.range_fine) {
            error_sum += error[0];  //积分上限判断
            if (error_sum > config.error_max) {
                error_sum = config.error_max;
            }
            if (error_sum < -config.error_max) {
                error_sum = -config.error_max;
            }
            output = config.KP_fine * error[0] + config.KI * error_sum + config.KD * error_delta;
        } else {
            output = config.KP * (error[0] + fsgn(error[0]) * config.compensation) + config.KD * error_delta;
            error_sum = fsgn(error[0]) * config.error_preload;
        }
    }

    /* 输出上限 */
    if (output > config.outputMax) output = config.outputMax;
    if (output < -config.outputMax) output = -config.outputMax;
}

PID::PID_config PID::pos_pid_config(float kp, float ki, float kd, float errormax, float outputmax) {
    PID_config config;
    config.KP = kp;
    config.KI = ki;
    config.KD = kd;
    config.error_max = errormax;
    config.error_preload = outputmax;
    config.PID_Mode = PID_POSITION;
    return config;
}

void PID::setConfig(PID_config _config) {
    config = _config;
}