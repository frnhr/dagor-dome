
#define STOP 0
#define CALIBRATION_IN_PROGRESS 0
#define CALIBRATION_DONE 1
#define NOT_CALIBRATED -1




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


typedef struct {
	int blink_counter; 
} StatusInternal;
StatusInternal STATUS_INTERNAL_DEFAULTS = {1500};
StatusInternal _status_internals = STATUS_INTERNAL_DEFAULTS;


/***** PROTOTYPES *****/

void status_loop();

void cancel_calibration();
int closer_route(double azimuth, double target_azimuth);

void blink_led();
