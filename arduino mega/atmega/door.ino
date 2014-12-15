

void door_loop()
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
    digitalWrite(do_close_pin, LOW);
    digitalWrite(do_open_pin, HIGH);
}

/*
 * Close Sesame!
 */
void door_close()
{
    digitalWrite(do_open_pin, LOW);
    digitalWrite(do_close_pin, HIGH);
}

/*
 * Halt Sesame!
 */
void door_clear()
{
    digitalWrite(do_open_pin, LOW);
    digitalWrite(do_close_pin, LOW);
}
