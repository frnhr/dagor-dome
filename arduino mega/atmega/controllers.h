

/*
 * Handles:
 *  - control pendant
 *  - door switch
 */



/***** INTERFACE *****/

// writes to input_buffer



/***** INTERNALS *****/

typedef struct {
    bool rotation_pressed;
    bool door_pressed;
    unsigned long last_failed_read_millis;  // for the pendant
} DoorInternals;
DoorInternals _controllers = {false, false, 0};



/***** PROTOTYPES *****/

void controllers_loop();

bool doorSwitchRead(int pin);
bool controllerRead(int pin);
