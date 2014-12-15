void motors_loop()
{
    if (_motors.rotation == STOP) {
        if (status_buffer.rotation != STOP) {
            if (millis() - _motors.spindown_time < settings_buffer.minimal_spindown_time) {
                return;
            }
            
            _motors.rotation = status_buffer.rotation;
            _motors.spinup_time = millis();
        }
    }
    
    if (_motors.rotation != STOP) {
        if (_motors.rotation != status_buffer.rotation) {
            if (millis() - _motors.spinup_time < settings_buffer.minimal_spinup_time) {
                return;
            }
            
            _motors.rotation = STOP;
            _motors.spindown_time = millis();
        }
    }
    
    if (_motors.rotation == DOWN) {
        digitalWrite(motor_up, LOW);
        digitalWrite(motor_down, HIGH);
    } else if (_motors.rotation == UP) {
        digitalWrite(motor_up, HIGH);
        digitalWrite(motor_down, LOW);
    } else  {
        digitalWrite(motor_up, LOW);
        digitalWrite(motor_down, LOW);
    }
}
