#include "Arduino.h"

//settings:
typedef struct
{
    double home_azimuth;
    double cycles_for_degree;   //how many encoder cycles for one degree - determinated by calibration
    unsigned long minimal_spinup_time,    //minimal time dome will move before any change
        minimal_spindown_time,  //minimal time after rotation is done to allow new rotation
        controller_plug_in_deadzone,    // controller will become responsive after this ammount of time
        spindown_drift_time,    //minimal time after rotation is done to measure drift
        switch_read_cycles;    //switch reading length
    int deadzone_movement;      //minimal movement allowed in degrees
    double drift;               //measured drift in degrees
} Settings;
Settings settings = {0, 0, 1500, 2500, 5000, 5000, 128, 5, 0};



//input buffer:
const int NOOP = 0, DOWN = 1, UP = -1;
const int OPEN = 1, CLOSE = -1, DOORS_STOP = 3;
const int NULL_AZIMUTH = 500;
typedef struct
{
    int direction; //0 nothing    1 down    -1 up        (right hand)
    int doors; //0 nothing    1 open    -1 close
    double target_azimuth; //target azimuth, default >360
    bool start_calibration;
    bool stop;
} InputBuffer;
InputBuffer INPUT_BUFFER_DEFAULTS = {NOOP, NOOP, NULL_AZIMUTH, false, false};
InputBuffer input_buffer = INPUT_BUFFER_DEFAULTS;



//status buffer:
const int STOP = 0;
const int CALIBRATION_IN_PROGRESS = 0, CALIBRATION_DONE = 1, NOT_CALIBRATED = -1;
typedef struct
{
    int rotation; //0 stop    1 down    -1 up        (right hand)
    int doors; //0 nothing    1 open    -1 close
    double target_azimuth; //target azimuth, default >360
    double current_azimuth; //current azimuth
    int calibration; //0=in progress, 1=done, -1=not calibrated
} StatusBuffer;
StatusBuffer status_buffer = {STOP, NOOP, NULL_AZIMUTH, NULL_AZIMUTH, NOT_CALIBRATED};



//switches:
typedef struct
{
    boolean one;
    boolean two;
    boolean three;
} MehanicalCombination;
MehanicalCombination EMPTY_COMBINATION = {false, false, false};

boolean operator==(const MehanicalCombination& meh_comb_1, const MehanicalCombination& meh_comb_2)
{
    return meh_comb_1.one == meh_comb_2.one &&
           meh_comb_1.two == meh_comb_2.two &&
           meh_comb_1.three == meh_comb_2.three;
}

typedef struct
{
    MehanicalCombination comb;
    double azimuth;
    double cycles;
} FixedPosition;
FixedPosition EMPTY_POSITION = {EMPTY_COMBINATION, 0, 0};

//volatile FixedPosition[] position = {EMPTY_POSITION, EMPTY_POSITION, EMPTY_POSITION, EMPTY_POSITION};
FixedPosition position1 = EMPTY_POSITION;
FixedPosition position2 = EMPTY_POSITION;
FixedPosition position3 = EMPTY_POSITION;
FixedPosition position4 = EMPTY_POSITION;

MehanicalCombination current_combination = EMPTY_COMBINATION; //used to detect current combination



//encoder(s):
typedef struct
{
    double current_azimuth;
    
    MehanicalCombination last_combination;
    bool read_combination;
} Encoders;
Encoders encoders = {NULL_AZIMUTH, EMPTY_COMBINATION, false};

long current_combination_remaining_cycles;
long current_combination_start_cycles;

typedef struct
{
    long cycle;
    long last_cycle;
    int QE_old;
    int QE_oldd;
    int QE_new;
    int QE_code;
    int QE_matrix[16];
} QEncoder;
volatile QEncoder q_encoder = {0, 0, 0, 0, 0, 0, {0, 2, 1, 3, 0, 0, 3, 0, 0, 3, 0, 0, 3, 0, 0, 0}};



//calibration:
const int HOME_OFFSET = 1, POSITIONS = 2, DRIFT = 3;
typedef struct
{
    int stage; //0 nothing      1 home offset       2 positions        3 drift
    unsigned long drift_start_time;
    MehanicalCombination current_combination;
    MehanicalCombination first_combination;
} Calibration;
Calibration EMPTY_CALIBRATION = {NOOP, 0, EMPTY_COMBINATION, EMPTY_COMBINATION};
Calibration calibration = EMPTY_CALIBRATION;



//motors:
typedef struct
{
    int rotation; //0 stop    1 down    -1 up        (right hand)
    //timers:
    unsigned long spinup_time,
        spindown_time;
} Motor;
Motor motors = {0, 0, 0};



//serial communications:
typedef struct
{
    String data_received;
    boolean reading_complete;
} SerialComm;
SerialComm serial_comm = {"", false};



//utility functions:
int map_to_circle(double azimuth) {
    while(azimuth < 0)
    {
        azimuth += 360;
    }
    while(azimuth >= 360)
    {
        azimuth -= 360;
    }
    return azimuth;
}



//function prototypes:
double EEPROM_read_home();
void EEPROM_write_home(double value);
void calibration_loop();
int doorSwitchRead(int pin);
boolean controllerRead(int pin);
void controller_loop();
FixedPosition * get_position(MehanicalCombination comb);
void encoder_loop();
boolean in_interupt();
void update_QE();
int count_quadrature(int value);
int read_quadrature();
int read_quadrature_edge();
void motor_loop();
void radio_loop();
void door_open();
void door_close();
void door_clear();
void serial_loop();
boolean calibration_check();
void status_loop();
void cancel_calibration();
int closer_route(double azimuth, double target_azimuth);
