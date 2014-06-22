double cycles_at_last_hit = 0;
long current_cycles = 0;
long soft_start_down=0, soft_start_up=0; //0 while unpressed, millis() after controller is pressed
long soft_end=0;

void motor_loop() {
  if(calibration.in_progress) {
    current_cycles = calibration.cycles;
  } else {
    current_cycles = encoder.cycle;
  }
  
  if(analogRead(switch_one)==0) {
    update_switch1();
  }
  if(analogRead(switch_two)==0) {
    update_switch2();
  }
  if(analogRead(switch_three)==0) {
    update_switch3();
  }
  
 
  if((current_screws.one || current_screws.two ||  current_screws.three) && abs(current_cycles-cycles_at_last_hit) > 128) { //if we hit screws, correct azimuth
    finish_screws();
  }
  
  //rotate if controller button pressed
  if((millis()-soft_end > end_time_min) && (controllerRead(controller_down) || soft_start_down>0)) {
    soft_end = 0;
    dome.route = 0;
    calibration.in_progress = false;
    calibration.cycles = 0;
    
    if(controllerRead(controller_down) && soft_start_down==0) {
      soft_start_down = millis();
    } else if(!controllerRead(controller_down) && millis()-soft_start_down >= start_time_min) {
      soft_start_down = 0;
    }
    
    digitalWrite(motor_up, LOW);
    digitalWrite(motor_down, HIGH);
  } else if((millis()-soft_end > end_time_min) && (controllerRead(controller_up) || soft_start_up>0)) {
    soft_end = 0;
    dome.route = 0;
    calibration.in_progress = false;
    calibration.cycles = 0;
    
    if(controllerRead(controller_up) && soft_start_up==0) {
      soft_start_up = millis();
    } else if(!controllerRead(controller_up) && millis()-soft_start_up >= start_time_min) {
      soft_start_up = 0;
    }
    
    digitalWrite(motor_up, HIGH);
    digitalWrite(motor_down, LOW);
  } else if((millis()-soft_end > end_time_min) && dome.route == 1) {
    soft_end = 0;
    
    digitalWrite(motor_up, LOW);
    digitalWrite(motor_down, HIGH);
  } else if((millis()-soft_end > end_time_min) && dome.route == -1) {
    soft_end = 0;
    
    digitalWrite(motor_up, HIGH);
    digitalWrite(motor_down, LOW);
  } else {
    if(soft_end==0) {
      soft_end = millis();
    }
    
    digitalWrite(motor_up, LOW);
    digitalWrite(motor_down, LOW);
  }
}

void motor_goto(double azimuth) {
  if(abs(dome.azimuth-azimuth)<minimal_movement || abs(dome.azimuth-360-azimuth)<minimal_movement || abs(dome.azimuth+360-azimuth)<minimal_movement) {
    return; //don't rotate in case that movment is smaller than minimal allowed
  }
  
  dome.target_azimuth = azimuth;
  
  dome.route = closer_route(dome.azimuth, dome.target_azimuth); //determinate route
  
  //apply correction for drift:
  if(dome.route==1) {
    dome.target_azimuth -= calibration.drift;
  } else if(dome.route==-1) {
    dome.target_azimuth += calibration.drift;
  }
  if(dome.target_azimuth>=360) {
    dome.target_azimuth -= 360;
  } else if(dome.target_azimuth<0) {
    dome.target_azimuth += 360;
  }
  
  if(DEBUG) {
    Serial.print("Target azimuth:");
    Serial.println(dome.target_azimuth);
  }
}

int closer_route(double azimuth, double target_azimuth) {
  if(azimuth==target_azimuth) {
    return 0;
  }
  int i = target_azimuth-azimuth;
  if(i<0) {
    i+=360;
  } else if(i>=360) {
    i-=360;
  }
  if(i<180) {
    return 1;
  } else {
    return -1;
  }
}

void update_switch1() {
  current_screws.one = true;
  cycles_at_last_hit = current_cycles;
}

void update_switch2() {
  current_screws.two = true;
  cycles_at_last_hit = current_cycles;
}

void update_switch3() {
  current_screws.three = true;
  cycles_at_last_hit = current_cycles;
}

void finish_screws() {
  double diff = current_cycles-cycles_at_last_hit;
  
  if(calibration.in_progress) {
    detect_screws(diff);
  } else {
    if(DEBUG) {
      Serial.print("Before correction: ");
      Serial.println(dome.azimuth);
    }
    
    //detect which group of screws is hit and correct azimuth to it's azimuth:
    if(current_screws.one==screws1.one && current_screws.two==screws1.two && current_screws.three==screws1.three) {
      if(DEBUG) {
        Serial.println("Detected screws1");
      }
      dome.azimuth = screws1.azimuth+diff/dome.cycles_for_degree;
    } else if(current_screws.one==screws2.one && current_screws.two==screws2.two && current_screws.three==screws2.three) {
      if(DEBUG) {
        Serial.println("Detected screws2");
      }
      dome.azimuth = screws2.azimuth+diff/dome.cycles_for_degree;
    } else if(current_screws.one==screws3.one && current_screws.two==screws3.two && current_screws.three==screws3.three) {
      if(DEBUG) {
        Serial.println("Detected screws3");
      }
      dome.azimuth = screws3.azimuth+diff/dome.cycles_for_degree;
    } else if(current_screws.one==screws4.one && current_screws.two==screws4.two && current_screws.three==screws4.three) {
      if(DEBUG) {
        Serial.println("Detected screws4");
      }
      dome.azimuth = screws4.azimuth+diff/dome.cycles_for_degree;
    } else {
      if(DEBUG) {
        Serial.print("Error, no screws detected on azimuth: ");
        Serial.println(dome.azimuth);
      }
    }
    
    if(DEBUG) {
      Serial.print("After correction: ");
      Serial.println(dome.azimuth);
    }
  }
  
  current_screws.one = false;
  current_screws.two = false;
  current_screws.three = false;
}
