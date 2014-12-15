
/*
 * Responds to communication via serial port.
 * See serial_protocol.txt, dome.py
 */



/***** INTERFACE *****/

// does not read

// writes to input_buffer



/***** INTERNALS *****/

typedef struct {
    String data_received;
    bool reading_complete;
} SerialComm;
SerialComm _serial = {"", false};



/***** PROTOTYPES *****/

void serial_loop();
