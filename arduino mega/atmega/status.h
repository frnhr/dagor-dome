

//TODO get rid of these
const int STOP = 0;
const int CALIBRATION_IN_PROGRESS = 0, CALIBRATION_DONE = 1, NOT_CALIBRATED = -1;




/***** INTERFACE *****/

// reads from input_buffer

// writes to status_buffer: 
typedef struct {
    int rotation; // 0 stop    1 down    -1 up        (right hand)
    int doors; // 0 nothing    1 open    -1 close
    double target_azimuth; // target azimuth, default >360
    double current_azimuth; // current azimuth
    int calibration; // 0=in progress, 1=done, -1=not calibrated
} StatusBuffer;
StatusBuffer status_buffer = {STOP, NOOP, NULL_AZIMUTH, NULL_AZIMUTH, NOT_CALIBRATED};



/***** PROTOTYPES *****/

void status_loop();

void cancel_calibration();
int closer_route(double azimuth, double target_azimuth);
