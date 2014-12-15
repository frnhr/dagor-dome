
void encoder_loop()
{    
    //TODO move from storing azimuth to storing cycles in encoder_buffer

    // _q_encoder:
    // update _q_encoder.last_cycle and encoders.current_azimuth
    if (_q_encoder.last_cycle != _q_encoder.cycle && status_buffer.calibration == CALIBRATION_DONE)
    {
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
        if (abs(_q_encoder.cycle - _encoders.current_combination_start_cycles) < settings_buffer.switch_read_cycles) {
            _encoders.current_combination.one |= one;
            _encoders.current_combination.two |= two;
            _encoders.current_combination.three |= three;          
        } else {
            if (_encoders.read_combination) {
                encoders.last_combination = _encoders.current_combination;
                if (status_buffer.calibration == CALIBRATION_DONE) {
                    long diff = settings_buffer.switch_read_cycles;
                    if (abs(_q_encoder.cycle - _encoders.current_combination_start_cycles) < 0) {
                        // TODO check directions
                        diff *= -1;
                    }
                    FixedPosition * current_position = get_position(_encoders.current_combination);
                    encoders.current_azimuth = current_position->azimuth + diff / settings_buffer.cycles_for_degree;
                }
                _encoders.read_combination = false;
            } else {
                _encoders.current_combination_start_cycles = _q_encoder.cycle;
                _encoders.current_combination = EMPTY_COMBINATION;
                _encoders.read_combination = true;
            }
        }
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
