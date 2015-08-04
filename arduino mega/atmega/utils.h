

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


/*
 * Make sure argument is within [0, 360) range
 */
double map_to_circle(double azimuth) {
    while(azimuth < 0) {
        azimuth += 360;
    }
    while(azimuth >= 360) {
        azimuth -= 360;
    }
    return azimuth;
}
