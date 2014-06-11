void radio_loop() {
  if(digitalRead(controller_close)) {
    door_close();
  } else if(digitalRead(controller_open)) {
    door_open();
  }
}

void door_open() {
  digitalWrite(open_pin, LOW);
  digitalWrite(close_pin, HIGH);
}

void door_close() {
  digitalWrite(open_pin, HIGH);
  digitalWrite(close_pin, LOW);
}
