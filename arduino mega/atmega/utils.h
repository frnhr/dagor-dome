

void debug(String str)
{
    if (DEBUG) Serial.print(str);
}


void debugln(String str)     
{
    debug(str + "\n");
}


/*
 * Is an interupt currently active?
 */
bool in_interupt()
{
    return !(SREG / 128);
}


String to_string(double d) {
	return String(d);
}

