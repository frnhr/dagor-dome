


void status_loop()
{
    status_buffer.current_azimuth = encoders.current_azimuth;

    if (input_buffer.direction != NOOP) {
        cancel_calibration();
        status_buffer.target_azimuth = NULL_AZIMUTH;
        status_buffer.rotation = input_buffer.direction;
    }
    if (input_buffer.stop) {
        cancel_calibration();
        status_buffer.target_azimuth = NULL_AZIMUTH;
        status_buffer.rotation = STOP;
    }
    
    if (input_buffer.target_azimuth != NULL_AZIMUTH && status_buffer.calibration != CALIBRATION_IN_PROGRESS) {
        
        status_buffer.target_azimuth = input_buffer.target_azimuth;
        status_buffer.rotation = closer_route(status_buffer.current_azimuth, status_buffer.target_azimuth);
    }
    if (status_buffer.target_azimuth != NULL_AZIMUTH) {
        double correction = settings_buffer.drift;
        correction = (status_buffer.rotation == UP) ? correction : (-1 * correction);
        double target = map_to_circle(status_buffer.target_azimuth + correction);
        
        if (status_buffer.rotation != closer_route(status_buffer.current_azimuth, target)) {
            status_buffer.rotation = STOP;
            status_buffer.target_azimuth = NULL_AZIMUTH;
        }
    }
    
    if (input_buffer.doors != NOOP) {
        if (input_buffer.doors == DOORS_STOP) {
            status_buffer.doors = STOP;
        } else {
            status_buffer.doors = input_buffer.doors;
        }
    }
    
    if (input_buffer.start_calibration) {
        status_buffer.calibration = CALIBRATION_IN_PROGRESS;
        status_buffer.rotation = DOWN;
        calibration = EMPTY_CALIBRATION;
    }
    
    if (calibration.stage == DRIFT) {
        status_buffer.rotation = STOP;
    }
}

void cancel_calibration()
{
    if (status_buffer.calibration == CALIBRATION_IN_PROGRESS) {
        status_buffer.calibration = NOT_CALIBRATED;
        calibration = EMPTY_CALIBRATION;
    }
}

int closer_route(double azimuth, double target_azimuth)
{    
    int distance = map_to_circle(target_azimuth - azimuth);
    int delta;
    
    if (distance < 180)     {
        delta = distance;
    } else {
        delta = abs(distance - 360);
    }
    
    if (delta < settings_buffer.deadzone_movement) {
        return STOP;
    }
    
    return (distance < 180) ? DOWN : UP;
}
