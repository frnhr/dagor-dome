

void motor_loop()
{
    if(motors.rotation == STOP) {
        if(status_buffer.rotation != STOP) {
            if(millis() - motors.spindown_time < settings.minimal_spindown_time) {
                return;
            }
            
            motors.rotation = status_buffer.rotation;
            motors.spinup_time = millis();
        }
    }
    
    if(motors.rotation != STOP) {
        if(motors.rotation != status_buffer.rotation) {
            if(millis() - motors.spinup_time < settings.minimal_spinup_time) {
                return;
            }
            
            motors.rotation = STOP;
            motors.spindown_time = millis();
        }
    }
    
    if(motors.rotation == DOWN) {
        digitalWrite(motor_up, LOW);
        digitalWrite(motor_down, HIGH);
    } else  if(motors.rotation == UP) {
        digitalWrite(motor_up, HIGH);
        digitalWrite(motor_down, LOW);
    } else  {
        digitalWrite(motor_up, LOW);
        digitalWrite(motor_down, LOW);
    }
}
