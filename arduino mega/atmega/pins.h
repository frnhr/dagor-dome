#include "Arduino.h"

int quadrature_L = 3; //encoder
int quadrature_H = 2;

int motor_up = 11;
int motor_down = 12;

int controller_up = 6; //controller pins
int controller_down = 7;
int controller_open = 5;
int controller_close = 4;

int door_switch_open = 25; //door switch direct control
int door_switch_close = 23;

int do_open_pin = 49; //control for door
int do_close_pin = 47;
int is_open_pin = 51; //unused
int is_closed_pin = 53; //unused

int switch_one = A4; //1st mehanical switch
int switch_two = A5; //2nd mehanical switch
int switch_three = A6; //3rd mehanical switch

int led = 13;
