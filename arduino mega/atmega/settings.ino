#include <EEPROM.h>

/*
0-4     home azimuth
4-8     cycles for degree
8-12    drift
12-23   position 1
23-34   position 2
34-45   position 3
45-56   position 4
*/


// HOME POSITION

void EEPROM_read_home()
{
    settings_buffer.home_azimuth = _EEPROM_read_double(0);
}

void EEPROM_write_home()
{
    _EEPROM_write_double(settings_buffer.home_azimuth, 0);
}


// CALIBRATION

void EEPROM_read_calibration()
{
    settings_buffer.cycles_for_degree = _EEPROM_read_double(4);
    settings_buffer.drift = _EEPROM_read_double(8);
    position1 = _EEPROM_read_position(12);
    position2 = _EEPROM_read_position(23);
    position3 = _EEPROM_read_position(34);
    position4 = _EEPROM_read_position(45);

    // TODO breaking interface!
    calibration = EMPTY_CALIBRATION;
    status_buffer.calibration = CALIBRATION_DONE;
}

void EEPROM_write_calibration()
{
    _EEPROM_write_double(settings_buffer.cycles_for_degree, 4);
    _EEPROM_write_double(settings_buffer.drift, 8);
    _EEPROM_write_position(position1, 12);
    _EEPROM_write_position(position2, 23);
    _EEPROM_write_position(position3, 34);
    _EEPROM_write_position(position4, 45);
}


// INTERNALS

void _EEPROM_write_double(double d, int addr)
{
    EEPROM.put(addr, d);
}

double _EEPROM_read_double(int addr)
{
    double d = 0.0;
    EEPROM.get(addr, d);

    return d;
}

void _EEPROM_write_position(FixedPosition pos, int addr)
{
    EEPROM.put(addr, pos);
}

FixedPosition _EEPROM_read_position(int addr)
{
    FixedPosition pos;
    EEPROM.get(addr, pos);

    return pos;
}


