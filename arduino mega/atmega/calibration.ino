



void calibration_loop() 
{
    if (status_buffer.calibration != CALIBRATION_IN_PROGRESS) {
        return;
    }
    
    if(calibration.stage == NOOP) {
        q_encoder.cycle = 0;
        calibration.stage = HOME_OFFSET;
        calibration.current_combination = EMPTY_COMBINATION;
    }
    
    if (!(encoders.last_combination == calibration.current_combination)) {
        calibration.current_combination = encoders.last_combination;
        
        FixedPosition * current_position = get_position(EMPTY_COMBINATION);
        current_position->comb = calibration.current_combination;
        long diff = settings.switch_read_cycles;
        if (q_encoder.cycle - current_combination_start_cycles < 0) {
            // TODO check directions
            diff *= -1;
        }
        current_position->cycles = q_encoder.cycle + diff;
        
        if(calibration.stage == HOME_OFFSET) {
            calibration.first_combination = calibration.current_combination;
            calibration.stage = POSITIONS;
        } else if(calibration.stage == POSITIONS && calibration.current_combination == calibration.first_combination) {
            FixedPosition * first_position = get_position(calibration.first_combination);
            settings.cycles_for_degree = abs((q_encoder.cycle + diff - first_position->cycles) / 360);
            
            q_encoder.cycle = 0;
            calibration.stage = DRIFT;
            calibration.drift_start_time = millis();
            
            position1.azimuth = settings.home_azimuth + (position1.cycles / settings.cycles_for_degree);
            position2.azimuth = settings.home_azimuth + (position2.cycles / settings.cycles_for_degree);
            position3.azimuth = settings.home_azimuth + (position3.cycles / settings.cycles_for_degree);
            position4.azimuth = settings.home_azimuth + (position4.cycles / settings.cycles_for_degree);
        }
    }
    
    if(calibration.stage == DRIFT && (millis() - calibration.drift_start_time > settings.spindown_drift_time)) {
        settings.drift = abs(q_encoder.cycle / settings.cycles_for_degree);
        
        calibration = EMPTY_CALIBRATION;
        status_buffer.calibration = CALIBRATION_DONE;
    }
}
