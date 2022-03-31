#ifndef _BUZZER_H
#define _BUZZER_H
#include <warpper.h>

#include <vector>

#include "bsp_pwm.h"

class buzzer {
   public:
    struct buzzer_config {
        BSP_PWM_Typedef* pwm_device;
        std::vector<uint16_t>* music;
    };

   private:
    static std::vector<buzzer*> buzzers;
    uint16_t finished;
    uint16_t count;
    buzzer_config config;
    bool is_run;
    void update();

   public:
    friend void Buzzer_Loop();
    static std::vector<uint16_t> buzzer_musics[7];
    buzzer(const buzzer_config& _config);
    ~buzzer();
    void start();
};

#endif