#include "can_motor.h"

#include "bsp_can.h"
#include "bsp_log.h"
#include "stdlib.h"
#include "string.h"

#define VELOCITY_WINDOW 5
#define ERROR_ID_INVALID 1
#define ERROR_ID_CONFLICT 2
can_motor::can_motor_registry::can_motor_registry() {
    reset();
}

void can_motor::can_motor_registry::reset() {
    memset(this, 0, sizeof(can_motor_registry));
}

uint32_t can_motor::can_motor_registry::register_motor(can_motor* motor) {
    uint32_t register_error = 0;
    uint32_t rx_id = 0;
    uint8_t motor_id = motor->config.motor_set_id;
    Motor_Model_e model = motor->config.motor_model;
    if (motor_id < 1 || motor_id > 8) register_error = ERROR_ID_INVALID;
    if (motor_id_registry[model][motor_id] != NULL) register_error = ERROR_ID_CONFLICT;
    if (!register_error) {
        switch (model) {
            case MODEL_2006:
                if (motor_id < 5) {
                    if (identifier_registry[0][motor_id - 1] == NULL)
                        identifier_registry[0][motor_id - 1] = motor;
                    else
                        register_error = ERROR_ID_CONFLICT;
                } else {
                    if (identifier_registry[1][motor_id - 5] == NULL)
                        identifier_registry[1][motor_id - 5] = motor;
                    else
                        register_error = ERROR_ID_CONFLICT;
                }
                rx_id = 0x200 + motor_id;
                break;
            case MODEL_3508:
                if (motor_id < 5) {
                    if (identifier_registry[0][motor_id - 1] == NULL)
                        identifier_registry[0][motor_id - 1] = motor;
                    else
                        register_error = ERROR_ID_CONFLICT;
                } else {
                    if (identifier_registry[1][motor_id - 5] == NULL)
                        identifier_registry[1][motor_id - 5] = motor;
                    else
                        register_error = ERROR_ID_CONFLICT;
                }
                rx_id = 0x200 + motor_id;
                break;
            case MODEL_6020:
                if (motor_id < 5) {
                    if (identifier_registry[1][motor_id - 1] == NULL)
                        identifier_registry[1][motor_id - 1] = motor;
                    else
                        register_error = ERROR_ID_CONFLICT;
                } else {
                    if (identifier_registry[2][motor_id - 5] == NULL)
                        identifier_registry[2][motor_id - 5] = motor;
                    else
                        register_error = ERROR_ID_CONFLICT;
                }
                rx_id = 0x204 + motor_id;
                break;
            default:
                register_error = ERROR_ID_INVALID;
                break;
        }
    }
    if (register_error == ERROR_ID_INVALID) {
        printf_log("model %d:id %d Invalid Motor ID!\n", model, motor_id);
        return register_error;
    } else if (register_error == ERROR_ID_CONFLICT) {
        printf_log("model %d:id %d Motor ID Conflict!\n", model, motor_id);
        return register_error;
    } else {
        motor_id_registry[model][motor_id] = motor;
        return rx_id;
    }
}

can_motor::can_motor_registry can_motor::can_reg[DEVICE_CAN_CNT];

can_motor::can_motor(const can_motor_config& _config) : monitor_item(_config.lost_callback, 5, this) {
    //初始化
    config = _config;
    motor_enable = MOTOR_STOP;
    fdbPosition = last_fdbPosition = fdbSpeed = electric_current = round = current_output = 0;
    position_sum = 0;
    temperature = 0;
    real_position = last_real_position = line_speed = velocity = 0;
    speed_pid.setConfig(_config.config_speed);
    position_pid.setConfig(_config.config_position);
    motor_rx_id = can_reg[config.can_device->id].register_motor(this);
    //注册回调函数
    BSP_CanTypeDef::can_rx_callback now_callback = std::bind(&can_motor::motor_rx_callback, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4);
    config.can_device->register_rx_callback(this, now_callback);
    config.can_device->add_filter(motor_rx_id);
}

can_motor::~can_motor() {
    config.can_device->unregister_rx_callback(this);
    config.can_device->remove_filter(motor_rx_id);
}

void can_motor::motor_rx_callback(BSP_CanTypeDef* device, uint32_t identifier, uint8_t* data, uint32_t len) {
    if (device == config.can_device && identifier == motor_rx_id) {
        motor_feedback_update(data);
    }
}

