#include"Arduino.h"
#include"Hercules_Motor_Driver.h"
Hercules_Motor::Hercules_Motor(int IN1, int PWM_PIN, int IN2,int PHASE_A, int PHASE_B, float ACCL_TIME,float DECL_TIME)
{
  in1 = IN1;
  in2 = IN2;
  pwm = PWM_PIN;
  phase_A= PHASE_A;
  phase_B= PHASE_B;
  accl_time=ACCL_TIME;
  decl_time=DECL_TIME;
  start=0, min_pwm=0, max_pwm=0, time_start=0, inst_pwm=0, lastEncoded=0, enc_count=0;
  flag=0,time1=0,rpm=0,one_rot_count;
}
void Hercules_Motor::begin()
{
  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);
  pinMode(pwm, OUTPUT);
  pinMode(phase_A,INPUT);
  pinMode(phase_B,INPUT);
}
void Hercules_Motor::runSimple(String dir, int pwm_val)
{
  if (dir == "CLOCK")
  {
    digitalWrite(in1, HIGH);
    digitalWrite(in2, LOW);
    analogWrite(pwm, pwm_val);
  }
  else if (dir == "ACLOCK")
  {
    digitalWrite(in1, LOW);
    digitalWrite(in2, HIGH);
    analogWrite(pwm, pwm_val);
  }
  else if (dir == "STOP")
  {
    digitalWrite(in1, LOW);
    digitalWrite(in2, LOW);
    analogWrite(pwm, 0);
  }
  else if (dir == "HSTOP")
  {
    digitalWrite(in1, HIGH);
    digitalWrite(in2, HIGH);
    analogWrite(pwm, 0);
  }
}
void Hercules_Motor::run(int pwm_val)
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
int Hercules_Motor::accelerate(int MIN_PWM,int MAX_PWM)
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
int Hercules_Motor::decelerate(int MIN_PWM,int MAX_PWM)
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
void Hercules_Motor::get_count()
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
float Hercules_Motor::get_rpm()
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
