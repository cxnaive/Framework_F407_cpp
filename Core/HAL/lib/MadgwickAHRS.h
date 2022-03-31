#ifndef _MadgwickAHRS_h
#define _MadgwickAHRS_h

class MadgwickAHRS {
    float beta;
    float sampleFreq;
    float q0, q1, q2, q3;

   public:
    float euler[3];
    MadgwickAHRS(float _beta, float _sampleFreq);
    void clear();
    void update(float gx, float gy, float gz, float ax, float ay, float az);
};

// void MadgwickAHRS_init(MadgwickAHRS *obj, float _beta, float _sampleFreq);
// void MadgwickAHRS_clear(MadgwickAHRS *obj);
// void MadgwickAHRS_update(MadgwickAHRS *obj, float gx, float gy, float gz,
//                          float ax, float ay, float az);

#endif