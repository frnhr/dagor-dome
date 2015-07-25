// dome config
#define HOME_AZIMUTH 180.0

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
    unsigned long switch_read_cycles;    // switch reading length
    int deadzone_movement;      // minimal movement allowed in degrees
    double drift;               // measured drift in degrees
    int weak_switch_threshold;
} Settings;
Settings settings_buffer = {HOME_AZIMUTH, 0, MIN_SPINUP_TIME, MIN_SPINDOWN_TIME, 5000, 5000, 128, 5, 0, 4};





/***** PROTOTYPES *****/

double EEPROM_read_home();
void EEPROM_write_home(double value);