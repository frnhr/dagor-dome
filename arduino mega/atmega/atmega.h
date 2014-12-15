//TODO get rid of global consts
const int NULL_AZIMUTH = 500;
const int NOOP = 0, DOWN = 1, UP = -1;



// order includes from general to specific

#include "Arduino.h"
#include "utils.h"

#include "status.h"
#include "encoders.h"
#include "motors.h"
#include "calibration.h"
#include "controllers.h"
#include "serial.h"
#include "door.h"

#include "settings.h"


/***** PINS *****/

int quadrature_L = 3; // encoder
int quadrature_H = 2;

int motor_up = 11;
int motor_down = 12;

int controller_up = 6; // controller pins
int controller_down = 7;
int controller_open = 5;
int controller_close = 4;

int door_switch_open = 25; // door switch direct control
int door_switch_close = 23;

int do_open_pin = 49; // control for door
int do_close_pin = 47;
int is_open_pin = 51; // unused
int is_closed_pin = 53; // unused

int switch_one = A4; // 1st mehanical switch
int switch_two = A5; // 2nd mehanical switch
int switch_three = A6; // 3rd mehanical switch

int led = 13;



/***** INTERFACES *****/

//TODO get rid of this
const int OPEN = 1, CLOSE = -1, DOORS_STOP = 3;


// input_buffer
typedef struct {
    int direction; // 0 nothing    1 down    -1 up        (right hand)
    int doors; // 0 nothing    1 open    -1 close
    double target_azimuth; // target azimuth, default >360
    bool start_calibration;
    bool stop;
} InputBuffer;
InputBuffer INPUT_BUFFER_DEFAULTS = {NOOP, NOOP, NULL_AZIMUTH, false, false};
InputBuffer input_buffer = INPUT_BUFFER_DEFAULTS;


// utility functions:

/*
 * Make sure argument is within [0, 360) range
 */
int map_to_circle(double azimuth) {
    while(azimuth < 0) {
        azimuth += 360;
    }
    while(azimuth >= 360) {
        azimuth -= 360;
    }
    return azimuth;
}


