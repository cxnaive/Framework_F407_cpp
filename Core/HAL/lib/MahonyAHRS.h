#ifndef _MahonyAHRS_h
#define _MahonyAHRS_h

class MahonyAHRS {
    float twoKp;
    float twoKi;
    float sampleFreq;
    float q0, q1, q2, q3;
    float integralFBx, integralFBy, integralFBz;

   public:
    float euler[3];
    MahonyAHRS(float _twoKp, float _twoKi,float _sampleFreq);
    void clear();
    void update(float gx, float gy, float gz, float ax, float ay, float az);
};

// void MahonyAHRS_init(MahonyAHRS *obj, float _twoKp, float _twoKi,
//                      float _sampleFreq);
// void MahonyAHRS_clear(MahonyAHRS *obj);
// void MahonyAHRS_update(MahonyAHRS *obj, float gx, float gy, float gz, float ax,
//                        float ay, float az);

#endif