#ifndef _OT_MOTOR_CONTROL_H_
#define _OT_MOTOR_CONTROL_H_

int motor_control_init(const char* spi_dev_path);

void motor_control(int yaw, int pitch, int* yaw_ticks, int* pitch_ticks);

void motor_control_off();

#endif