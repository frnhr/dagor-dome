//pins:
int quadrature_L = 3; //encoder
int quadrature_H = 2;
int motor_up = 11;
int motor_down = 12;
int controller_up = 6; //controller pins
int controller_down = 7;
int controller_open = 4;
int controller_close = 5;
int open_pin = 23; //control for door
int close_pin = 25;
int is_open_pin = 27; //unused
int is_closed_pin = 29; //unused
int switch_one = A0; //1st mehanical switch
int switch_two = A1; //1st mehanical switch
int switch_three = A2; //1st mehanical switch

//constants:
const int start_time_min=1500, //minimal time dome will move when controller is pressed
          end_time_min=3500,   //minimal time after rotation is done to allow new rotation
          minimal_movement=5; //minimal movement allowed
const boolean DEBUG = true; //prints additional information over serial

//switches:
typedef struct { //all combinations are when dome is rotating right
  boolean one;
  boolean two;
  boolean three;
  double azimuth; //azimuth of dome when third screw is hit
  long cycles; //cycles between previous screws and this one... used only for calibration
} MehanicalCombination;
volatile MehanicalCombination screws1 = {false, false, false, 0, 0};
volatile MehanicalCombination screws2 = {false, false, false, 0, 0};
volatile MehanicalCombination screws3 = {false, false, false, 0, 0};
volatile MehanicalCombination screws4 = {false, false, false, 0, 0};

volatile MehanicalCombination current_screws = {false, false, false, 0, 0}; //used to detect current combination

//encoder:
typedef struct {
  long cycle;
  long last_cycle;
  
  int QE_old;
  int QE_oldd;
  int QE_new;
  int QE_code;
  int QE_matrix[16];
} Encoder;
volatile Encoder encoder = {0, 0, 0, 0, 0, 0, {0,2,1,3,0,0,3,0,0,3,0,0,3,0,0,0}};

//dome:
typedef struct {
  double cycles_for_degree; //how many encoder cycles for one degree - determinated by calibration
  double azimuth; //current azimuth
  double target_azimuth; //target azimuthswitch_one
  double home_azimuth;
  int route; //direction    0 nothing    1 down    -1 up        (right hand)
} DomeSettings;
volatile DomeSettings dome = {0, 0, 0, 0, 0};

//calibration:
typedef struct {
  boolean in_progress;
  boolean measuring_drift;
  long cycles;
  long drift_start_azimuth;
  long drift_start_time;
  double drift; //measured drift in degrees
} Calibration;
volatile Calibration calibration = {false, false, 0L, 0L, 0L, 0};

//serial communications:
typedef struct {
  String data_received;
  boolean reading_complete;
} SerialComm;
SerialComm serial_comm = {"", false};

void setup() {
  Serial.begin(115200); //serial comm might not work correctly while dome is rotating (interrupts)

  //quadrature inputs (encoder):
  pinMode(quadrature_L, INPUT_PULLUP);
  digitalWrite(quadrature_L, HIGH);
  pinMode(quadrature_H, INPUT_PULLUP);
  digitalWrite(quadrature_H, HIGH);

  read_quadrature_edge(); // set initial value
  read_quadrature();
  attachInterrupt(1, update_QE, CHANGE);
  
  //switches:
  pinMode(switch_one, INPUT);
  pinMode(switch_two, INPUT);
  pinMode(switch_three, INPUT);
  
  //motor control:
  pinMode(motor_up, OUTPUT);
  pinMode(motor_down, OUTPUT);
  
  //controller:
  pinMode(controller_up, INPUT);
  pinMode(controller_down, INPUT);
  pinMode(controller_open, INPUT);
  pinMode(controller_close, INPUT);
  
  //door open/close switch:
  pinMode(open_pin, OUTPUT);
  digitalWrite(open_pin, HIGH);
  pinMode(close_pin, OUTPUT);
  digitalWrite(close_pin, HIGH);

  dome.home_azimuth = EEPROM_read_home(); //read home from EEPROM
  
  if(DEBUG) {
    Serial.print("Home azimuth: ");
    Serial.println(dome.home_azimuth);
  }
}

void loop() {  
  encoder_loop();
  
  serial_loop();

  radio_loop();

  motor_loop();
}
