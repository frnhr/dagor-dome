
// dome motors config
#define MIN_SPINUP_TIME 1500  // milliseconds
#define MIN_SPINDOWN_TIME 2500  // milliseconds

// controller pendant config
#define CONTROLLER_PLUG_IN_TIMEOUT 5000




/**** INTERFACES ****/


// writes to settings_buffer

typedef struct {
    double home_azimuth;
    double cycles_for_degree;   // how many encoder cycles for one degree - determinated by calibration
    unsigned long minimal_spinup_time;    // minimal time dome will move before any change
    unsigned long minimal_spindown_time;  // minimal time after rotation is done to allow new rotation
    unsigned long controller_plug_in_timeout;    // controller will become responsive after this ammount of time
    unsigned long spindown_drift_time;    // minimal time after rotation is done to measure drift
    long switch_read_cycles;    // switch reading length
    int deadzone_movement;      // minimal movement allowed in degrees
    double drift;               // measured drift in degrees
    int weak_switch_threshold;
} Settings;
Settings settings_buffer = {
    0,                  // home_azimuth
    0,                  // cycles_for_degree
    MIN_SPINUP_TIME,    // minimal_spinup_time
    MIN_SPINDOWN_TIME,  // minimal_spindown_time
    5000,               // controller_plug_in_timeout
    5000,               // spindown_drift_time
    15,                 // switch_read_cycles
    5,                  // deadzone_movement
    0,                  // drift
    4,                  // weak_switch_threshold
};





/***** PROTOTYPES *****/

void EEPROM_read_home();
void EEPROM_write_home();

void EEPROM_read_calibration();
void EEPROM_write_calibration();

void _EEPROM_write_double(double d, int addr);
double _EEPROM_read_double(int addr);
void _EEPROM_write_position(FixedPosition pos, int addr);
FixedPosition _EEPROM_read_position(int addr);


