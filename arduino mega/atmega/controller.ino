unsigned long controller_last_failed_read_millis = 0;

void controller_loop() {
    boolean door_up = controllerRead(controller_up);
    boolean door_down = controllerRead(controller_down);
    boolean door_open = controllerRead(controller_open) || doorSwitchRead(door_switch_open);
    boolean door_close = controllerRead(controller_close) || doorSwitchRead(door_switch_close);
    if(door_open && door_close)
        door_open = false;
    
    if(door_up || door_down) {
        InputBuffer clean_input_buffer = INPUT_BUFFER_DEFAULTS;
        clean_input_buffer.doors = input_buffer.doors;
        input_buffer = clean_input_buffer;
        clean_input_buffer.direction = door_up ? UP : DOWN;
        input_buffer = clean_input_buffer;
    } else if(input_buffer.direction != NOOP || status_buffer.rotation != NOOP) {
        input_buffer.direction = NOOP;
    }
    
    if(door_open || door_close) {
        input_buffer.doors = door_open ? OPEN : CLOSE;
    } else if(input_buffer.doors != NOOP || status_buffer.doors != NOOP) {
        input_buffer.doors = NOOP;
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
