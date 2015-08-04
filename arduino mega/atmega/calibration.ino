

void calibration_loop()
{
    if (status_buffer.calibration != CALIBRATION_IN_PROGRESS) {
        return;
    }
    

    if (calibration.stage == NOOP) {
        /* init the calibration procedure */
        _q_encoder.cycle = 0;
        calibration.stage = HOME_OFFSET;
        calibration.current_combination = EMPTY_COMBINATION;
        position1.combination = EMPTY_COMBINATION;
        position2.combination = EMPTY_COMBINATION;
        position3.combination = EMPTY_COMBINATION;
        position4.combination = EMPTY_COMBINATION;
    }
    
    if (encoders.last_combination != calibration.current_combination) {
        /* a fixed position was just hit */
        calibration.current_combination = encoders.last_combination;
        
        if (calibration.current_combination != calibration.first_combination) {
            // get next empty FixedPosition object
            FixedPosition *current_position = get_position(EMPTY_COMBINATION);
            // ... populate its combination
            current_position->combination = calibration.current_combination;
            //TODO here we break interfaces:
            current_position->cycles = _q_encoder.cycle - settings_buffer.switch_read_cycles;
        }
        
        if (calibration.stage == HOME_OFFSET) {
            calibration.first_combination = calibration.current_combination;
            calibration.stage = POSITIONS;

        } else if (calibration.stage == POSITIONS && 
                   calibration.current_combination == calibration.first_combination) {
            FixedPosition * first_position = get_position(calibration.first_combination);
            FixedPosition * last_position = get_position(encoders.last_combination);
            settings_buffer.cycles_for_degree = abs(( _q_encoder.cycle - first_position->cycles) / 360.0);

            _q_encoder.cycle = 0;
            calibration.stage = DRIFT;
            calibration.drift_start_time = millis();

            position1.azimuth = map_to_circle(settings_buffer.home_azimuth + (position1.cycles / settings_buffer.cycles_for_degree));
            position2.azimuth = map_to_circle(settings_buffer.home_azimuth + (position2.cycles / settings_buffer.cycles_for_degree));
            position3.azimuth = map_to_circle(settings_buffer.home_azimuth + (position3.cycles / settings_buffer.cycles_for_degree));
            position4.azimuth = map_to_circle(settings_buffer.home_azimuth + (position4.cycles / settings_buffer.cycles_for_degree));

            debugln("Switch azimuths:");
            debugln(String(position1.azimuth));
            debugln(String(position2.azimuth));
            debugln(String(position3.azimuth));
            debugln(String(position4.azimuth));
        }
    }
    
    if (calibration.stage == DRIFT && (millis() - calibration.drift_start_time > settings_buffer.spindown_drift_time)) {
        settings_buffer.drift = abs(_q_encoder.cycle / settings_buffer.cycles_for_degree);
        
        // TODO breaking interfaces!
        encoders.current_azimuth = position1.azimuth + settings_buffer.switch_read_cycles +settings_buffer.drift;

        debugln("DRIFT:");
        debugln(String(settings_buffer.drift));

        calibration = EMPTY_CALIBRATION;

        // TODO breaking interface!
        status_buffer.calibration = CALIBRATION_DONE;
    }
}