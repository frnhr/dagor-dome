
void encoder_loop()
{    
    // _q_encoder:
    // update _q_encoder.last_cycle and encoders.current_azimuth
    if ((_q_encoder.last_cycle != _q_encoder.cycle) && (status_buffer.calibration == CALIBRATION_DONE)) {
        encoders.current_azimuth += (_q_encoder.cycle - _q_encoder.last_cycle) / settings_buffer.cycles_for_degree;
        encoders.current_azimuth = map_to_circle(encoders.current_azimuth);
        _q_encoder.last_cycle = _q_encoder.cycle;
    }
    
    // FIXED_POSITIONS:
    // update encoders.last_combination and correct encoders.current_azimuth
    bool one = analogRead(switch_one) < 50;  // TODO treshold
    bool two = analogRead(switch_two) < 50;
    bool three = analogRead(switch_three) < 50;

    if (one || two || three) {
        //TODO warn is any screw is "too weak"
        debug(one ? " X " : "   ");
        debug(two ? " X " : "   ");
        debugln(three ? " X " : "   ");
    };

    if (_encoders.read_stage == NOOP) {
        if (one || two || three) {
            _encoders.read_stage = READING;
            _encoders.current_combination_start_cycles = _q_encoder.cycle;
            _encoders.current_combination = EMPTY_COMBINATION;
            _encoders.number_of_reads_1 = 0;
            _encoders.number_of_reads_2 = 0;
            _encoders.number_of_reads_3 = 0;
        }
    }

    if (_encoders.read_stage == READING) {
        _encoders.current_combination.one |= one;
        _encoders.current_combination.two |= two;
        _encoders.current_combination.three |= three;
        _encoders.number_of_reads_1 += (int) one;
        _encoders.number_of_reads_2 += (int) two;
        _encoders.number_of_reads_3 += (int) three;


        if (abs(_q_encoder.cycle - _encoders.current_combination_start_cycles) > settings_buffer.switch_read_cycles) {
            _encoders.read_stage = FINISHED;
        }
    }

    if (_encoders.read_stage == FINISHED) {
        encoders.last_combination = _encoders.current_combination;

        if (status_buffer.calibration == CALIBRATION_DONE) {
            long diff = _q_encoder.cycle - _encoders.current_combination_start_cycles;
            FixedPosition * current_position = get_position(_encoders.current_combination);
            encoders.current_azimuth = current_position->azimuth + diff / settings_buffer.cycles_for_degree;
        }

        if (_encoders.number_of_reads_1 && 
            _encoders.number_of_reads_1 <= settings_buffer.weak_switch_threshold) {
            debugln("WEAK SWITCH: 1");
            debugln(to_string(_encoders.current_combination));
        }
        if (_encoders.number_of_reads_2 && 
            _encoders.number_of_reads_2 <= settings_buffer.weak_switch_threshold) {
            debugln("WEAK SWITCH: 2");
            debugln(to_string(_encoders.current_combination));
        }
        if (_encoders.number_of_reads_3 &&
            _encoders.number_of_reads_3 <= settings_buffer.weak_switch_threshold) {
            debugln("WEAK SWITCH: 3");
            debugln(to_string(_encoders.current_combination));
        }

        _encoders.read_stage = NOOP;        
    }
}


// _q_encoder interrupts, reads, etc.

void update_QE()
{
    _q_encoder.cycle = count_quadrature(_q_encoder.cycle);
}

int count_quadrature(int value)
{
    if (in_interupt()) {
        _q_encoder.QE_code = read_quadrature_edge();
    } else {
        _q_encoder.QE_code = read_quadrature();
    }

    if (_q_encoder.QE_code == 1) {
        value++;
    }
    if (_q_encoder.QE_code == 2) {
        value--;
    }

    return value;
}

int read_quadrature()
{
    _q_encoder.QE_oldd = _q_encoder.QE_old;
    _q_encoder.QE_new = digitalRead(quadrature_H) * 2 + digitalRead(quadrature_L);
    _q_encoder.QE_code = _q_encoder.QE_matrix [_q_encoder.QE_old * 4 + _q_encoder.QE_new];
    _q_encoder.QE_old = _q_encoder.QE_new;
    return _q_encoder.QE_code;

}

int read_quadrature_edge()
{
    _q_encoder.QE_new = digitalRead(quadrature_L);
    if (_q_encoder.QE_new == _q_encoder.QE_old) {
        return 0;
    }
    _q_encoder.QE_old = _q_encoder.QE_new;
    if (_q_encoder.QE_new == HIGH) {
        return 0;
    }

    if (digitalRead(quadrature_H)) {
        return 1;
    } else {
        return 2;
    }
}
