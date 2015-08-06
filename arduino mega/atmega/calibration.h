
#define HOME_OFFSET 1
#define POSITIONS 2
#define DRIFT 3



/***** INTERFACE *****/

typedef struct {
    int stage; // 0 nothing      1 home offset       2 positions        3 drift
    unsigned long drift_start_time;
    MehanicalCombination current_combination;
    MehanicalCombination first_combination;
    int switch_counter;
} Calibration;
Calibration EMPTY_CALIBRATION = {NOOP, 0, EMPTY_COMBINATION, EMPTY_COMBINATION, 0};
Calibration calibration = EMPTY_CALIBRATION;


/***** PROTOTYPES *****/

void calibration_loop();

bool _serial_calibration_check();
