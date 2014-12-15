
void controllers_loop()
{
    bool dome_up = controllerRead(controller_up);
    bool dome_down = controllerRead(controller_down);
    bool door_open = controllerRead(controller_open) || doorSwitchRead(door_switch_open);
    bool door_close = controllerRead(controller_close) || doorSwitchRead(door_switch_close);

    /* if controller says open and switch says close (or vice versa) then close */
    if (door_open && door_close) {
        door_open = false;
    }

    /* rotate the donme */
    if (dome_up || dome_down) {
        InputBuffer clean_input_buffer = INPUT_BUFFER_DEFAULTS;
        clean_input_buffer.doors = input_buffer.doors;
        input_buffer = clean_input_buffer;
        
        clean_input_buffer.direction = dome_up ? UP : DOWN;
        input_buffer = clean_input_buffer;
        _controllers.rotation_pressed = true;
    } else if (_controllers.rotation_pressed) {        
        input_buffer.stop = true;
        _controllers.rotation_pressed = false;
    }
    
    /* open / close the door */
    if (door_open || door_close) {        
        input_buffer.doors = door_open ? OPEN : CLOSE;
        _controllers.door_pressed = true;
    } else if (_controllers.door_pressed) {
        input_buffer.doors = DOORS_STOP;
        _controllers.door_pressed = false;
    }
}


/*
 * Simple negative logic.
 */
bool doorSwitchRead(int pin)
{
    return digitalRead(pin) == LOW;
}


/*
 * This function reads controller buttons.
 * It also detects when controller is disconnected (i.e. all pins are HIGH due to pullups), if so returns 0.
 */
bool controllerRead(int pin)
{
    /* controller pins have internal pullups, so if they are all HIGH, it means controller is unplugged */
    bool disconnected = (digitalRead(controller_close) && digitalRead(controller_open))
                      || (digitalRead(controller_up) && digitalRead(controller_down));
    if (disconnected) {
        _controllers.last_failed_read_millis = millis();
        return LOW;
    } 

    /* wait for at least settings_buffer.controller_plug_in_timeout millis before allowing a read from the controller */
    /* this prevents random triggering when (un)plugging the controller */
    if (millis() - _controllers.last_failed_read_millis < settings_buffer.controller_plug_in_timeout) {
        return LOW;
    }

    /* read the pin... */
    return digitalRead(pin);
}
