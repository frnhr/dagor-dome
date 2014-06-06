void encoder_loop() {
  if (encoder.last_cycle != encoder.cycle) {
    if(calibration.in_progress) {
      if(encoder.cycle >= encoder.last_cycle) {
        calibration.cycles++;
      } else if(encoder.cycle <= encoder.last_cycle) {
        calibration.cycles--;
      }
    } else if(dome.cycles_for_degree!=0) {
      dome.azimuth += (encoder.cycle-encoder.last_cycle)/dome.cycles_for_degree;
      if(dome.azimuth >= 360) {
        dome.azimuth -= 360;
      } else if(dome.azimuth < 0) {
        dome.azimuth += 360;
      }
      
      if(dome.route!=closer_route(dome.azimuth, dome.target_azimuth) && !calibration.measuring_drift && dome.route!=0) { //did we passed target azimuth?
        dome.route = 0;
        Serial.println("dd");
      }
    }
    
    encoder.last_cycle = encoder.cycle;
  } else if(calibration.measuring_drift && millis()-calibration.drift_start_time>=end_time_min*1.5) {
    calibration.drift = abs(dome.azimuth - calibration.drift_start_azimuth);
    if(calibration.drift>=360) {
      calibration.drift -= 360;
    }
    
    calibration.measuring_drift = false;
    Serial.println("cd"); //calibration is done
    
    if(DEBUG) {
      Serial.print("Drift: ");
      Serial.println(calibration.drift);
    }
  }
}

boolean in_interupt() {
  return !(SREG / 128);
}

int read_quadrature() {
  encoder.QE_oldd = encoder.QE_old;
  encoder.QE_new = digitalRead(quadrature_H) * 2 + digitalRead(quadrature_L);
  encoder.QE_code = encoder.QE_matrix [encoder.QE_old * 4 + encoder.QE_new];
  encoder.QE_old = encoder.QE_new;
  return encoder.QE_code;

}

int read_quadrature_edge() {
  encoder.QE_new = digitalRead(quadrature_L);
  if (encoder.QE_new == encoder.QE_old) {
    return 0;
  }
  encoder.QE_old = encoder.QE_new;
  if (encoder.QE_new == HIGH) {
    return 0;
  }
  
  if (digitalRead(quadrature_H)) {
    return 1;
  } else {
    return 2;
  }
}

int count_quadrature(int value) {
  if (in_interupt()) {
    encoder.QE_code = read_quadrature_edge();
  } else {
    encoder.QE_code = read_quadrature();
  }

  if (encoder.QE_code == 1) {
    value++;
  }
  if (encoder.QE_code == 2) {
    value--;
  }
  
  return value;
}

void update_QE() {
  encoder.cycle = count_quadrature(encoder.cycle);
}
