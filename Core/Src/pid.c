#include "pid.h"

#include "encoder.h"
#include "motor.h"

#include "mpu6050.h"
#include "inv_mpu.h"
#include "inv_mpu_dmp_motion_driver.h"

#include "main.h"
#include "oled.h"

#define VelocityKP -0.25

extern TIM_HandleTypeDef htim2;
extern TIM_HandleTypeDef htim4;

Balance_statemachine Balance_state;
Remote_Direction_GB Direction_G_B = Direction_G_B_DEFAULT;
Remote_Direction_LR Direction_L_R = Direction_L_R_DEFAULT;
Parameter_CONFIG_SM Parameter_Config_IDX;

int duty; // for test
int Encoder_L, Encoder_R;
float pitch, roll, yaw;
short Gyro_X, Gyro_Y, Gyro_Z;
short Acc_X, Acc_Y, Acc_Z;

float Med_Angle = 0;
float Med_Yaw = 0;

// PID parameters
float Vertical_KP = -350;       // -500*0.6
float Vertical_KD = -1.02;      //-1.7*0.6
float Velocity_KP = VelocityKP; //-0.3
float Velocity_KI = VelocityKP / 200;
float Turn_KP = 10, Turn_KD = 0;

int Vertical_Out, Velocity_Out, Turn_Out, Target_Speed, Target_Turn, DUTY_L, DUTY_R;
int Err_S;

uint8_t Parameter_IDX;

Parameter_CONFIG_TYPE PID_Parameter[Parameter_NUM] =
    {
        {&Med_Angle, Parameter_Free},
        {&Vertical_KP, Parameter_Free},
        {&Vertical_KD, Parameter_Free},
        {&Velocity_KP, Parameter_Free},
        {&Velocity_KI, Parameter_Free},
        {&Turn_KP, Parameter_Free},
        {&Turn_KD, Parameter_Free},
        {&Med_Yaw, Parameter_Free}};

Parameter_state IS_PARAMETER_SELECTED(Parameter_CONFIG_TYPE * parameter)
{
    if (parameter->state == Parameter_Selected)
    {
        return Parameter_Selected;
    }
    else
        return Parameter_Free;
}

FlagStatus Parameter_IDX_LOCKED(Parameter_CONFIG_TYPE *parameter)
{
    for (int i = 0; i < Parameter_NUM; i++)
    {
        if (parameter->state == Parameter_Selected)
            return SET;
        parameter++;
    }
    return RESET;
}

int Vertical_Loop(float Exception, float Angle, float Gyro_X)
{
    int result;
    result = Vertical_KP * (Angle - Exception) + Vertical_KD * Gyro_X;
    return result;
}

int Velocity_Loop(int Target_Speed, int encoder_L, int encoder_R)
{
    static int Err_Lowout_Last;
    static float a = 0.7;
    int Err, Err_Lowout, result;
    Err = (encoder_L + encoder_R) - Target_Speed;
    // lowout filter
    Err_Lowout = Err * (1 - a) + a * Err_Lowout_Last;
    Err_Lowout_Last = Err_Lowout;
    // calculate integral
    Err_S += Err_Lowout;
    // limit integral part
    Err_S = (Err_S > 20000) ? 20000 : (Err_S < (-20000) ? (-20000) : Err_S);
    // Velocity loop calculate
    result = Velocity_KP * Err_Lowout + Velocity_KI * Err_S;
    return result;
}

int Turn_Loop(float Gyro_Z, float Target_Yaw, float yaw)
{
    int result;
    result = Turn_KP * (Target_Yaw - yaw) + Turn_KD * Gyro_Z;
    return result;
}

int control()
{
    int PWM_out;
    // get real speed
    Encoder_L = Get_Speed(&htim2);
    Encoder_R = -Get_Speed(&htim4);
    mpu_dmp_get_data(&pitch, &roll, &yaw);
    MPU_Get_Gyroscope(&Gyro_X, &Gyro_Y, &Gyro_Z);
    MPU_Get_Accelerometer(&Acc_X, &Acc_Y, &Acc_Z);
    // transfer data to the pid loop
    if (Balance_state == Balance_running)
    {
        // if (Direction_G_B == Direction_GO)
        //     Target_Speed++;
        // else
        //     Target_Speed--;
        // Target_Speed = (Target_Speed > 15) ? 15 :(Target_Speed < -15) ? (-15) : Target_Speed;

        Velocity_Out = Velocity_Loop(Target_Speed, Encoder_L, Encoder_R);
        Vertical_Out = Vertical_Loop(Velocity_Out + Med_Angle, roll, Gyro_X);
        Turn_Out = Turn_Loop(Gyro_Z, Med_Yaw, yaw);
        PWM_out = Vertical_Out;
        //        PWM_out = Velocity_Out;
        DUTY_L = PWM_out - Turn_Out;
        DUTY_R = PWM_out + Turn_Out;
        Duty_motor(DUTY_L, DUTY_R);
    }
    else if (Balance_state == Balance_stop)
    {
        Duty_motor(0, 0); // stop
        Balance_state = Balance_idle;
        Err_S = 0;
    }
}


