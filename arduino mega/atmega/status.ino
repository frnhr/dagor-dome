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
volatile StatusBuffer status_buffer = {STOP, NOOP, NULL_AZIMUTH, NULL_AZIMUTH, NOT_CALIBRATED};


void status_loop() {
    if(input_buffer.direction != NOOP) {
        cancel_calibration();
        status_buffer.target_azimuth = NULL_AZIMUTH;
        status_buffer.rotation = input_buffer.direction;
    }
    if(input_buffer.stop) {
        cancel_calibration();
        status_buffer.target_azimuth = NULL_AZIMUTH;
        status_buffer.rotation = STOP;
    }
    
    if(input_buffer.target_azimuth != NULL_AZIMUTH && status_buffer.calibation != CALIBRATION_IN_PROGRESS) {
        
        status_buffer.target_azimuth = input_buffer.target_azimuth;
        status_buffer.rotation = closer_route(status_buffer.current_azimuth, status_buffer.target_azimuth);
    }
    if(status_buffer.target_azimuth != NULL_AZIMUTH) {
        double correction = settings.drift;
        correction = (status_buffer.rotation == UP) ? correction : (-1 * correction);
        double target = map_to_circle(status_buffer.target_azimuth + correction);
        
        if(status_buffer.rotation != closer_route(status_buffer.current_azimuth, target)) {
            status_buffer.rotation = STOP;
            status_buffer.target_azimuth = NULL_AZIMUTH;
        }
    }
    
    status_buffer.doors = input_buffer.doors;
    
    if(input_buffer.start_calibration) {
        status_buffer.calibration = CALIBRATION_IN_PROGRESS;
    }
    
}

void cancel_calibration() {
    if(status_buffer.calibration == CALIBRATION_IN_PROGRESS) {
        status_buffer.calibration == NOT_CALIBRATED;
    }
}

int closer_route(double azimuth, double target_azimuth)
{    
    int distance = map_to_circle(target_azimuth - azimuth);
    
    if (distance < 180)
    {
        int delta = distance;
    else
    {
        int delta = abs(distance - 360);
    }
    
    if(delta < settings.deadzone_movement) {
        return STOP;
    }
    
    return (distance < 180) ? DOWN : UP;
}