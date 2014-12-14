unsigned long controller_last_failed_read_millis = 0;
boolean controller_direction = false;
boolean controller_doors = false;

void controller_loop() {
    boolean dome_up = controllerRead(controller_up);
    boolean dome_down = controllerRead(controller_down);
    boolean door_open = controllerRead(controller_open) || doorSwitchRead(door_switch_open);
    boolean door_close = controllerRead(controller_close) || doorSwitchRead(door_switch_close);
    if(door_open && door_close)
        door_open = false;

    if(dome_up || dome_down) {
        InputBuffer clean_input_buffer = INPUT_BUFFER_DEFAULTS;
        clean_input_buffer.doors = input_buffer.doors;
        input_buffer = clean_input_buffer;
        
        clean_input_buffer.direction = dome_up ? UP : DOWN;
        input_buffer = clean_input_buffer;
        controller_direction = true;
    } else if(controller_direction) {        
        input_buffer.stop = true;
        controller_direction = false;
    }
    
    if(door_open || door_close) {        
        input_buffer.doors = door_open ? OPEN : CLOSE;
        controller_doors = true;
    } else if(controller_doors) {
        input_buffer.doors = DOORS_STOP;
        controller_doors = false;
    }
}


/*
 * Simple negative-logic.
 */
int doorSwitchRead(int pin)
{
    return digitalRead(pin) == 0;
}

/*
 * This function reads controller buttons.
 * It also detects when controller is disconnected (i.e. all pins are HIGH due to pullups), if so returns 0.
 */
boolean controllerRead(int pin)
{
    if ((digitalRead(controller_close) && digitalRead(controller_open))
            || (digitalRead(controller_up) && digitalRead(controller_down)))
    {
        controller_last_failed_read_millis = millis();
        return LOW;
    }
    else if (millis() - controller_last_failed_read_millis < settings.controller_plug_in_deadzone)
    {
        return LOW;
    }
    return digitalRead(pin);
}
