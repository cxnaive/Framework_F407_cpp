#include <bsp_math.h>
#include <arm_math.h>

float bsp_sin(float x){
    return arm_sin_f32(x);
}
float bsp_cos(float x){
    return arm_cos_f32(x);
}