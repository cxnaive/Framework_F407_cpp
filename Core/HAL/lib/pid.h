#ifndef PID_H_
#define PID_H_

#include <stdint.h>

class PID {
   public:
    enum PID_Mode_e {
        PID_POSITION = 0,
        PID_DELTA,
        PID_COMP_POSITION
    };
    struct PID_config {
        float KP;
        float KI;
        float KD;
        float KP_fine;
        float range_rough;
        float range_fine;
        float error_max;
        float outputMax;
        float compensation;
        float error_preload;
        enum PID_Mode_e PID_Mode;
    };

    PID_config config;
    float error[3];
    float error_sum;
    float fdb;
    float ref;
    float output;
    float error_delta;
    PID();
    PID(const PID_config& _config);
    void calc();
    void reset();
    void setConfig(PID_config _config);
    static PID_config pos_pid_config(float kp, float ki, float kd, float errormax, float outputmax);
};

// void PID_Init(struct PID_t* pid, struct PID_config_t* config);
// void PID_Calc(struct PID_t* pid);
// void PID_SetConfig(struct PID_config_t* obj, float kp, float ki, float kd, float errormax, float outputmax);
#endif
