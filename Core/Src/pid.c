#include "pid.h"

#include "encoder.h"
#include "motor.h"

#include "mpu6050.h"
#include "inv_mpu.h"
#include "inv_mpu_dmp_motion_driver.h"

#include "main.h"

extern TIM_HandleTypeDef htim2;
extern TIM_HandleTypeDef htim4;

int duty; // sssssssssssssssssssssssss
int Encoder_L, Encoder_R;
float pitch, roll, yaw;
short Gyro_X,Gyro_Y,Gyro_Z;
short Acc_X,Acc_Y,Acc_Z;

float Med_Angle;

//PID parameters
float Vertical_KP,Vertical_KD;
float Velocity_KP,Velocity_KI;
float Turn_KP, Turn_KD;

int Vertical_Out, Velocity_Out, Turn_Out,Target_Speed,Target_Turn,DUTY_L,DUTY_R;

uint8_t stop_flag;


int Vertical_Loop(float Exception, float Angle, float Gyro_Y)
{
    int temp;
    temp = Vertical_KP * (Angle - Exception) + Vertical_KD * Gyro_Y;
    return temp;
}


int Velocity_Loop(int Target_Speed, int encoder_L, int encoder_R)
{
    static int Err_Lowout_Last, Err_S;
    static float a = 0.7;
    int Err, Err_Lowout, result;
    //erro 
    Err = (encoder_L + encoder_R) - Target_Speed;
    // lowout filter
    Err_Lowout = Err * (1 - a) + Err_Lowout_Last;
    Err_Lowout_Last = Err_Lowout;
    //calculate integral
    Err_S += Err_Lowout;
    //limit integral part
    Err_S = (Err_S > 20000) ? 20000 : (Err_S < -20000) ? -20000 : Err_S;
    //react to stop control signal
    if(stop_flag == 1)
    {
        Err_S = 0;
        stop_flag = 0;
    }
    //Velocity loop calculate
    result = Velocity_KP * Err_Lowout + Velocity_KI * Err_S; 
    
    return result;
}

int Turn_Loop(float Gyro_Z, int Target_turn)
{
    int result;
    result = Turn_KP * Target_turn + Turn_KD * Gyro_Z;
    return result;
}


int control()
{
    int PWM_out;
    //get real speed
    Encoder_L = Get_Speed(&htim2);
    Encoder_R = Get_Speed(&htim4);
    //
    mpu_dmp_get_data(&pitch, &roll, &yaw);
    MPU_Get_Gyroscope(&Gyro_X,&Gyro_Y,&Gyro_Z);
    MPU_Get_Accelerometer(&Acc_X,&Acc_Y,&Acc_Z);

    // transfer data to the pid loop 
    Velocity_Out = Velocity_Loop(Target_Speed, Encoder_L, Encoder_R);
    Vertical_Out = Vertical_Loop(Velocity_Out + Med_Angle, roll, Gyro_X);
    Turn_Out = Turn_Loop(Gyro_Z, Target_Turn);
    
    PWM_out = Vertical_Out;
    DUTY_L = PWM_out - Turn_Out;
    DUTY_R = PWM_out + Turn_Out;

    Duty_motor(DUTY_L,DUTY_R);
}
