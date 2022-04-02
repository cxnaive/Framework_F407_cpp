#ifndef _CHASSIS_BOARD_CMD_H_
#define _CHASSIS_BOARD_CMD_H_
#include <robot_def.h>
#include <stdint.h>
class chassis_board_cmd {
   private:
    Robot_mode robot_mode;
    cmd_chassis chassis_control;

   public:
    chassis_board_cmd();
    void update();
};

#endif