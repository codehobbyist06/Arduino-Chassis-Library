#ifndef Hercules_Motor_h
#define Hercules_Motor_h
#include"Arduino.h"
class Hercules_Motor{
  private:
    unsigned int in1, in2, pwm;
    float min_pwm, max_pwm, start, time_start;
    float accl_time,decl_time,inst_pwm,rpm;
    volatile int lastEncoded;
    bool flag;
    long time1;
  public:
    unsigned int phase_A, phase_B,one_rot_count;
    volatile int enc_count;
    Hercules_Motor(int IN1, int PWM_PIN , int IN2, int phase_A, int phase_B, float ACCL_TIME, float DECL_TIME);
    void runSimple(String dir, int pwm_val);
    void run(int pwm_val);
    void begin();
    int accelerate(int MIN_PWM,int MAX_PWM);
    int decelerate(int MIN_PWM,int MAX_PWM);
    void get_count();
    float get_rpm();
};
#endif
