//pins:
int quadrature_L = 3; //encoder
int quadrature_H = 2;
int motor_up = 11;
int motor_down = 12;
int controller_up = 6; //controller pins
int controller_down = 7;
int controller_open = 4;
int controller_close = 5;
int door_switch_open = 23; //door switch direct control
int door_switch_close = 25;
int do_open_pin = 47; //control for door
int do_close_pin = 49;
int is_open_pin = 51; //unused
int is_closed_pin = 53; //unused
int switch_one = A4; //1st mehanical switch
int switch_two = A5; //2nd mehanical switch
int switch_three = A6; //3rd mehanical switch
int led = 13;

//constants:
const boolean DEBUG = true; //prints additional information over serial

//timers:
typedef struct
{
    unsigned long spinup_time,
        spindown_time,
        controller_last_failed_read_millis;
} Timers;
volatile Timers timers = {0, 0, 0};

//settings:
typedef struct
{
    double home_azimuth;
    double cycles_for_degree;   //how many encoder cycles for one degree - determinated by calibration
    int minimal_spinup_time,    //minimal time dome will move before any change
        minimal_spindown_time,  //minimal time after rotation is done to allow new rotation
        deadzone_movement,      //minimal movement allowed in degrees
        controller_plug_in_deadzone,    // controller will become responsive after this ammount of time
        minimal_spindown_drift_time,    //minimal time after rotation is done to measure drift
        switch_read_cycles;    //switch reading length
    double drift;               //measured drift in degrees
} Settings;
Settings settings = {0, 0, 1500, 2500, 5, 5000, 3000, 128, 0};

//serial communications:
typedef struct
{
    String data_received;
    boolean reading_complete;
} SerialComm;
SerialComm serial_comm = {"", false};


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

void debug(String str) {
    if(DEBUG)
        Serial.println(str);
}


void setup()
{
    Serial.begin(9600); //serial comm might not work correctly while dome is rotating (interrupts)

    //quadrature inputs (encoder):
    pinMode(quadrature_L, INPUT_PULLUP);
    digitalWrite(quadrature_L, HIGH);
    pinMode(quadrature_H, INPUT_PULLUP);
    digitalWrite(quadrature_H, HIGH);

    read_quadrature_edge(); // set initial value
    read_quadrature();
    attachInterrupt(1, update_QE, CHANGE);

    //led:
    pinMode(led, OUTPUT);

    // fixed position switches:
    pinMode(switch_one, INPUT);
    pinMode(switch_two, INPUT);
    pinMode(switch_three, INPUT);

    //motor control:
    pinMode(motor_up, OUTPUT);
    pinMode(motor_down, OUTPUT);

    //controller:
    pinMode(controller_up, INPUT_PULLUP);
    digitalWrite(controller_up, HIGH);
    pinMode(controller_down, INPUT_PULLUP);
    digitalWrite(controller_down, HIGH);
    pinMode(controller_open, INPUT_PULLUP);
    digitalWrite(controller_open, HIGH);
    pinMode(controller_close, INPUT_PULLUP);
    digitalWrite(controller_close, HIGH);

    // dome door direct control switch:
    pinMode(door_switch_open, INPUT_PULLUP);
    digitalWrite(door_switch_open, HIGH);
    pinMode(door_switch_close, INPUT_PULLUP);
    digitalWrite(door_switch_close, HIGH);

    //door open/close switch:
    pinMode(do_open_pin, OUTPUT);
    digitalWrite(do_open_pin, HIGH);
    pinMode(do_close_pin, OUTPUT);
    digitalWrite(do_close_pin, HIGH);

    settings.home_azimuth = EEPROM_read_home(); //read home from EEPROM

    if (DEBUG)
    {
        Serial.print("Home azimuth: ");
        Serial.println(dome.home_azimuth);
    }
}

void loop()
{
    serial_loop();
    
    controller_loop();
    
    status_loop();
    
    encoder_loop();
    
    calibration_loop();
}
