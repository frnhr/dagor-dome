#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"

boolean DEBUG = true;

//
// Hardware configuration
//
int rele_open = 5;
int rele_close = 8;

unsigned long current_door_state = 3;
unsigned long last_door_state = 3;
unsigned long commanded_door_state = 3;
unsigned long state_change_timeout = 0;
unsigned long state_change_start = 0;
unsigned long state_change_time = 10000;

unsigned long last_radio_recieve_millis = 0;
unsigned long radio_wd_time = 4000;

int led = 6;
boolean led_state = HIGH;
unsigned long led_timer = 300;
unsigned long led_time_1 = 10000;
unsigned long led_time_2 = 1000;
unsigned long led_timer_12 = 500;
unsigned long led_time = led_time_2;

// Set up nRF24L01 radio on SPI bus plus pins 9 & 10
RF24 radio(9,10);

//
// Topology
//
// Radio pipe addresses for the 2 nodes to communicate.
const uint64_t pipes[2] = { 0xF0F0F0F0E1LL, 0xE8E8F0F0E1LL };


void setup(void) {
  //reset hack
  digitalWrite(7, HIGH);
  pinMode(7,OUTPUT);
  
  pinMode(rele_open, OUTPUT);       
  pinMode(rele_close, OUTPUT);       
  digitalWrite(rele_open, LOW);
  digitalWrite(rele_close, LOW);
  
  pinMode(led, OUTPUT);       
  digitalWrite(led, led_state);
  
  //
  // Setup and configure rf radio
  //
  radio.begin();
  
  // optionally, increase the delay between retries & # of retries
  radio.setRetries(15, 15);
  
  // optionally, reduce the payload size.      seems to
  // improve reliability
  radio.setPayloadSize(8);
  
  radio.openWritingPipe(pipes[1]);
  radio.openReadingPipe(1, pipes[0]);
  
  if(DEBUG) {
    Serial.begin(115200);
    Serial.println("BEGIN");
  }
  
  radio.startListening();
}

void door_loop() {
  if (state_change_timeout > 0 && millis() - state_change_start < state_change_timeout) {
    return;
  } else {
    state_change_timeout = 0;
    
    unsigned long next_door_state = current_door_state;
    
    // the logic if stopped
    if (current_door_state == 3) {                                    // currently STOPPED
      if (commanded_door_state != 3) {                                    // and commanded to MOVE
        next_door_state = commanded_door_state;
        state_change_timeout = state_change_time;
      }
    }
          
    // the logic if moving
    if (current_door_state != 3) {                                     // currently MOVING
      if (commanded_door_state == 3) {                                    // and commanded to STOP
        next_door_state = 3;
        state_change_timeout = state_change_time;
      }
      if (commanded_door_state != current_door_state) {      // and commanded to CHANGE DIRECTION
        next_door_state = 3;
        state_change_timeout = state_change_time;
      }
    }
    
    if (state_change_timeout > 0) {
      state_change_start = millis();
    }
    current_door_state = next_door_state;
  }
  
  if (current_door_state == 1) {
    digitalWrite(rele_open, HIGH);
    digitalWrite(rele_close, LOW);
  } else if(current_door_state == 2) {
    digitalWrite(rele_open, LOW);
    digitalWrite(rele_close, HIGH);
  } else if(current_door_state == 3) {
    digitalWrite(rele_open, LOW);
    digitalWrite(rele_close, LOW);
  }
}


void loop(void) {
  led_timer++;
  if (led_timer > led_time) {
    led_state = ! led_state;
    led_timer = 0;      
    digitalWrite(led, led_state);
  }
  
  //
  // Receive each packet, dump it out, and send it back
  //
  if (radio.available()) {
    // Dump the payloads until we've gotten everything
    unsigned long door_action;
    boolean done = false;
    while (!done) {
      // Fetch the payload, and see if this was the last one.
      done = radio.read(&door_action, sizeof(unsigned long));
      
      
      if(DEBUG) {
        Serial.print("Got payload:");
        Serial.println(door_action);
      }
      
      // Delay just a little bit to let the other unit
      // make the transition to receiver
      delay(20);
    }
    
    if (door_action >= 1 && door_action <= 3) {
      last_radio_recieve_millis = millis();
      led_time = led_time_1;
    } 
    
    // First, stop listening so we can talk
    radio.stopListening();
    
    // Send the final one back.
    radio.write(&door_action, sizeof(unsigned long));
    
    commanded_door_state = door_action;
    
    // Now, resume listening so we catch the next packets.
    radio.startListening();
  }
  	
  if (millis() - last_radio_recieve_millis > led_timer_12) {
    led_time = led_time_2;
  }
  
  if (millis() - last_radio_recieve_millis > radio_wd_time) {
    // reset!
    if(DEBUG) Serial.println("RESET");
    delay(100);
    digitalWrite(7, LOW); //Pulling the RESET pin LOW triggers the reset. 
  }
  
  door_loop();
}
