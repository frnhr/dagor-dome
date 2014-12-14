#include "pins.h"

#include "definitions.h"

const boolean DEBUG = true; //prints additional information over serial
void debug(String str) {
    if(DEBUG)
        Serial.println(str);
}

void setup()
{
    Serial.begin(9600); //serial comm might not work correctly while dome is rotating (interrupts)

    //quadrature inputs (encoder):
    pinMode(quadrature_L, INPUT_PULLUP);
    digitalWrite(quadrature_L, HIGH);
    pinMode(quadrature_H, INPUT_PULLUP);
    digitalWrite(quadrature_H, HIGH);

    read_quadrature_edge(); // set initial value
    read_quadrature();
    attachInterrupt(1, update_QE, CHANGE);

    //led:
    pinMode(led, OUTPUT);

    // fixed position switches:
    pinMode(switch_one, INPUT);
    pinMode(switch_two, INPUT);
    pinMode(switch_three, INPUT);

    //motor control:
    pinMode(motor_up, OUTPUT);
    pinMode(motor_down, OUTPUT);

    //controller:
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

    //door open/close switch:
    pinMode(do_open_pin, OUTPUT);
    digitalWrite(do_open_pin, HIGH);
    pinMode(do_close_pin, OUTPUT);
    digitalWrite(do_close_pin, HIGH);

    settings.home_azimuth = EEPROM_read_home(); //read home from EEPROM

    if (DEBUG)
    {
        Serial.print("Home azimuth: ");
        Serial.println(settings.home_azimuth);
    }
}

void loop()
{
    serial_loop();
    
    controller_loop();
    
    encoder_loop();
    
    calibration_loop();
    
    status_loop();
    
    motor_loop();
}
