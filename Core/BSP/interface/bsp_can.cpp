/**
 ******************************************************************************
 * 文件名          : bsp_can.c
 * 创建时间        : 2021.12.07
 * 作者            : 陈迅
 ******************************************************************************
 * 1.本代码基于STM32F407IGH6TR开发，编译环境为vscode，基于FreeRTOS进行开发
 * 2.本代码包含大量中文注释，请以UTF-8编码格式打开
 * 3.本代码最终解释权归哈尔滨工业大学（深圳）南工骁鹰战队Critical HIT所有
 *
 * Copyright (c) 哈尔滨工业大学（深圳）南工骁鹰战队Critical HIT 版权所有
 ******************************************************************************
 * 说明：
 ******************************************************************************
 */
#include "bsp_can.h"

#define DEVICE_CAN_CNT 2  //主控共使用（拥有）的can总线数量
#define ID_MAX 0x07FF
#define ID_NOTSET 0x800  //未使用过滤器时的填充值

void BSP_CanTypeDef::init(CAN_HandleTypeDef *_device, uint32_t _bank_prefix, uint8_t _id) {
    device = _device;
    bank_prefix = _bank_prefix;
    id = _id;
    call_backs.reserve(5);  //创建用于储存回调函数的向量组，存储方式为其头指针
    for (size_t i = 0; i < FILTER_MAX_CNT; ++i) {
        filters[i] = ID_NOTSET;
    }
    HAL_CAN_Start(device);
    HAL_CAN_ActivateNotification(device, CAN_IT_RX_FIFO0_MSG_PENDING);
    HAL_CAN_ActivateNotification(device, CAN_IT_RX_FIFO1_MSG_PENDING);
}

void BSP_CanTypeDef::send(uint16_t identifier, uint8_t *data, uint32_t len) {
    CAN_TxHeaderTypeDef txconf;
    txconf.StdId = identifier;
    txconf.IDE = CAN_ID_STD;
    txconf.RTR = CAN_RTR_DATA;
    txconf.DLC = len;
    while (HAL_CAN_GetTxMailboxesFreeLevel(device) == 0)
        ;
    HAL_CAN_AddTxMessage(device, &txconf, data, &tx_mailbox);
}

void BSP_CanTypeDef::add_filter(uint32_t filter) {
    for (size_t filter_index = 0; filter_index < FILTER_MAX_CNT; ++filter_index) {
        if (filters[filter_index] == ID_NOTSET) {
            filters[filter_index] = filter;
            update_filter(filter_index);
            return;
        }
    }
}

void BSP_CanTypeDef::remove_filter(uint32_t filter) {
    for (size_t filter_index = 0; filter_index < FILTER_MAX_CNT; ++filter_index) {
        if (filters[filter_index] == filter) {
            filters[filter_index] = ID_NOTSET;
            update_filter(filter_index);
        }
    }
}

void BSP_CanTypeDef::update_filter(uint32_t filter_index) {
    uint32_t group_index = filter_index / 4;
    CAN_FilterTypeDef tmp;
    tmp.FilterMode = CAN_FILTERMODE_IDLIST;
    tmp.FilterScale = CAN_FILTERSCALE_16BIT;
    tmp.FilterFIFOAssignment = (group_index & 1) ? CAN_RX_FIFO0 : CAN_RX_FIFO1;  // 4个一组，按照组别设置FIFO，平衡负载
    tmp.SlaveStartFilterBank = 14;
    tmp.FilterIdLow = filters[group_index * 4] << 5;
    tmp.FilterIdHigh = filters[group_index * 4 + 1] << 5;
    tmp.FilterMaskIdLow = filters[group_index * 4 + 2] << 5;
    tmp.FilterMaskIdHigh = filters[group_index * 4 + 3] << 5;
    tmp.FilterBank = group_index + bank_prefix;
    uint8_t id_all_notset = 1;
    for (size_t i = 0; i < 4; ++i) {
        if (filters[group_index * 4 + i] != ID_NOTSET) id_all_notset = 0;
    }
    if (id_all_notset)
        tmp.FilterActivation = CAN_FILTER_DISABLE;
    else
        tmp.FilterActivation = CAN_FILTER_ENABLE;
    HAL_CAN_ConfigFilter(device, &tmp);
}

void BSP_CanTypeDef::register_rx_callback(void *identifier, can_rx_callback func) {
    call_backs.insert({identifier, func});
}

void BSP_CanTypeDef::unregister_rx_callback(void *identifier) {
    call_backs.erase(identifier);
}

BSP_CanTypeDef BSP_CanTypeDef::can_devices[DEVICE_CAN_CNT];  //定义对应数量的can外设，即代码中定义can总线

void BSP_CAN_Init() {
    BSP_CanTypeDef::can_devices[0].init(&hcan1, 0, 0);
    BSP_CanTypeDef::can_devices[1].init(&hcan2, 14, 1);
}

void BSP_CAN_FifoMsg_Callback(CAN_HandleTypeDef *hcan, uint32_t fifo) {
    for (int i = 0; i < DEVICE_CAN_CNT; ++i) {
        if (hcan == BSP_CanTypeDef::can_devices[i].device) {
            uint8_t bsp_can_rxbuf[8];
            CAN_RxHeaderTypeDef rxconf;
            HAL_CAN_GetRxMessage(hcan, fifo, &rxconf, bsp_can_rxbuf);
            for (auto &func_pair : BSP_CanTypeDef::can_devices[i].call_backs) {
                auto &func = func_pair.second;
                if (func) {
                    func(&BSP_CanTypeDef::can_devices[i], rxconf.StdId, bsp_can_rxbuf, rxconf.DLC);
                }
            }
        }
    }
}

void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan) {
    BSP_CAN_FifoMsg_Callback(hcan, CAN_RX_FIFO0);
}

void HAL_CAN_RxFifo1MsgPendingCallback(CAN_HandleTypeDef *hcan) {
    BSP_CAN_FifoMsg_Callback(hcan, CAN_RX_FIFO1);
}