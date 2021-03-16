#include"Arduino.h"
#include"Cytron_Motor_Driver.h"
Cytron_Motor::Cytron_Motor(int DIR,int PWM_PIN,int PHASE_A, int PHASE_B, float ACCL_TIME,float DECL_TIME)
{
  dir=DIR;
  pwm=PWM_PIN;
  phase_A= PHASE_A;
  phase_B= PHASE_B;
  accl_time=ACCL_TIME;
  decl_time=DECL_TIME;
  start=0, min_pwm=0, max_pwm=0, time_start=0, inst_pwm=0, lastEncoded=0, enc_count=0;
  flag=0,time1=0,rpm=0,one_rot_count;
}
void Cytron_Motor::begin()
{
  pinMode(dir, OUTPUT);
  pinMode(pwm, OUTPUT);
  pinMode(phase_A,INPUT);
  pinMode(phase_B,INPUT);
}
void Cytron_Motor::runSimple(String rot, int pwm_val)
{
  if(pwm_val>255)
    pwm_val=255;
  if (rot == "CLOCK")
  {
    digitalWrite(dir, HIGH);
    analogWrite(pwm, pwm_val);
  }
  else if (rot == "ACLOCK")
  {
    digitalWrite(dir, LOW);
    analogWrite(pwm, pwm_val);
  }
  else if (rot == "STOP")
  {
    analogWrite(pwm, 0);
  }
}
void Cytron_Motor::run(int pwm_val)
{
    if(pwm_val<0)
    {
        runSimple("ACLOCK",-pwm_val);
    }
    else if(pwm_val>0)
    {
        runSimple("CLOCK",pwm_val);
    }
    else if(pwm_val==0)
    {
        runSimple("HSTOP",0);
    }
}
int Cytron_Motor::accelerate(int MIN_PWM,int MAX_PWM)
{
 min_pwm=MIN_PWM;
 max_pwm=MAX_PWM;
 if(max_pwm<0)
    min_pwm=-min_pwm;
 if(!start)
 {
    time_start=millis();
    start = 1;
 }
 if((millis()-time_start)<=accl_time)
 {
    inst_pwm=(((millis()-time_start)*(max_pwm-min_pwm))/accl_time)+min_pwm;
    return (inst_pwm);
 }
 else
 {
    inst_pwm=max_pwm;
    return(inst_pwm);
 }
}
int Cytron_Motor::decelerate(int MIN_PWM,int MAX_PWM)
{
 min_pwm=MIN_PWM;
 max_pwm=MAX_PWM;
 if(max_pwm<0)
    min_pwm=-min_pwm;
 if(start)
 {
    time_start=millis();
    start = 0;
 }
 if((millis()-time_start)<=decl_time)
 {
    inst_pwm=max_pwm+(((millis()-time_start)*(min_pwm-max_pwm))/decl_time);
    return(inst_pwm);
 }
 else
 {
    inst_pwm=min_pwm;
    return(inst_pwm);
 }
}
void Cytron_Motor::get_count()
{
    int MSB = digitalRead(phase_A);
    int LSB = digitalRead(phase_B);
    int encoded = (MSB << 1) | LSB;
    int sum  = (lastEncoded << 2) | encoded;
    if (sum == 0b1101 || sum == 0b0100 || sum == 0b0010 || sum == 0b1011)
    {
        enc_count =enc_count-1;
    }
    if (sum == 0b1110 || sum == 0b0111 || sum == 0b0001 || sum == 0b1000)
    {
        enc_count =enc_count+1;
    }
    lastEncoded = encoded;
}
float Cytron_Motor::get_rpm()
{
  if (flag)
  {
    time1 = millis();
    flag = 0;
  }
  if ((millis() - time1) >= 20)
  {
    rpm = ((enc_count * 3000.0) / one_rot_count);
    flag=1;
    enc_count=0;
  }
  return(rpm);
}
