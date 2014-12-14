void controller_loop() {
    bool up = controllerRead(controller_up);
    bool down = controllerRead(controller_down);
    bool open = controllerRead(controller_open) || digitalRead(door_switch_open);
    bool close = controllerRead(controller_close) || digitalRead(door_switch_close);
    if(open && close)
        open = false;
    
    if(up || down) {
        InputBuffer clean_input_buffer = INPUT_BUFFER_DEFAULTS;
        clean_input_buffer.doors = input_buffer.doors;
        input_buffer = clean_input_buffer;
        clean_input_buffer.direction = up ? UP : DOWN;
        input_buffer = clean_input_buffer;
    }
    if(open || close) {
        input_buffer.doors = open ? OPEN : CLOSE;
    }
}


/*
 * This function reads controller buttons.
 * It also detects when controller is disconnected (i.e. all pins are HIGH due to pullups), if so returns 0.
 */
bool controllerRead(int pin)
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