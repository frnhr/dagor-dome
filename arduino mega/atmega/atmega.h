

#define NULL_AZIMUTH 500
#define NOOP 0
#define DOWN 1
#define UP -1

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

const int quadrature_L = 3; // encoder
const int quadrature_H = 2;

const int motor_up = 11;
const int motor_down = 12;

const int controller_up = 6; // controller pins
const int controller_down = 7;
const int controller_open = 5;
const int controller_close = 4;

const int door_switch_open = 25; // door switch direct control
const int door_switch_close = 23;

const int do_open_pin = 49; // control for door
const int do_close_pin = 47;
const int is_open_pin = 51; // unused
const int is_closed_pin = 53; // unused

const int switch_one = A4; // 1st mehanical switch
const int switch_two = A5; // 2nd mehanical switch
const int switch_three = A7; // 3rd mehanical switch

const int led = 13;



/***** INTERFACES *****/

#define OPEN 1
#define CLOSE -1
#define DOORS_STOP 3


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