void can_motor::motor_feedback_update(uint8_t* data) {
    last_fdbPosition = fdbPosition;
    fdbPosition = ((short)data[0]) << 8 | data[1];
    fdbSpeed = ((short)data[2]) << 8 | data[3];
    electric_current = ((short)data[4]) << 8 | data[5];
    if (config.motor_model == MODEL_2006) {
        temperature = 0;
    } else {
        temperature = data[6];
    }
    if (fdbPosition - last_fdbPosition > 4096)
        round--;
    else if (fdbPosition - last_fdbPosition < -4096)
        round++;
    last_real_position = real_position;
    real_position = fdbPosition + round * 8192;

    float position_delta = real_position - last_real_position;
    if (pos_delta_queue.size() == VELOCITY_WINDOW) {
        float now = pos_delta_queue.front();
        pos_delta_queue.pop();
        position_sum -= now;
    }
    pos_delta_queue.push(position_delta);
    position_sum += position_delta;
    float vnow = position_sum * 43.9453125 / pos_delta_queue.size();
    velocity = 0.2f * velocity + 0.8f * vnow;
    monitor_item.reset();
}

void can_motor::can_send(uint8_t can_index, uint16_t identifier, short motor_data_id1, short motor_data_id2, short motor_data_id3, short motor_data_id4) {
    static uint8_t data[8];
    data[0] = (uint8_t)(motor_data_id1 >> 8);
    data[1] = (uint8_t)(motor_data_id1 & 0x00FF);
    data[2] = (uint8_t)(motor_data_id2 >> 8);
    data[3] = (uint8_t)(motor_data_id2 & 0x00FF);
    data[4] = (uint8_t)(motor_data_id3 >> 8);
    data[5] = (uint8_t)(motor_data_id3 & 0x00FF);
    //对于标识符为0x2FF的情况，则data[6]和data[7]为NULL
    if (identifier == 0x200 || identifier == 0x1FF) {
        data[6] = (uint8_t)((motor_data_id4 & 0xFF00) >> 8);
        data[7] = (uint8_t)(motor_data_id4 & 0x00FF);
    }
    BSP_CanTypeDef::can_devices[can_index].send(identifier, data, 8);
}

//PID计算
void Motor_Loop() {
    for (size_t can_index = 0; can_index < DEVICE_CAN_CNT; ++can_index) {
        for (size_t identifier = 0; identifier < 3; ++identifier) {
            uint8_t identifier_send = 0;
            short buf[4] = {0};
            for (size_t id = 0; id < 4; ++id) {
                can_motor* obj = can_motor::can_reg[can_index].identifier_registry[identifier][id];
                if (obj == NULL) continue;
                identifier_send = 1;
                if (obj->config.motor_pid_model == can_motor::POSITION_LOOP) {
                    if (obj->config.position_fdb_model == can_motor::OTHER_FDB) {
                        obj->position_pid.fdb = *obj->config.position_pid_fdb;
                    }
                    if (obj->config.position_fdb_model == can_motor::MOTOR_FDB) {
                        obj->position_pid.fdb = obj->real_position;
                    }
                    obj->position_pid.calc();
                    obj->speed_pid.ref = obj->position_pid.output;
                }
                if (obj->config.motor_pid_model >= can_motor::SPEED_LOOP) {
                    if (obj->config.speed_feedforward != NULL) {
                        obj->speed_pid.ref += *obj->config.speed_feedforward;
                    }
                    if (obj->config.speed_fdb_model == can_motor::OTHER_FDB) {
                        obj->speed_pid.fdb = *obj->config.speed_pid_fdb;
                    }
                    if (obj->config.speed_fdb_model == can_motor::MOTOR_FDB) {
                        obj->speed_pid.fdb = obj->velocity;
                    }
                    obj->speed_pid.calc();
                    obj->current_output = obj->speed_pid.output;
                }
                if (obj->config.current_feedforward != NULL) {
                    obj->current_output += *obj->config.current_feedforward;
                }
                buf[id] = obj->current_output;
                if (obj->config.output_model == can_motor::MOTOR_OUTPUT_REVERSE) {
                    buf[id] *= -1;
                }
                if (obj->motor_enable == can_motor::MOTOR_STOP) {
                    buf[id] = 0;
                }
            }
            // 如果此标识符(identifier)对应的四个电机里至少有一个被注册，就发送这个标识符的报文，如果全部没有被注册，则这个标识符无需发送
            if (identifier_send) {
                can_motor::can_send(can_index, can_motor::identifiers[identifier], buf[0], buf[1], buf[2], buf[3]);
            }
        }
    }
}

void can_motor::stop(){
    motor_enable = MOTOR_STOP;
}

void can_motor::enable(){
    motor_enable = MOTOR_ENABLE;
}