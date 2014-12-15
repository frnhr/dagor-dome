#include <EEPROM.h>

//TODO home_position should not be in EEPROM, but in settings.h instead, as a text config

//TODO settings (or calibration?)  should be written to EEPROM and restored on restart


double EEPROM_read_home()
{
    String buffer = "";
    for (int i = 0; i < 4; i++)
    {
        buffer += EEPROM.read(i);
    }

    char buf[buffer.length()];
    buffer.toCharArray(buf, buffer.length());
    return (double)atof(buf);
}

void EEPROM_write_home(double value)
{
    int values[4];
    values[0] = (int)(value / 100);
    values[1] = (int)((value / 10) - (values[0] * 10));
    values[2] = (int)(value - (values[0] * 100) - (values[1] * 10));
    values[3] = (int)((value * 10) - (values[0] * 1000) - (values[1] * 100) - (values[2] * 10));

    for (int i = 0; i < 4; i++)
    {
        EEPROM.write(i, values[i]);
    }

    settings_buffer.home_azimuth = value;
}
