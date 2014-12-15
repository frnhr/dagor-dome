
/***** INTERNALS *****/

typedef struct {
    int rotation; // 0 stop    1 down    -1 up        (right hand)
    // timers:
    unsigned long spinup_time;
    unsigned long spindown_time;
} Motor;
Motor _motors = {0, 0, 0};


/***** INTERFACE *****/

// reads from status_buffer

// does not write 



/***** PROTOTYPES *****/

void motors_loop();
