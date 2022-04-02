#include <app.h>
#include <chassis.h>
#include <chassis_board_cmd.h>
#include <gimbal.h>
#include <gimbal_board_cmd.h>
#include <robot_def.h>
#include <shoot.h>
#include <warpper.h>

#ifdef GIMBAL_BOARD
gimbal_board_cmd* cmd;
gimbal* gimbal_module;
shoot* shoot_module;
void APP_Layer_Init() {
    cmd = new gimbal_board_cmd();
    gimbal_module = new gimbal();
    shoot_module = new shoot();
}

void APP_Loop() {
    cmd->update();
    gimbal_module->update();
    shoot_module->update();
}
#endif

#ifdef CHASSIS_BOARD
chassis_board_cmd* cmd;
chassis* chassis_module;
void APP_Layer_Init() {
    cmd = new chassis_board_cmd();
    chassis_module = new chassis();
}

void APP_Loop() {
    cmd->update();
    chassis_module->update();
}
#endif