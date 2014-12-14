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
    if (status_buffer.doors == OPEN)
    {
        door_close();
    }
    else if (status_buffer.doors == CLOSE)
    {
        door_open();
    }
    else
    {
        door_clear();
    }
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
    digitalWrite(do_open_pin, HIGH);
    digitalWrite(do_close_pin, LOW);
}

/*
 * Give it a rest Sesame!
 */
void door_clear()
{
    digitalWrite(do_open_pin, LOW);
    digitalWrite(do_close_pin, LOW);
}
