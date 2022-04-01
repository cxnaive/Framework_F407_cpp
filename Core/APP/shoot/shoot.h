#ifndef _SHOOT_H_
#define _SHOOT_H_
#include <can_motor.h>
#include <robot_def.h>
class shoot {
   private:
    can_motor friction_a;
    can_motor friction_b;
    can_motor load;
    cmd_shoot* cmd_data;
    //用于单/双/三发模式
    uint32_t cooldown_start;  //冷却起始时间点
    uint32_t cooldown_time;   //冷却时间
    void load_update();

   public:
    shoot();
    void update();
};

#endif