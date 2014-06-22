// radio_functions.ino
// (included from atmega.ino)

/*
 * The radio loop.
 * Handles communication with the dome arduino C3 (a.k.a. "server") via intermediary Arduino C2.
 * Communication with C2 is performed in the simplest way, by develar dedicated lines ("pins"). Maybe will add SPI...
 *
 * Why "radio loop"? Because this Arduino used to perform functions that are now on C2, namely radio communication with C3.
 */
void radio_loop()
{
  // door switch has priority in any case
  if (doorSwitchRead(door_switch_close) || controllerRead(controller_close)) {
    door_close();
  }  else if (doorSwitchRead(door_switch_open) || controllerRead(controller_open)) {
    door_open();
  }  else {
    door_clear();
  }
}

/*
 * Simple negative-logic.
 */
int doorSwitchRead(int pin)
{
  return digitalRead(pin) == 0;
}


 
unsigned long controller_last_failed_read_millis = 0;
unsigned long controller_plug_in_deadzone = 5000;  // controller will become responsive after this ammount of time

/*
 * This function reads controller buttons.
 * It also detects when controller is disconnected (i.e. all pins are HIGH due to pullups), if so returns 0.
 */
int controllerRead(int pin) 
{
  if ((digitalRead(controller_close) && digitalRead(controller_open))
  || (digitalRead(controller_up) && digitalRead(controller_down))) {
    controller_last_failed_read_millis = millis();
    return 0;
  } else if (millis() - controller_last_failed_read_millis < controller_plug_in_deadzone) {
    return 0;
  }
  return digitalRead(pin);
}

/*
 * Open Sesame!
 */
void door_open()
{
  digitalWrite(do_open_pin, LOW);
  digitalWrite(do_close_pin, HIGH);
}

/*
 * Close Sesame!
 */
void door_close()
{
  digitalWrite(do_close_pin, LOW);
  digitalWrite(do_open_pin, HIGH);
}

/*
 * Give it a rest Sesame!
 */
void door_clear()
{
  digitalWrite(do_open_pin, LOW);
  digitalWrite(do_close_pin, LOW);
}
