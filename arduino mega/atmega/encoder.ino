FixedPosition * get_position(MehanicalCombination comb) {
    if (comb == position1.comb) {
        return &position1;
    }
    if (comb == position2.comb) {
        return &position2;
    }
    if (comb == position3.comb) {
        return &position3;
    }
    if (comb == position4.comb) {
        return &position4;
    }
    return &EMPTY_POSITION;
}


void encoder_loop()
{
    // Q_ENCODER:
    // update q_encoder.last_cycle and encoders.current_azimuth
    if (q_encoder.last_cycle != q_encoder.cycle && status_buffer.calibration == CALIBRATION_DONE)
    {
        encoders.current_azimuth += (q_encoder.cycle - q_encoder.last_cycle) / settings.cycles_for_degree;
        encoders.current_azimuth = map_to_circle(encoders.current_azimuth);

        q_encoder.last_cycle = q_encoder.cycle;
    }
    
    // FIXED_POSITIONS:
    // update encoders.last_combination and correct encoders.current_azimuth
    bool one = analogRead(switch_one) == 0;  // TODO treshold
    bool two = analogRead(switch_one) == 0;
    bool three = analogRead(switch_one) == 0;
    if (one || two || three) {
        if (current_combination_remaining_cycles == -1) {
            current_combination_remaining_cycles = settings.switch_read_cycles;
            current_combination_start_cycles = q_encoder.cycle;
            current_combination = EMPTY_COMBINATION;
        }
        if (current_combination_remaining_cycles > 0) {
            current_combination.one |= one;
            current_combination.two |= two;
            current_combination.three |= three;
            current_combination_remaining_cycles--;
        }
    }
    // current_combination_remaining_cycles == 0 means that screws are ready to be read
    if (current_combination_remaining_cycles == 0) {
        encoders.last_combination = current_combination;
        current_combination_remaining_cycles = -1;
        
        if (status_buffer.calibration == CALIBRATION_DONE) {
            long diff = settings.switch_read_cycles;
            if (q_encoder.cycle - current_combination_start_cycles < 0) {
                // TODO check directions
                diff *= -1;
            }
            FixedPosition * current_position = get_position(current_combination);
            encoders.current_azimuth = current_position->azimuth + diff / settings.cycles_for_degree;
        }
    }
}


// Q_ENCODER interrupts, reads, etc.

boolean in_interupt()
{
    return !(SREG / 128);
}

void update_QE()
{
    q_encoder.cycle = count_quadrature(q_encoder.cycle);
}

int count_quadrature(int value)
{
    if (in_interupt())
    {
        q_encoder.QE_code = read_quadrature_edge();
    }
    else
    {
        q_encoder.QE_code = read_quadrature();
    }

    if (q_encoder.QE_code == 1)
    {
        value++;
    }
    if (q_encoder.QE_code == 2)
    {
        value--;
    }

    return value;
}

int read_quadrature()
{
    q_encoder.QE_oldd = q_encoder.QE_old;
    q_encoder.QE_new = digitalRead(quadrature_H) * 2 + digitalRead(quadrature_L);
    q_encoder.QE_code = q_encoder.QE_matrix [q_encoder.QE_old * 4 + q_encoder.QE_new];
    q_encoder.QE_old = q_encoder.QE_new;
    return q_encoder.QE_code;

}

int read_quadrature_edge()
{
    q_encoder.QE_new = digitalRead(quadrature_L);
    if (q_encoder.QE_new == q_encoder.QE_old)
    {
        return 0;
    }
    q_encoder.QE_old = q_encoder.QE_new;
    if (q_encoder.QE_new == HIGH)
    {
        return 0;
    }

    if (digitalRead(quadrature_H))
    {
        return 1;
    }
    else
    {
        return 2;
    }
}
