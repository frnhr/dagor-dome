/******

Firmware if for "C1" controller, Arduino Mega 2560.


STAUS of this package:

 - refactoring in progress
 - not fully functional:
    - check calibration 
    - check goto
- some changes not tested on Arduino

******/



// prints additional information over serial
//TODO make this a define statement
const bool DEBUG = true; 

#include "atmega.h"


/*
 * setup Arduino pins, etc.
 */

void setup() 
{
    Serial.begin(9600); // serial comm might not work correctly while dome is rotating (interrupts)

    debugln("setup START");

    // quadrature inputs (encoder):
    pinMode(quadrature_L, INPUT_PULLUP);
    digitalWrite(quadrature_L, HIGH);
    pinMode(quadrature_H, INPUT_PULLUP);
    digitalWrite(quadrature_H, HIGH);

    read_quadrature_edge(); // set initial value
    read_quadrature();
    attachInterrupt(1, update_QE, CHANGE);

    // led:
    pinMode(led, OUTPUT);

    // fixed position switches:
    pinMode(switch_one, INPUT);
    pinMode(switch_two, INPUT);
    pinMode(switch_three, INPUT);

    // motor control:
    pinMode(motor_up, OUTPUT);
    pinMode(motor_down, OUTPUT);
    // controller:
    pinMode(controller_up, INPUT_PULLUP);
    digitalWrite(controller_up, HIGH);
    pinMode(controller_down, INPUT_PULLUP);
    digitalWrite(controller_down, HIGH);
    pinMode(controller_open, INPUT_PULLUP);
    digitalWrite(controller_open, HIGH);
    pinMode(controller_close, INPUT_PULLUP);
    digitalWrite(controller_close, HIGH);

    // dome door direct control switch:
    pinMode(door_switch_open, INPUT_PULLUP);
    digitalWrite(door_switch_open, HIGH);
    pinMode(door_switch_close, INPUT_PULLUP);
    digitalWrite(door_switch_close, HIGH);

    // door open/close switch:
    pinMode(do_open_pin, OUTPUT);
    digitalWrite(do_open_pin, HIGH);
    pinMode(do_close_pin, OUTPUT);
    digitalWrite(do_close_pin, HIGH);

    settings_buffer.home_azimuth = EEPROM_read_home(); // read home from EEPROM

    debug("Home azimuth: ");
    debugln(String(settings_buffer.home_azimuth));
    debugln("setup END");

} // end setup()


/*
 * The main loop.
 * Order of sub-loops is important.
 */
void loop()
{
    input_loop();

    sensors_loop();
    
    calibration_loop();
    
    status_loop();
    
    hardware_loop();
}


// SUB_LOOPS // 

/*
 * Read all inputs and populate input_buffer
 * NOTHING ELSE! :)
 */
void input_loop()
{
    input_buffer = INPUT_BUFFER_DEFAULTS;    
    serial_loop();
    controllers_loop();
}

/*
 * Read all sensors and populate sensor_buffer
 * NOTHING ELSE! :)
 */
 void sensors_loop()
 {
    encoder_loop();
 }

/*
 * Read from status_buffer and command hardware accordingly.
 * NOTHING ELSE! :)
 */
void hardware_loop()
{
    blink_led();
    motors_loop();    
    door_loop();
}


