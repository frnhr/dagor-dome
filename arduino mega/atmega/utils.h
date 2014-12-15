

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
    char buff[5]; 
    dtostrf(d, 4, 3, buff);  //4 is mininum width, 3 is precision; float value is copied onto buff
    return String(buff);
}

